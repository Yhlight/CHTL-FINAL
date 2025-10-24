#include "HTMLGenerator.h"
#include <iostream>
namespace CHTL {

std::string HTMLGenerator::generate(const ASTNode& root) {
    root.accept(*this);
    return m_output.str();
}

void HTMLGenerator::visit(const TextNode& node) {
    m_output << node.getText();
}

void HTMLGenerator::visit(const ElementNode& node) {
    if (node.getTagName() == "root") {
        for (const auto& child : node.getChildren()) {
            child->accept(*this);
        }
        return;
    }

    m_output << "<" << node.getTagName();

    std::string style_string;
    for (const auto& child : node.getChildren()) {
        if (dynamic_cast<StyleNode*>(child.get())) {
            std::stringstream ss;
            const auto& properties = dynamic_cast<StyleNode*>(child.get())->getProperties();
            for (const auto& prop : properties) {
                ss << prop.key << ": " << prop.value << ";";
            }
            style_string = ss.str();
        }
    }


    bool has_style_attr = false;
    for (const auto& attr : node.getAttributes()) {
        m_output << " " << attr.key << "=\"" << attr.value;
         if (attr.key == "style") {
            has_style_attr = true;
            m_output << style_string;
        }
        m_output << "\"";
    }

    if (!style_string.empty() && !has_style_attr) {
         m_output << " style=\"" << style_string << "\"";
    }


    m_output << ">";

    for (const auto& child : node.getChildren()) {
        if (!dynamic_cast<StyleNode*>(child.get())) {
            child->accept(*this);
        }
    }

    m_output << "</" << node.getTagName() << ">";
}

void HTMLGenerator::visit(const StyleNode& node) {
    // Handled by the ElementNode visitor, so this does nothing.
}

} // namespace CHTL
