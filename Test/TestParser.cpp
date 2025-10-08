#include "catch.hpp"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/StyleNode.h"
#include <string>

TEST_CASE("Parser::Simple Element", "[parser]") {
    const std::string input = "div {}";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(parser.Errors().empty());
    REQUIRE(program->GetChildren().size() == 1);

    auto element = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(element != nullptr);
    REQUIRE(element->GetTokenLiteral() == "div");
}

TEST_CASE("Parser::Nested Elements", "[parser]") {
    const std::string input = R"(
        body {
            div {
                span {}
            }
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(parser.Errors().empty());

    // body
    REQUIRE(program->GetChildren().size() == 1);
    auto body = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(body != nullptr);

    // div
    REQUIRE(body->GetChildren().size() == 1);
    auto div = dynamic_cast<CHTL::ElementNode*>(body->GetChildren()[0].get());
    REQUIRE(div != nullptr);

    // span
    REQUIRE(div->GetChildren().size() == 1);
    auto span = dynamic_cast<CHTL::ElementNode*>(div->GetChildren()[0].get());
    REQUIRE(span != nullptr);
}

TEST_CASE("Parser::Element with Attributes", "[parser]") {
    const std::string input = R"(
        div {
            id: my_div;
            class: "container";
            width: 100;
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    if (!parser.Errors().empty()) INFO(parser.Errors()[0]);
    REQUIRE(parser.Errors().empty());

    REQUIRE(program->GetChildren().size() == 1);
    auto div = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(div != nullptr);
}

TEST_CASE("Parser::Element with Style Block", "[parser]") {
    const std::string input = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    if (!parser.Errors().empty()) INFO(parser.Errors()[0]);
    REQUIRE(parser.Errors().empty());

    REQUIRE(program->GetChildren().size() == 1);
    auto div = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(div != nullptr);

    REQUIRE(div->GetChildren().size() == 1);
    auto styleNode = dynamic_cast<CHTL::StyleNode*>(div->GetChildren()[0].get());
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->GetChildren().size() == 2);
}

TEST_CASE("Parser::Unquoted Text Node", "[parser]") {
    const std::string input = R"(
        p {
            text { This is some unquoted text }
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    if (!parser.Errors().empty()) INFO(parser.Errors()[0]);
    REQUIRE(parser.Errors().empty());

    REQUIRE(program->GetChildren().size() == 1);
    auto p = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(p != nullptr);

    REQUIRE(p->GetChildren().size() == 1);
    auto textNode = dynamic_cast<CHTL::TextNode*>(p->GetChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->GetText() == "This is some unquoted text");
}

TEST_CASE("Parser::Quoted Text Node", "[parser]") {
    const std::string input = R"(
        p {
            text { "This is some quoted text." }
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    if (!parser.Errors().empty()) INFO(parser.Errors()[0]);
    REQUIRE(parser.Errors().empty());

    REQUIRE(program->GetChildren().size() == 1);
    auto p = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(p != nullptr);

    REQUIRE(p->GetChildren().size() == 1);
    auto textNode = dynamic_cast<CHTL::TextNode*>(p->GetChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->GetText() == "This is some quoted text.");
}

TEST_CASE("Parser::Arithmetic Expression", "[parser]") {
    const std::string input = R"(
        div {
            width: 100 + 20px;
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    if (!parser.Errors().empty()) INFO(parser.Errors()[0]);
    REQUIRE(parser.Errors().empty());
}

TEST_CASE("Parser::Conditional Expression", "[parser]") {
    const std::string input = R"(
        div {
            background-color: width > 50 ? "red" : "blue";
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    if (!parser.Errors().empty()) INFO(parser.Errors()[0]);
    REQUIRE(parser.Errors().empty());
}