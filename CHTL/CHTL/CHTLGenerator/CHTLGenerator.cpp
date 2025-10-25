#include "CHTLGenerator.hpp"

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
        html_ += ">";
        for (const auto& child : node.children) {
            visit(*child);
        }
        html_ += "</" + node.tag_name + ">";
    }
}

void CHTLGenerator::visitTextNode(const TextNode& node) {
    html_ += node.text;
}
