#include "CHTLGenerator.hpp"
#include "CHTLNode/StyleNode.hpp"
#include "CHTLNode/RuleNode.hpp"

Document CHTLGenerator::generate(const AstNode& root) {
    visit(root);
    return {html_, css_};
}

void CHTLGenerator::visit(const AstNode& node) {
    if (const ElementNode* element = dynamic_cast<const ElementNode*>(&node)) {
        visitElementNode(*element);
    } else if (const TextNode* text = dynamic_cast<const TextNode*>(&node)) {
        visitTextNode(*text);
    } else if (const RuleNode* rule = dynamic_cast<const RuleNode*>(&node)) {
        visitRuleNode(*rule);
    }
}

void CHTLGenerator::visitElementNode(const ElementNode& node) {
    if (node.tag_name == "text") {
        for (const auto& child : node.children) {
            visit(*child);
        }
    } else {
        html_ += "<" + node.tag_name;
        for (const auto& attr : node.attributes) {
            html_ += " " + attr.first + "=\"" + attr.second + "\"";
        }

        std::string style_string = "";
        std::string class_string = "";
        for (const auto& child : node.children) {
            if (const StyleNode* style_node = dynamic_cast<const StyleNode*>(child.get())) {
                for (const auto& prop : style_node->properties) {
                    style_string += prop->key + ":" + prop->value + ";";
                }
                for (const auto& rule : style_node->rules) {
                    if (rule->selector[0] == '.') {
                        class_string += rule->selector.substr(1) + " ";
                    }
                    visit(*rule);
                }
            }
        }
        if (!style_string.empty()) {
            html_ += " style=\"" + style_string + "\"";
        }
        if (!class_string.empty()) {
            class_string.pop_back(); // remove trailing space
            html_ += " class=\"" + class_string + "\"";
        }

        html_ += ">";
        for (const auto& child : node.children) {
            if (!dynamic_cast<const StyleNode*>(child.get())) {
                visit(*child);
            }
        }
        html_ += "</" + node.tag_name + ">";
    }
}

void CHTLGenerator::visitTextNode(const TextNode& node) {
    html_ += node.text;
}

void CHTLGenerator::visitRuleNode(const RuleNode& node) {
    css_ += node.selector + "{";
    for (const auto& prop : node.properties) {
        css_ += prop->key + ":" + prop->value + ";";
    }
    css_ += "}";
}
