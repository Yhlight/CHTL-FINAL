#include "CMOD/packer.h"
#include "nodes/TemplateDefinitionNode.h"
#include "nodes/CustomDefinitionNode.h"
#include "nodes/StylePropertyNode.h"
#include "nodes/ExpressionNode.h"
#include <fstream>
#include <stdexcept>
#include <vector>

namespace CHTL {
namespace CMOD {

// Helper function to write a string to a binary file
void write_string(std::ofstream& out, const std::string& str) {
    size_t len = str.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(str.c_str(), len);
}

// Recursive function to serialize a node
void serialize_node(std::ofstream& out, const AstNode& node) {
    // Write node type
    NodeType type = node.GetType();
    out.write(reinterpret_cast<const char*>(&type), sizeof(type));

    // Serialize based on node type
    switch (type) {
        case NodeType::Program: {
            const auto& program_node = static_cast<const ProgramNode&>(node);
            size_t child_count = program_node.children.size();
            out.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
            for (const auto& child : program_node.children) {
                serialize_node(out, *child);
            }
            break;
        }
        case NodeType::Element: {
            const auto& element_node = static_cast<const ElementNode&>(node);
            write_string(out, element_node.tag_name);
            size_t attr_count = element_node.attributes.size();
            out.write(reinterpret_cast<const char*>(&attr_count), sizeof(attr_count));
            for (const auto& attr : element_node.attributes) {
                write_string(out, attr.name);
                write_string(out, attr.value);
            }
            size_t child_count = element_node.children.size();
            out.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
            for (const auto& child : element_node.children) {
                serialize_node(out, *child);
            }
            break;
        }
        case NodeType::Text: {
            const auto& text_node = static_cast<const TextNode&>(node);
            write_string(out, text_node.value);
            break;
        }
        case NodeType::TemplateDefinition: {
            const auto& tmpl_node = static_cast<const TemplateDefinitionNode&>(node);
            write_string(out, tmpl_node.type);
            write_string(out, tmpl_node.name);
            // Serialize body
            size_t body_count = tmpl_node.body.size();
            out.write(reinterpret_cast<const char*>(&body_count), sizeof(body_count));
            for (const auto& item : tmpl_node.body) {
                serialize_node(out, *item);
            }
            // Serialize properties for @Style templates
            size_t prop_count = tmpl_node.properties.size();
            out.write(reinterpret_cast<const char*>(&prop_count), sizeof(prop_count));
            for (const auto& prop : tmpl_node.properties) {
                serialize_node(out, *prop);
            }
            break;
        }
        case NodeType::CustomDefinition: {
            const auto& custom_node = static_cast<const CustomDefinitionNode&>(node);
            write_string(out, custom_node.type);
            write_string(out, custom_node.name);
            size_t child_count = custom_node.children.size();
            out.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
            for (const auto& child : custom_node.children) {
                serialize_node(out, *child);
            }
            break;
        }
        case NodeType::StyleProperty: {
            const auto& prop_node = static_cast<const StyleProperty&>(node);
            write_string(out, prop_node.name);
            bool has_value = prop_node.value != nullptr;
            out.write(reinterpret_cast<const char*>(&has_value), sizeof(has_value));
            if (has_value) {
                serialize_node(out, *prop_node.value);
            }
            break;
        }
        case NodeType::Identifier: {
            const auto& ident_node = static_cast<const Identifier&>(node);
            write_string(out, ident_node.value);
            break;
        }
        case NodeType::NumberLiteral: {
            const auto& num_node = static_cast<const NumberLiteral&>(node);
            out.write(reinterpret_cast<const char*>(&num_node.value), sizeof(num_node.value));
            write_string(out, num_node.unit);
            break;
        }
        case NodeType::StringLiteral: {
            const auto& str_node = static_cast<const StringLiteral&>(node);
            write_string(out, str_node.value);
            break;
        }
        case NodeType::InfixExpression: {
            const auto& infix_node = static_cast<const InfixExpression&>(node);
            write_string(out, infix_node.op);
            serialize_node(out, *infix_node.left);
            serialize_node(out, *infix_node.right);
            break;
        }
        case NodeType::ConditionalExpression: {
            const auto& cond_node = static_cast<const ConditionalExpression&>(node);
            serialize_node(out, *cond_node.condition);
            serialize_node(out, *cond_node.consequence);
            bool has_alternative = cond_node.alternative != nullptr;
            out.write(reinterpret_cast<const char*>(&has_alternative), sizeof(has_alternative));
            if (has_alternative) {
                serialize_node(out, *cond_node.alternative);
            }
            break;
        }
        case NodeType::ExpressionList: {
            const auto& list_node = static_cast<const ExpressionListNode&>(node);
            size_t count = list_node.expressions.size();
            out.write(reinterpret_cast<const char*>(&count), sizeof(count));
            for (const auto& expr : list_node.expressions) {
                serialize_node(out, *expr);
            }
            break;
        }
        default:
            // For now, we'll just skip unknown nodes. A more robust implementation might error here.
            break;
    }
}

void Packer::Pack(const ProgramNode& program, const std::string& output_path) {
    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot open file for writing: " + output_path);
    }
    serialize_node(out, program);
}

} // namespace CMOD
} // namespace CHTL
