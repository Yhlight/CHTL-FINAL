#include "CHTLGenerator.h"
#include <stdexcept>

CHTLGenerator::CHTLGenerator(std::unique_ptr<ProgramNode> ast) : ast(std::move(ast)) {}

std::string CHTLGenerator::generate() {
    // First pass to collect all style templates
    for (const auto& stmt : ast->getStatements()) {
        if (auto* st = dynamic_cast<StyleTemplateNode*>(stmt.get())) {
            styleTemplates[st->getName()] = st;
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
    }
    // Other node types will be handled in later steps
    return "";
}

std::string valueNodeToString(const ValueNode* valueNode) {
    if (const auto* literal = dynamic_cast<const LiteralValueNode*>(valueNode)) {
        if (literal->getIsString()) {
            return "\"" + literal->getValue() + "\"";
        }
        return literal->getValue();
    }
    // Handle other ValueNode types if necessary
    return "";
}

std::string CHTLGenerator::generateElementNode(ElementNode* node) {
    std::string html = "<" + node->getTagName();
    for (const auto& attr : node->getAttributes()) {
        html += " " + attr.first + "=\"" + valueNodeToString(attr.second.get()) + "\"";
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
            styles += "  " + prop->getKey() + ": " + valueNodeToString(prop->getValue()) + ";\n";
        } else if (auto* usage = dynamic_cast<StyleTemplateUsageNode*>(item.get())) {
            auto it = styleTemplates.find(usage->getName());
            if (it != styleTemplates.end()) {
                for (const auto& templateItem : it->second->getBody()->getItems()) {
                    if (auto* templateProp = dynamic_cast<StylePropertyNode*>(templateItem.get())) {
                        styles += "  " + templateProp->getKey() + ": " + valueNodeToString(templateProp->getValue()) + ";\n";
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
