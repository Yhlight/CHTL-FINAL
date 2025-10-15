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

TEST_CASE("Test parsing style block with identifier property", "[parser]")
{
    std::string input = R"(style { color: red; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->properties.size() == 1);

    REQUIRE(style_node->properties[0].name == "color");
    auto* value_node = dynamic_cast<CHTL::Identifier*>(style_node->properties[0].value.get());
    REQUIRE(value_node != nullptr);
    REQUIRE(value_node->value == "red");
}

TEST_CASE("Test parsing style block with arithmetic expressions", "[parser]")
{
    std::string input = R"(style { width: 100 + 50 * 2; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->properties.size() == 1);

    REQUIRE(style_node->properties[0].name == "width");

    // Check for expression: (100 + (50 * 2))
    auto* expr = dynamic_cast<CHTL::InfixExpression*>(style_node->properties[0].value.get());
    REQUIRE(expr != nullptr);
    REQUIRE(expr->op == "+");

    auto* left_num = dynamic_cast<CHTL::NumberLiteral*>(expr->left.get());
    REQUIRE(left_num != nullptr);
    REQUIRE(left_num->value == 100.0);

    auto* right_infix_expr = dynamic_cast<CHTL::InfixExpression*>(expr->right.get());
    REQUIRE(right_infix_expr != nullptr);
    REQUIRE(right_infix_expr->op == "*");

    auto* right_left_num = dynamic_cast<CHTL::NumberLiteral*>(right_infix_expr->left.get());
    REQUIRE(right_left_num != nullptr);
    REQUIRE(right_left_num->value == 50.0);

    auto* right_right_num = dynamic_cast<CHTL::NumberLiteral*>(right_infix_expr->right.get());
    REQUIRE(right_right_num != nullptr);
    REQUIRE(right_right_num->value == 2.0);
}

TEST_CASE("Test parsing a generator comment", "[parser]")
{
    std::string input = R"(# this is a comment)";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* comment_node = dynamic_cast<CHTL::CommentNode*>(program->children[0].get());
    REQUIRE(comment_node != nullptr);
    REQUIRE(comment_node->value == "this is a comment");
}
