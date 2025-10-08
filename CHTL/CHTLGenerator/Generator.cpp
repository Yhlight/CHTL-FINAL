#include "Generator.h"

std::string Generator::generate(RootNode& root) {
    output.str(""); // Clear the stream
    output.clear();
    indentLevel = 0;
    root.accept(*this);
    return output.str();
}

void Generator::visit(RootNode& node) {
    for (auto& child : node.children) {
        child->accept(*this);
    }
}

void Generator::visit(ElementNode& node) {
    indent();
    output << "<" << node.tagName;

    // Append attributes
    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    output << ">";

    if (!node.children.empty()) {
        output << "\n";
        indentLevel++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indentLevel--;
        indent();
    }
    output << "</" << node.tagName << ">\n";
}

void Generator::visit(TextNode& node) {
    indent();
    output << node.content << "\n";
}

void Generator::visit(CommentNode& node) {
    if (node.type == TokenType::GENERATOR_COMMENT) {
        indent();
        // The spec says "# comment", so we find the first space and take the rest.
        size_t firstSpace = node.content.find(' ');
        std::string content = (firstSpace != std::string::npos)
                              ? node.content.substr(firstSpace + 1)
                              : node.content;
        output << "<!-- " << content << " -->\n";
    }
    // Other comment types are ignored by the generator.
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}