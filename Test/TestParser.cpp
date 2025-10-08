#include "catch.hpp"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLNode/ElementNode.h"
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
    REQUIRE(body->GetTokenLiteral() == "body");

    // div
    REQUIRE(body->GetChildren().size() == 1);
    auto div = dynamic_cast<CHTL::ElementNode*>(body->GetChildren()[0].get());
    REQUIRE(div != nullptr);
    REQUIRE(div->GetTokenLiteral() == "div");

    // span
    REQUIRE(div->GetChildren().size() == 1);
    auto span = dynamic_cast<CHTL::ElementNode*>(div->GetChildren()[0].get());
    REQUIRE(span != nullptr);
    REQUIRE(span->GetTokenLiteral() == "span");
}

TEST_CASE("Parser::Text Node", "[parser]") {
    const std::string input = R"(
        text { "Hello World" }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(parser.Errors().empty());

    REQUIRE(program->GetChildren().size() == 1);
    auto textNode = dynamic_cast<CHTL::TextNode*>(program->GetChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->GetText() == "Hello World");
}

TEST_CASE("Parser::Element with Text Node", "[parser]") {
    const std::string input = R"(
        p {
            text { "Some sample text." }
        }
    )";
    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    auto program = parser.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(parser.Errors().empty());

    REQUIRE(program->GetChildren().size() == 1);
    auto p = dynamic_cast<CHTL::ElementNode*>(program->GetChildren()[0].get());
    REQUIRE(p != nullptr);
    REQUIRE(p->GetTokenLiteral() == "p");

    REQUIRE(p->GetChildren().size() == 1);
    auto textNode = dynamic_cast<CHTL::TextNode*>(p->GetChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->GetText() == "Some sample text.");
}