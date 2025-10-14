#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"

#include <string>
#include <memory>

// Helper to check for parsing errors
void checkParserErrors(const CHTL::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }

    std::string error_messages;
    for (const auto& msg : errors) {
        error_messages += msg + "\n";
    }
    FAIL("Parser has errors:\n" << error_messages);
}


TEST_CASE("Parser correctly parses simple element and text nodes", "[parser]")
{
    SECTION("Parses a simple div with a text node")
    {
        std::string input = R"(
            div {
                text { "Hello" }
            }
        )";

        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 1);

        // Check the element node
        auto* element_node_ptr = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(element_node_ptr != nullptr);
        REQUIRE(element_node_ptr->tag_name == "div");
        REQUIRE(element_node_ptr->children.size() == 1);

        // Check the text node inside the element
        auto* text_node_ptr = dynamic_cast<CHTL::TextNode*>(element_node_ptr->children[0].get());
        REQUIRE(text_node_ptr != nullptr);
        REQUIRE(text_node_ptr->value == "Hello");
    }

    SECTION("Parses multiple top-level elements")
    {
        std::string input = R"(
            div {}
            span {}
        )";

        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 2);

        auto* first_element = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(first_element != nullptr);
        REQUIRE(first_element->tag_name == "div");

        auto* second_element = dynamic_cast<CHTL::ElementNode*>(program->children[1].get());
        REQUIRE(second_element != nullptr);
        REQUIRE(second_element->tag_name == "span");
    }
}

TEST_CASE("Parser correctly parses attributes", "[parser]")
{
    SECTION("Parses various attribute types")
    {
        std::string input = R"(
            div {
                id: box;
                class = "container";
                width: 100;
                height = 25.5;
            }
        )";

        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 1);

        auto* el = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(el != nullptr);
        REQUIRE(el->tag_name == "div");
        REQUIRE(el->attributes.size() == 4);

        // Check attributes
        REQUIRE(el->attributes[0].name == "id");
        REQUIRE(el->attributes[0].value == "box");

        REQUIRE(el->attributes[1].name == "class");
        REQUIRE(el->attributes[1].value == "container");

        REQUIRE(el->attributes[2].name == "width");
        REQUIRE(el->attributes[2].value == "100");

        REQUIRE(el->attributes[3].name == "height");
        REQUIRE(el->attributes[3].value == "25.5");
    }
}

TEST_CASE("Parser correctly parses style blocks", "[parser]")
{
    SECTION("Parses a style block with multiple properties")
    {
        std::string input = R"(
            div {
                style {
                    width: 100px;
                    color: red;
                    background-color = "nice blue";
                }
            }
        )";

        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 1);

        auto* el = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(el != nullptr);
        REQUIRE(el->children.size() == 1);

        auto* style_node = dynamic_cast<CHTL::StyleNode*>(el->children[0].get());
        REQUIRE(style_node != nullptr);
        REQUIRE(style_node->properties.size() == 3);

        // Check style properties
        REQUIRE(style_node->properties[0].name == "width");
        REQUIRE(style_node->properties[0].value == "100px");

        REQUIRE(style_node->properties[1].name == "color");
        REQUIRE(style_node->properties[1].value == "red");

        REQUIRE(style_node->properties[2].name == "background-color");
        REQUIRE(style_node->properties[2].value == "nice blue");
    }
}
