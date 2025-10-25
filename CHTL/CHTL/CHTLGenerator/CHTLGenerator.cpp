#include "CHTLGenerator.hpp"
#include "CHTLNode/StyleNode.hpp"

std::string CHTLGenerator::generate(const AstNode& root) {
    visit(root);
    return html_;
}

void CHTLGenerator::visit(const AstNode& node) {
    if (const ElementNode* element = dynamic_cast<const ElementNode*>(&node)) {
        visitElementNode(*element);
    } else if (const TextNode* text = dynamic_cast<const TextNode*>(&node)) {
        visitTextNode(*text);
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
        for (const auto& child : node.children) {
            if (const StyleNode* style_node = dynamic_cast<const StyleNode*>(child.get())) {
                for (const auto& prop : style_node->properties) {
                    style_string += prop->key + ":" + prop->value + ";";
                }
            }
        }
        if (!style_string.empty()) {
            html_ += " style=\"" + style_string + "\"";
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
