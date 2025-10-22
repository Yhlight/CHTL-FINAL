#include "gtest/gtest.h"
#include "AstNode.h"
#include "Serialize.h"
#include "nodes/ProgramNode.h"
#include "nodes/ElementNode.h"
#include "nodes/TextNode.h"
#include "nodes/StyleNode.h"
#include "nodes/StyleRuleNode.h"
#include "nodes/StylePropertyNode.h"
#include "nodes/ExpressionNode.h"
#include "nodes/ScriptNode.h"
#include "nodes/ProgramNode.h"
#include "nodes/RawJSNode.h"
#include <fstream>
#include <memory>

using namespace CHTL;

TEST(SerializationTest, ProgramNode)
{
    auto program = std::make_unique<ProgramNode>();
    program->children.push_back(std::make_unique<TextNode>("hello"));
    program->children.push_back(std::make_unique<TextNode>("world"));

    std::ofstream ofs("test.bin", std::ios::binary);
    program->serialize(ofs);
    ofs.close();

    std::ifstream ifs("test.bin", std::ios::binary);
    auto deserialized_program = AstNode::deserialize(ifs);
    ifs.close();

    ASSERT_NE(deserialized_program, nullptr);
    ASSERT_EQ(deserialized_program->GetType(), NodeType::Program);
    auto deserialized_program_node = static_cast<ProgramNode*>(deserialized_program.get());
    ASSERT_EQ(deserialized_program_node->children.size(), 2);
    ASSERT_EQ(deserialized_program_node->children[0]->GetType(), NodeType::Text);
    ASSERT_EQ(static_cast<TextNode*>(deserialized_program_node->children[0].get())->value, "hello");
    ASSERT_EQ(deserialized_program_node->children[1]->GetType(), NodeType::Text);
    ASSERT_EQ(static_cast<TextNode*>(deserialized_program_node->children[1].get())->value, "world");
}

TEST(SerializationTest, ComplexNode)
{
    auto program = std::make_unique<ProgramNode>();

    auto element = std::make_unique<ElementNode>();
    element->tag_name = "div";
    element->attributes.push_back({"id", "main"});
    element->children.push_back(std::make_unique<TextNode>("content"));
    program->children.push_back(std::move(element));

    auto style = std::make_unique<StyleNode>();
    auto rule = std::make_unique<StyleRuleNode>();
    rule->selector = "#main";
    auto prop = std::make_unique<StyleProperty>();
    prop->name = "width";
    prop->value = std::make_unique<NumberLiteral>(100.0, "px");
    rule->properties.push_back(std::move(prop));
    style->children.push_back(std::move(rule));
    program->children.push_back(std::move(style));

    auto script = std::make_unique<ScriptNode>();
    script->js_ast = std::make_unique<CHTLJS::ProgramNode>();
    auto raw_js = std::make_unique<CHTLJS::RawJSNode>();
    raw_js->content = "console.log('hello');";
    script->js_ast->children.push_back(std::move(raw_js));
    program->children.push_back(std::move(script));

    std::ofstream ofs("complex_test.bin", std::ios::binary);
    program->serialize(ofs);
    ofs.close();

    std::ifstream ifs("complex_test.bin", std::ios::binary);
    auto deserialized_program = AstNode::deserialize(ifs);
    ifs.close();

    ASSERT_NE(deserialized_program, nullptr);
    ASSERT_EQ(deserialized_program->GetType(), NodeType::Program);
    auto deserialized_program_node = static_cast<ProgramNode*>(deserialized_program.get());
    ASSERT_EQ(deserialized_program_node->children.size(), 3);

    auto deserialized_element = static_cast<ElementNode*>(deserialized_program_node->children[0].get());
    ASSERT_EQ(deserialized_element->tag_name, "div");
    ASSERT_EQ(deserialized_element->attributes.size(), 1);
    ASSERT_EQ(deserialized_element->attributes[0].name, "id");
    ASSERT_EQ(deserialized_element->attributes[0].value, "main");
    ASSERT_EQ(deserialized_element->children.size(), 1);
    ASSERT_EQ(static_cast<TextNode*>(deserialized_element->children[0].get())->value, "content");

    auto deserialized_style = static_cast<StyleNode*>(deserialized_program_node->children[1].get());
    ASSERT_EQ(deserialized_style->children.size(), 1);
    auto deserialized_rule = static_cast<StyleRuleNode*>(deserialized_style->children[0].get());
    ASSERT_EQ(deserialized_rule->selector, "#main");
    ASSERT_EQ(deserialized_rule->properties.size(), 1);
    auto deserialized_prop = static_cast<StyleProperty*>(deserialized_rule->properties[0].get());
    ASSERT_EQ(deserialized_prop->name, "width");
    auto deserialized_value = static_cast<NumberLiteral*>(deserialized_prop->value.get());
    ASSERT_EQ(deserialized_value->value, 100.0);
    ASSERT_EQ(deserialized_value->unit, "px");

    auto deserialized_script = static_cast<ScriptNode*>(deserialized_program_node->children[2].get());
    ASSERT_NE(deserialized_script->js_ast, nullptr);
    ASSERT_EQ(deserialized_script->js_ast->children.size(), 1);
    auto deserialized_raw_js = static_cast<CHTLJS::RawJSNode*>(deserialized_script->js_ast->children[0].get());
    ASSERT_EQ(deserialized_raw_js->content, "console.log('hello');");
}
