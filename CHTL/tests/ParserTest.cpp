#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"

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

TEST_CASE("Parser correctly parses a simple element", "[parser]")
{
    std::string input = "div {}";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);

    auto program = p.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(p.GetErrors().empty());
    REQUIRE(program->statements.size() == 1);

    auto elementNode = dynamic_cast<CHTL::ElementNode*>(program->statements[0].get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->tagName == "div");
    REQUIRE(elementNode->children.empty());
}

TEST_CASE("Parser correctly parses text nodes", "[parser]")
{
    SECTION("Parses a text node with unquoted literals")
    {
        std::string input = "text { hello world }";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->statements.size() == 1);

        auto* text_node_ptr = dynamic_cast<CHTL::TextNode*>(program->statements[0].get());
        REQUIRE(text_node_ptr != nullptr);
        REQUIRE(text_node_ptr->value == "hello world");
    }
}
