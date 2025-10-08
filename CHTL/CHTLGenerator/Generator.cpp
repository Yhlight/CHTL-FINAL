#include "Generator.h"
#include "StyleCollector.h"
#include "AttributeInjector.h"
#include "ExpressionEvaluator.h"
#include <set>
#include <sstream>

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

std::string Generator::generate(RootNode& root) {
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

    if (node.style && !node.style->properties.empty()) {
        ExpressionEvaluator evaluator;
        output << " style=\"";
        for (size_t i = 0; i < node.style->properties.size(); ++i) {
            auto& prop = node.style->properties[i];
            EvaluatedValue val = evaluator.evaluate(*prop->value);
            output << prop->key << ": " << evaluatedValueToString(val) << ";";
            if (i < node.style->properties.size() - 1) output << " ";
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

void Generator::visit(StyleNode& node) {}
void Generator::visit(StylePropertyNode& node) {}
void Generator::visit(StyleRuleNode& node) {}