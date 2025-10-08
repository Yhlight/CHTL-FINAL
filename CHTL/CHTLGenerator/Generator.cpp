#include "Generator.h"
#include "StyleCollector.h"
#include "AttributeInjector.h"
#include "ExpressionEvaluator.h"
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLNode/InsertNode.h"
#include <set>
#include <sstream>
#include <algorithm>

// Helper to convert an evaluated value to a string for CSS
static std::string evaluatedValueToString(const EvaluatedValue& val) {
    if (val.isNumeric) {
        std::string s = std::to_string(val.value);
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s + val.unit;
    }
    return val.stringValue;
}

std::string Generator::generate(RootNode& root, CHTLContext& context) {
    this->context = &context;

    // Pass 1: Collect all style rules and their contexts
    StyleCollector collector;
    collector.collect(root);
    this->collectedRules = collector.collectedRules;

    // Pass 2: Inject attributes into the AST based on style rules
    AttributeInjector injector;
    injector.inject(root);

    // Pass 3: Render the final document from the modified AST
    output.str("");
    output.clear();
    indentLevel = 0;
    root.accept(*this);
    return output.str();
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  ";
    }
}

void Generator::visit(RootNode& node) {
    for (auto& child : node.children) {
        child->accept(*this);
    }
}

void Generator::visit(ElementNode& node) {
    indent();
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    if (node.style && (!node.style->properties.empty() || !node.style->templateUsages.empty())) {
        output << " style=\"";
        for (size_t i = 0; i < node.style->properties.size(); ++i) {
            node.style->properties[i]->accept(*this);
        }
        for (size_t i = 0; i < node.style->templateUsages.size(); ++i) {
            node.style->templateUsages[i]->accept(*this);
        }
        output << "\"";
    }

    bool hasContent = !node.children.empty() || (node.tagName == "head" && !collectedRules.empty());
    if (!hasContent) {
        output << "></" << node.tagName << ">\n";
        return;
    }

    output << ">\n";
    indentLevel++;

    if (node.tagName == "head" && !collectedRules.empty()) {
        ExpressionEvaluator evaluator;
        indent();
        output << "<style>\n";
        indentLevel++;
        for (const auto& rulePair : collectedRules) {
            StyleRuleNode* rule = rulePair.first;
            ElementNode* parentElement = rulePair.second;

            std::string finalSelector = rule->selector;
            if (finalSelector.find('&') != std::string::npos) {
                std::string primarySelector;
                if (parentElement->attributes.count("id")) {
                    primarySelector = "#" + parentElement->attributes.at("id");
                } else if (parentElement->attributes.count("class")) {
                    std::stringstream ss(parentElement->attributes.at("class"));
                    std::string firstClass;
                    ss >> firstClass;
                    primarySelector = "." + firstClass;
                }

                if(!primarySelector.empty()) {
                    finalSelector.replace(finalSelector.find('&'), 1, primarySelector);
                } else {
                    finalSelector.erase(finalSelector.find('&'), 1);
                }
            }

            indent();
            output << finalSelector << " {\n";
            indentLevel++;
            for (const auto& prop : rule->properties) {
                indent();
                EvaluatedValue val = evaluator.evaluate(*prop->value);
                output << prop->key << ": " << evaluatedValueToString(val) << ";\n";
            }
            indentLevel--;
            indent();
            output << "}\n";
        }
        indentLevel--;
        indent();
        output << "</style>\n";
    }

    for (auto& child : node.children) {
        child->accept(*this);
    }

    indentLevel--;
    indent();
    output << "</" << node.tagName << ">\n";
}

void Generator::visit(TextNode& node) {
    indent();
    output << node.content << "\n";
}

void Generator::visit(CommentNode& node) {
    if (node.type == TokenType::GENERATOR_COMMENT) {
        indent();
        size_t firstSpace = node.content.find(' ');
        std::string content = (firstSpace != std::string::npos)
                              ? node.content.substr(firstSpace + 1)
                              : node.content;
        output << "<!-- " << content << " -->\n";
    }
}

void Generator::visit(StyleNode& node) {
    for (auto& usage : node.templateUsages) {
        usage->accept(*this);
    }
    for (auto& prop : node.properties) {
        prop->accept(*this);
    }
}

void Generator::visit(StylePropertyNode& node) {
    ExpressionEvaluator evaluator;
    EvaluatedValue val = evaluator.evaluate(*node.value);
    output << node.key << ": " << evaluatedValueToString(val) << "; ";
}

void Generator::visit(StyleRuleNode& node) {}
void Generator::visit(TemplateDeclarationNode& node) {}
void Generator::visit(CustomDeclarationNode& node) {}
void Generator::visit(DeleteNode& node) {}
void Generator::visit(InsertNode& node) {}

void Generator::visit(TemplateUsageNode& node) {
    if (!context) return;

    if (node.templateType == TemplateType::STYLE) {
        if (context->styleTemplates.count(node.name)) {
            auto* tpl = context->styleTemplates[node.name].get();
            for (const auto& bodyNode : tpl->body) {
                bodyNode->accept(*this);
            }
        }
    } else if (node.templateType == TemplateType::ELEMENT) {
        if (context->elementTemplates.count(node.name)) {
            auto* tpl = context->elementTemplates[node.name].get();
            for (const auto& bodyNode : tpl->body) {
                bodyNode->accept(*this);
            }
        } else if (context->customTemplates.count(node.name)) {
            auto* tpl = context->customTemplates[node.name].get();

            // 1. Deep copy the template body
            std::vector<std::unique_ptr<BaseNode>> clonedBody;
            for(const auto& n : tpl->body) {
                clonedBody.push_back(n->clone());
            }

            // 2. Apply specializations to the copy
            for (const auto& spec : node.specializations) {
                if (auto* deleteRule = dynamic_cast<DeleteNode*>(spec.get())) {
                    for (const auto& target : deleteRule->targets) {
                        clonedBody.erase(std::remove_if(clonedBody.begin(), clonedBody.end(),
                            [&](const std::unique_ptr<BaseNode>& n) {
                                if (auto* el = dynamic_cast<ElementNode*>(n.get())) {
                                    return el->tagName == target;
                                }
                                return false;
                            }), clonedBody.end());
                    }
                } else if (auto* insertRule = dynamic_cast<InsertNode*>(spec.get())) {
                    auto it = std::find_if(clonedBody.begin(), clonedBody.end(),
                        [&](const std::unique_ptr<BaseNode>& n) {
                            if (auto* el = dynamic_cast<ElementNode*>(n.get())) {
                                // Simplified selector matching for now
                                return el->tagName == insertRule->targetSelector;
                            }
                            return false;
                        });

                    if (it != clonedBody.end()) {
                        if (insertRule->position == InsertPosition::AFTER) {
                            it++; // Get iterator to the position *after* the found element
                        }
                        // Insert the new nodes
                        for(const auto& nodeToInsert : insertRule->nodesToInsert) {
                            it = clonedBody.insert(it, nodeToInsert->clone());
                            it++; // Move iterator past the inserted element
                        }
                    }
                }
            }

            // 3. Render the modified body
            for (const auto& bodyNode : clonedBody) {
                bodyNode->accept(*this);
            }
        }
    }
}