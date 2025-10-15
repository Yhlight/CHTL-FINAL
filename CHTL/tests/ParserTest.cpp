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

TEST_CASE("Parser correctly parses infix expressions with operator precedence", "[parser]")
{
    SECTION("Parses a style property with mixed operators")
    {
        std::string input = "style { width: 100 + 50 * 2; }";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->statements.size() == 1);

        auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->statements[0].get());
        REQUIRE(style_node != nullptr);
        REQUIRE(style_node->properties.size() == 1);

        // Verify the expression tree: (100 + (50 * 2))
        auto* expr = dynamic_cast<CHTL::InfixExpression*>(style_node->properties[0].value.get());
        REQUIRE(expr != nullptr);
        REQUIRE(expr->op == "+"); // Top level is addition

        // Left side of addition
        auto* left_num = dynamic_cast<CHTL::NumberLiteral*>(expr->left.get());
        REQUIRE(left_num != nullptr);
        REQUIRE(left_num->value == 100.0);

        // Right side of addition should be another infix expression
        auto* right_infix = dynamic_cast<CHTL::InfixExpression*>(expr->right.get());
        REQUIRE(right_infix != nullptr);
        REQUIRE(right_infix->op == "*"); // Nested is multiplication

        auto* right_left_num = dynamic_cast<CHTL::NumberLiteral*>(right_infix->left.get());
        REQUIRE(right_left_num != nullptr);
        REQUIRE(right_left_num->value == 50.0);

        auto* right_right_num = dynamic_cast<CHTL::NumberLiteral*>(right_infix->right.get());
        REQUIRE(right_right_num != nullptr);
        REQUIRE(right_right_num->value == 2.0);
    }
}
