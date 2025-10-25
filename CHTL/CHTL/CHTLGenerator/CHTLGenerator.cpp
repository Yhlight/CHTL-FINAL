#include "CHTLGenerator.hpp"
#include "CHTLNode/StyleNode.hpp"
#include "CHTLNode/RuleNode.hpp"
#include "CHTLNode/ExpressionNode.hpp"
#include "CHTLNode/LiteralNode.hpp"
#include "CHTLNode/BinaryOpNode.hpp"

Document CHTLGenerator::generate(const AstNode& root) {
    visit(root);
    return {html_, css_, js_};
}

void CHTLGenerator::visit(const AstNode& node) {
    if (const ElementNode* element = dynamic_cast<const ElementNode*>(&node)) {
        visitElementNode(*element);
    } else if (const TextNode* text = dynamic_cast<const TextNode*>(&node)) {
        visitTextNode(*text);
    } else if (const RuleNode* rule = dynamic_cast<const RuleNode*>(&node)) {
        visitRuleNode(*rule);
    } else if (const ScriptNode* script = dynamic_cast<const ScriptNode*>(&node)) {
        visitScriptNode(*script);
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
                    style_string += prop->key + ":" + evaluator_.evaluate(*prop->value) + ";";
                }
                for (const auto& rule : style_node->rules) {
                    if (rule->selector[0] == '.') {
                        class_string += rule->selector.substr(1) + " ";
                    }
                    visit(*rule);
                }
            } else if (const ScriptNode* script_node = dynamic_cast<const ScriptNode*>(child.get())) {
                visit(*script_node);
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
            if (!dynamic_cast<const StyleNode*>(child.get()) && !dynamic_cast<const ScriptNode*>(child.get())) {
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
        css_ += prop->key + ":" + evaluator_.evaluate(*prop->value) + ";";
    }
    css_ += "}";
}

void CHTLGenerator::visitScriptNode(const ScriptNode& node) {
    js_ += node.content;
}
