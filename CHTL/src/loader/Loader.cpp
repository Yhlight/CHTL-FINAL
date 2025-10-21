#include "loader/Loader.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "nodes/ProgramNode.h"
#include "nodes/TemplateDefinitionNode.h"
#include "nodes/CustomDefinitionNode.h"
#include "nodes/StylePropertyNode.h"
#include "nodes/ExpressionNode.h"
#include "CJMOD/loader.h"
#include "CHTLJS/include/nodes/ProgramNode.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <memory>
#include <functional>
#include <utility>

namespace CHTL
{

std::string Loader::ReadFile(const std::string& base_file_path, const std::string& import_path)
{
    std::filesystem::path base_dir = base_file_path.empty()
        ? std::filesystem::current_path()
        : std::filesystem::path(base_file_path).parent_path();
    std::filesystem::path target_file_path = (base_dir / import_path).lexically_normal();
    if (!std::filesystem::exists(target_file_path))
    {
        throw std::runtime_error("Imported file not found: " + target_file_path.string());
    }
    std::ifstream file_stream(target_file_path);
    if (!file_stream.is_open())
    {
        throw std::runtime_error("Could not open imported file: " + target_file_path.string());
    }
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

std::pair<std::unique_ptr<ProgramNode>, std::unique_ptr<CHTLJS::ProgramNode>>
Loader::LoadModule(const std::string& base_file_path, const std::string& import_path)
{
    auto cjs_program = std::make_unique<CHTLJS::ProgramNode>();
    std::filesystem::path base_dir = base_file_path.empty()
        ? std::filesystem::current_path()
        : std::filesystem::path(base_file_path).parent_path();
    std::filesystem::path target_file_path = (base_dir / import_path).lexically_normal();
    std::string extension = target_file_path.extension().string();

    if (extension == ".cjmod")
    {
        CJMOD::Loader::Load(target_file_path.string(), *cjs_program);
        return {nullptr, std::move(cjs_program)};
    }
    else if (extension == ".cmod")
    {
        std::ifstream in(target_file_path, std::ios::binary);
        if (!in) {
            throw std::runtime_error("Cannot open module for reading: " + target_file_path.string());
        }
        auto read_string = [](std::ifstream& in_stream) -> std::string {
            size_t len;
            in_stream.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string str(len, '\0');
            in_stream.read(&str[0], len);
            return str;
        };
        std::function<std::unique_ptr<AstNode>(std::ifstream&)> deserialize_node =
            [&](std::ifstream& in_stream) -> std::unique_ptr<AstNode> {
            NodeType type;
            in_stream.read(reinterpret_cast<char*>(&type), sizeof(type));
            if (in_stream.eof()) return nullptr;
            switch (type) {
                case NodeType::Program: {
                    auto program_node = std::make_unique<ProgramNode>();
                    size_t child_count;
                    in_stream.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
                    for (size_t i = 0; i < child_count; ++i) {
                        if(auto child = deserialize_node(in_stream)) program_node->children.push_back(std::move(child));
                    }
                    return program_node;
                }
                case NodeType::Element: {
                    auto element_node = std::make_unique<ElementNode>();
                    element_node->tag_name = read_string(in_stream);
                    size_t attr_count;
                    in_stream.read(reinterpret_cast<char*>(&attr_count), sizeof(attr_count));
                    for (size_t i = 0; i < attr_count; ++i) {
                        element_node->attributes.push_back({read_string(in_stream), read_string(in_stream)});
                    }
                    size_t child_count;
                    in_stream.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
                    for (size_t i = 0; i < child_count; ++i) {
                         if(auto child = deserialize_node(in_stream)) element_node->children.push_back(std::move(child));
                    }
                    return element_node;
                }
                case NodeType::Text: {
                    auto text_node = std::make_unique<TextNode>();
                    text_node->value = read_string(in_stream);
                    return text_node;
                }
                case NodeType::TemplateDefinition: {
                    auto tmpl_node = std::make_unique<TemplateDefinitionNode>();
                    tmpl_node->type = read_string(in_stream);
                    tmpl_node->name = read_string(in_stream);
                    size_t body_count;
                    in_stream.read(reinterpret_cast<char*>(&body_count), sizeof(body_count));
                    for (size_t i = 0; i < body_count; ++i) {
                        if(auto item = deserialize_node(in_stream)) tmpl_node->body.push_back(std::move(item));
                    }
                    size_t prop_count;
                    in_stream.read(reinterpret_cast<char*>(&prop_count), sizeof(prop_count));
                    for (size_t i = 0; i < prop_count; ++i) {
                         if(auto prop = deserialize_node(in_stream)) {
                             tmpl_node->properties.push_back(std::unique_ptr<StyleProperty>(static_cast<StyleProperty*>(prop.release())));
                         }
                    }
                    return tmpl_node;
                }
                case NodeType::CustomDefinition: {
                    auto custom_node = std::make_unique<CustomDefinitionNode>();
                    custom_node->type = read_string(in_stream);
                    custom_node->name = read_string(in_stream);
                    size_t child_count;
                    in_stream.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
                    for (size_t i = 0; i < child_count; ++i) {
                        if(auto child = deserialize_node(in_stream)) custom_node->children.push_back(std::move(child));
                    }
                    return custom_node;
                }
                case NodeType::StyleProperty: {
                    auto prop_node = std::make_unique<StyleProperty>();
                    prop_node->name = read_string(in_stream);
                    bool has_value;
                    in_stream.read(reinterpret_cast<char*>(&has_value), sizeof(has_value));
                    if (has_value) {
                        auto expr = deserialize_node(in_stream);
                        prop_node->value = std::unique_ptr<Expression>(static_cast<Expression*>(expr.release()));
                    }
                    return prop_node;
                }
                case NodeType::Identifier: {
                    auto ident_node = std::make_unique<Identifier>();
                    ident_node->value = read_string(in_stream);
                    return ident_node;
                }
                default:
                    return nullptr;
            }
        };
        auto node = deserialize_node(in);
        if (node && node->GetType() == NodeType::Program) {
             return {std::unique_ptr<ProgramNode>(static_cast<ProgramNode*>(node.release())), std::move(cjs_program)};
        }
        throw std::runtime_error("Failed to deserialize module: " + target_file_path.string());
    }
    else
    {
        std::string file_content = ReadFile(base_file_path, import_path);
        Lexer lexer(file_content);
        Parser parser(lexer, target_file_path.string());
        return {parser.ParseProgram(), std::move(cjs_program)};
    }
}

}