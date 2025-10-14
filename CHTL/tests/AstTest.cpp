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
