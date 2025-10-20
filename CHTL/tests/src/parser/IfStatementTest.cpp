#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "catch.hpp"
#include <memory>

using namespace CHTL;

TEST_CASE("Parse Simple If", "[parser]") {
    std::string input = R"(
        if {
            condition: a > b;
            div {}
        }
    )";
    Lexer lexer(input, GetDefaultKeywords());
    Parser parser(lexer, "");
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto if_node = dynamic_cast<IfNode*>(program->children[0].get());
    REQUIRE(if_node != nullptr);
    REQUIRE(if_node->condition != nullptr);
    REQUIRE(if_node->consequence.size() == 1);
    REQUIRE(if_node->alternative == nullptr);
}

TEST_CASE("Parse If-Else", "[parser]") {
    std::string input = R"(
        if {
            condition: a > b;
            div {}
        } else {
            span {}
        }
    )";
    Lexer lexer(input, GetDefaultKeywords());
    Parser parser(lexer, "");
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto if_node = dynamic_cast<IfNode*>(program->children[0].get());
    REQUIRE(if_node != nullptr);
    REQUIRE(if_node->condition != nullptr);
    REQUIRE(if_node->consequence.size() == 1);

    auto else_node = dynamic_cast<ElseNode*>(if_node->alternative.get());
    REQUIRE(else_node != nullptr);
    REQUIRE(else_node->consequence.size() == 1);
}

TEST_CASE("Parse If-Else-If", "[parser]") {
    std::string input = R"(
        if {
            condition: a > b;
            div {}
        } else if {
            condition: a < b;
            span {}
        }
    )";
    Lexer lexer(input, GetDefaultKeywords());
    Parser parser(lexer, "");
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto if_node = dynamic_cast<IfNode*>(program->children[0].get());
    REQUIRE(if_node != nullptr);
    REQUIRE(if_node->condition != nullptr);
    REQUIRE(if_node->consequence.size() == 1);

    auto else_if_node = dynamic_cast<IfNode*>(if_node->alternative.get());
    REQUIRE(else_if_node != nullptr);
    REQUIRE(else_if_node->condition != nullptr);
    REQUIRE(else_if_node->consequence.size() == 1);
    REQUIRE(else_if_node->alternative == nullptr);
}

TEST_CASE("Parse If-Else-If-Else", "[parser]") {
    std::string input = R"(
        if {
            condition: a > b;
            div {}
        } else if {
            condition: a < b;
            span {}
        } else {
            p {}
        }
    )";
    Lexer lexer(input, GetDefaultKeywords());
    Parser parser(lexer, "");
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto if_node = dynamic_cast<IfNode*>(program->children[0].get());
    REQUIRE(if_node != nullptr);
    REQUIRE(if_node->condition != nullptr);
    REQUIRE(if_node->consequence.size() == 1);

    auto else_if_node = dynamic_cast<IfNode*>(if_node->alternative.get());
    REQUIRE(else_if_node != nullptr);
    REQUIRE(else_if_node->condition != nullptr);
    REQUIRE(else_if_node->consequence.size() == 1);

    auto else_node = dynamic_cast<ElseNode*>(else_if_node->alternative.get());
    REQUIRE(else_node != nullptr);
    REQUIRE(else_node->consequence.size() == 1);
}
