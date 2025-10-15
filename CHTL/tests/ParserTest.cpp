#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Parser.h"
#include "AstNode.h"
#include "Lexer.h"
#include <vector>
#include <string>

// Helper function to check for parser errors
void checkParserErrors(const CHTL::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }

    std::string error_message = "Parser has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("Test parsing a simple empty element", "[parser]")
{
    std::string input = "div {}";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* element_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(element_node != nullptr);
    REQUIRE(element_node->tag_name == "div");
    REQUIRE(element_node->attributes.empty());
    REQUIRE(element_node->children.empty());
}

TEST_CASE("Test parsing a simple text node with a quoted string", "[parser]")
{
    std::string input = R"(text { "hello world" })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* text_node = dynamic_cast<CHTL::TextNode*>(program->children[0].get());
    REQUIRE(text_node != nullptr);
    REQUIRE(text_node->value == "hello world");
}

TEST_CASE("Test parsing a simple text node with an unquoted literal", "[parser]")
{
    std::string input = R"(text { hello world })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* text_node = dynamic_cast<CHTL::TextNode*>(program->children[0].get());
    REQUIRE(text_node != nullptr);
    REQUIRE(text_node->value == "hello world");
}

TEST_CASE("Test parsing element with attributes", "[parser]")
{
    std::string input = R"(div { id: "box"; class: container; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* element_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(element_node != nullptr);
    REQUIRE(element_node->tag_name == "div");
    REQUIRE(element_node->children.empty());

    REQUIRE(element_node->attributes.size() == 2);
    REQUIRE(element_node->attributes[0].name == "id");
    REQUIRE(element_node->attributes[0].value == "box");
    REQUIRE(element_node->attributes[1].name == "class");
    REQUIRE(element_node->attributes[1].value == "container");
}

TEST_CASE("Test parsing nested elements", "[parser]")
{
    std::string input = R"(body { div { span {} } })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* body_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(body_node != nullptr);
    REQUIRE(body_node->tag_name == "body");
    REQUIRE(body_node->attributes.empty());
    REQUIRE(body_node->children.size() == 1);

    auto* div_node = dynamic_cast<CHTL::ElementNode*>(body_node->children[0].get());
    REQUIRE(div_node != nullptr);
    REQUIRE(div_node->tag_name == "div");
    REQUIRE(div_node->attributes.empty());
    REQUIRE(div_node->children.size() == 1);

    auto* span_node = dynamic_cast<CHTL::ElementNode*>(div_node->children[0].get());
    REQUIRE(span_node != nullptr);
    REQUIRE(span_node->tag_name == "span");
    REQUIRE(span_node->attributes.empty());
    REQUIRE(span_node->children.empty());
}

TEST_CASE("Test parsing a comprehensive structure", "[parser]")
{
    std::string input = R"(
        body {
            div {
                id: "main";
                class: container;

                p {
                    text { "This is a paragraph." }
                }

                span {
                    text { and this is unquoted text }
                }
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* body_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(body_node != nullptr);
    REQUIRE(body_node->tag_name == "body");
    REQUIRE(body_node->children.size() == 1);

    auto* div_node = dynamic_cast<CHTL::ElementNode*>(body_node->children[0].get());
    REQUIRE(div_node != nullptr);
    REQUIRE(div_node->tag_name == "div");
    REQUIRE(div_node->attributes.size() == 2);
    REQUIRE(div_node->attributes[1].value == "container");
    REQUIRE(div_node->children.size() == 2);

    auto* p_node = dynamic_cast<CHTL::ElementNode*>(div_node->children[0].get());
    REQUIRE(p_node != nullptr);
    REQUIRE(p_node->children.size() == 1);
    auto* p_text_node = dynamic_cast<CHTL::TextNode*>(p_node->children[0].get());
    REQUIRE(p_text_node != nullptr);
    REQUIRE(p_text_node->value == "This is a paragraph.");

    auto* span_node = dynamic_cast<CHTL::ElementNode*>(div_node->children[1].get());
    REQUIRE(span_node != nullptr);
    REQUIRE(span_node->children.size() == 1);
    auto* span_text_node = dynamic_cast<CHTL::TextNode*>(span_node->children[0].get());
    REQUIRE(span_text_node != nullptr);
    REQUIRE(span_text_node->value == "and this is unquoted text");
}

TEST_CASE("Test parsing style block with simple properties", "[parser]")
{
    std::string input = R"(
        style {
            width: 100px;
            height: 200;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->properties.size() == 2);

    REQUIRE(style_node->properties[0].name == "width");
    auto* width_value = dynamic_cast<CHTL::NumberLiteral*>(style_node->properties[0].value.get());
    REQUIRE(width_value != nullptr);
    REQUIRE(width_value->value == 100.0);
    REQUIRE(width_value->unit == "px");

    REQUIRE(style_node->properties[1].name == "height");
    auto* height_value = dynamic_cast<CHTL::NumberLiteral*>(style_node->properties[1].value.get());
    REQUIRE(height_value != nullptr);
    REQUIRE(height_value->value == 200.0);
    REQUIRE(height_value->unit.empty());
}
