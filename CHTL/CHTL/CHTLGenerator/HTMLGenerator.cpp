#include "HTMLGenerator.h"
#include <iostream>
#include <map>

namespace CHTL {

std::string HTMLGenerator::generate(const ASTNode& root) {
    std::stringstream final_output;
    root.accept(*this);

    final_output << "<html><head><style>" << m_global_css.str() << "</style></head><body>"
                 << m_output.str() << "</body></html>";

    return final_output.str();
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

    // Collect attributes from the node itself
    std::map<std::string, std::string> attributes;
    for(const auto& attr : node.getAttributes()){
        attributes[attr.key] = attr.value;
    }

    // Process style nodes to auto-add attributes and collect styles
    std::stringstream inline_style_stream;
    for (const auto& child : node.getChildren()) {
        if (auto* style_node = dynamic_cast<const StyleNode*>(child.get())) {
            for(const auto& prop : style_node->getInlineProperties()) {
                inline_style_stream << prop.key << ":" << prop.value << ";";
            }
            for(const auto& selector : style_node->getSelectors()) {
                if (selector.type == Selector::Type::Class) {
                    attributes["class"] += (attributes["class"].empty() ? "" : " ") + selector.name;
                } else if (selector.type == Selector::Type::Id) {
                    attributes["id"] = selector.name; // ID should be unique
                }
            }
        }
    }

    if (inline_style_stream.tellp() > 0) {
        attributes["style"] += inline_style_stream.str();
    }

    // Write out the collected attributes
    for(const auto& [key, value] : attributes) {
        m_output << " " << key << "=\"" << value << "\"";
    }

    m_output << ">";

    // Visit non-style children to generate their content
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    m_output << "</" << node.getTagName() << ">";
}

void HTMLGenerator::visit(const StyleNode& node) {
    // Determine the context for '&' selectors. The CHTL spec says this is the
    // first class or ID selector in the style block.
    std::string context_selector_str;
    for (const auto& selector : node.getSelectors()) {
        if (selector.type == Selector::Type::Class) {
            context_selector_str = "." + selector.name;
            break;
        }
        if (selector.type == Selector::Type::Id) {
            context_selector_str = "#" + selector.name;
            break;
        }
    }

    // Generate CSS rules for selectors and add them to the global stylesheet
    for (const auto& selector : node.getSelectors()) {
        if (selector.type == Selector::Type::Class) {
            m_global_css << "." << selector.name;
        } else if (selector.type == Selector::Type::Id) {
            m_global_css << "#" << selector.name;
        } else if (selector.type == Selector::Type::Context) {
            if (context_selector_str.empty()) {
                // This could be a user error, but we'll handle it gracefully.
                 std::cerr << "Warning: Context selector '&' used without a class or id context." << std::endl;
                 m_global_css << selector.name; // e.g., just output ':hover'
            } else {
                m_global_css << context_selector_str << selector.name;
            }
        }
        m_global_css << " { ";
        for (const auto& prop : selector.properties) {
            m_global_css << prop.key << ": " << prop.value << "; ";
        }
        m_global_css << "}\n";
    }
}

} // namespace CHTL
