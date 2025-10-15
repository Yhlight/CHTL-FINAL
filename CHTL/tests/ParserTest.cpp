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
