#include "CHTLGenerator.h"
#include "../CHTLParser/ASTNode.h"
#include <stdexcept>

CHTLGenerator::CHTLGenerator(std::unique_ptr<ProgramNode> ast) : ast(std::move(ast)) {}

std::string CHTLGenerator::generate() {
    // First pass to collect all templates
    for (const auto& stmt : ast->getStatements()) {
        if (auto* st = dynamic_cast<StyleTemplateNode*>(stmt.get())) {
            styleTemplates[st->getName()] = st;
        } else if (auto* et = dynamic_cast<ElementTemplateNode*>(stmt.get())) {
            elementTemplates[et->getName()] = et;
        } else if (auto* vt = dynamic_cast<VarTemplateNode*>(stmt.get())) {
            varTemplates[vt->getName()] = vt;
        } else if (auto* cst = dynamic_cast<CustomStyleTemplateNode*>(stmt.get())) {
            customStyleTemplates[cst->getName()] = cst;
        }
    }

    std::string html = generateNode(ast.get());
    return "<style>\n" + css + "</style>\n" + html;
}

std::string CHTLGenerator::generateNode(ASTNode* node) {
    if (auto* p = dynamic_cast<ProgramNode*>(node)) {
        std::string result;
        for (const auto& stmt : p->getStatements()) {
            result += generateNode(stmt.get());
        }
        return result;
    } else if (auto* e = dynamic_cast<ElementNode*>(node)) {
        return generateElementNode(e);
    } else if (auto* t = dynamic_cast<TextNode*>(node)) {
        return generateTextNode(t);
    } else if (auto* s = dynamic_cast<StyleNode*>(node)) {
        // We need a parent selector for context, but for now, we'll pass an empty string.
        // This will be improved in a later step.
        generateStyleNode(s, "");
    } else if (auto* st = dynamic_cast<StyleTemplateNode*>(node)) {
        // Already processed, do nothing
    } else if (auto* cst = dynamic_cast<CustomStyleTemplateNode*>(node)) {
        // Already processed, do nothing
    } else if (auto* vt = dynamic_cast<VarTemplateNode*>(node)) {
        // Already processed, do nothing
    } else if (auto* et = dynamic_cast<ElementTemplateNode*>(node)) {
        // Already processed, do nothing
    } else if (auto* etu = dynamic_cast<ElementTemplateUsageNode*>(node)) {
        auto it = elementTemplates.find(etu->getName());
        if (it != elementTemplates.end()) {
            std::string result;
            for (const auto& child : it->second->getBody()) {
                result += generateNode(child.get());
            }
            return result;
        }
    }
    // Other node types will be handled in later steps
    return "";
}

std::string CHTLGenerator::valueNodeToString(const ValueNode* valueNode) {
    if (const auto* literal = dynamic_cast<const LiteralValueNode*>(valueNode)) {
        if (literal->getIsString()) {
            return "\"" + literal->getValue() + "\"";
        }
        return literal->getValue();
    } else if (const auto* usage = dynamic_cast<const TemplateVarUsageNode*>(valueNode)) {
        auto it = varTemplates.find(usage->getTemplateName());
        if (it != varTemplates.end()) {
            const auto& vars = it->second->getVariables();
            auto varIt = vars.find(usage->getVarName());
            if (varIt != vars.end()) {
                return valueNodeToString(varIt->second.get());
            }
        }
    }
    return "";
}

std::string CHTLGenerator::generateElementNode(ElementNode* node) {
    std::string html = "<" + node->getTagName();
    for (const auto& attr : node->getAttributes()) {
        html += " " + attr.first + "=" + this->valueNodeToString(attr.second.get());
    }
    html += ">";
    for (const auto& child : node->getChildren()) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            generateStyleNode(styleNode, node->getTagName());
        } else {
            html += generateNode(child.get());
        }
    }
    html += "</" + node->getTagName() + ">";
    return html;
}

std::string CHTLGenerator::generateTextNode(TextNode* node) {
    return node->getText();
}

void CHTLGenerator::generateStyleNode(StyleNode* node, const std::string& parentSelector) {
    std::string styles;
    for (const auto& item : node->getItems()) {
        if (auto* prop = dynamic_cast<StylePropertyNode*>(item.get())) {
            styles += "  " + prop->getKey() + ": " + this->valueNodeToString(prop->getValue()) + ";\n";
        } else if (auto* usage = dynamic_cast<StyleTemplateUsageNode*>(item.get())) {
            auto it = styleTemplates.find(usage->getName());
            if (it != styleTemplates.end()) {
                for (const auto& templateItem : it->second->getBody()->getItems()) {
                    if (auto* templateProp = dynamic_cast<StylePropertyNode*>(templateItem.get())) {
                        styles += "  " + templateProp->getKey() + ": " + this->valueNodeToString(templateProp->getValue()) + ";\n";
                    }
                }
            }
        } else if (auto* usage = dynamic_cast<CustomStyleUsageNode*>(item.get())) {
            auto it = customStyleTemplates.find(usage->getName());
            if (it != customStyleTemplates.end()) {
                if (const auto* body = dynamic_cast<const StyleNode*>(usage->getBody())) {
                    for (const auto& bodyItem : body->getItems()) {
                        if (auto* bodyProp = dynamic_cast<StylePropertyNode*>(bodyItem.get())) {
                            styles += "  " + bodyProp->getKey() + ": " + this->valueNodeToString(bodyProp->getValue()) + ";\n";
                        }
                    }
                }
            }
        }
    }

    if (!parentSelector.empty()) {
        css += parentSelector + " {\n" + styles + "}\n";
    } else {
        css += styles;
    }
}

void CHTLGenerator::generateStyleTemplateNode(StyleTemplateNode* node) {
    // This function is not used directly, as style templates are processed on demand.
}
