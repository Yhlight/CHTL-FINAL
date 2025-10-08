#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"

TEST_CASE("Parser correctly parses simple structures", "[parser]") {
    SECTION("Parse a simple element") {
        Lexer lexer("div {}");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->children.size() == 1);

        auto* element = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->children.empty());
    }

    SECTION("Parse a text node") {
        Lexer lexer("text { \"hello world\" }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->children.size() == 1);

        auto* text = dynamic_cast<TextNode*>(ast->children[0].get());
        REQUIRE(text != nullptr);
        // The value in the token includes quotes, so the parser should strip them.
        REQUIRE(text->content == "hello world");
    }
}

TEST_CASE("Parser handles nested structures", "[parser]") {
    SECTION("Parse nested elements") {
        Lexer lexer("html { body { div {} } }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->children.size() == 1);

        auto* html = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(html != nullptr);
        REQUIRE(html->tagName == "html");
        REQUIRE(html->children.size() == 1);

        auto* body = dynamic_cast<ElementNode*>(html->children[0].get());
        REQUIRE(body != nullptr);
        REQUIRE(body->tagName == "body");
        REQUIRE(body->children.size() == 1);

        auto* div = dynamic_cast<ElementNode*>(body->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->tagName == "div");
        REQUIRE(div->children.empty());
    }
}

TEST_CASE("Parser correctly handles comments", "[parser]") {
    SECTION("Parser ignores comments") {
        Lexer lexer(R"(
            // This is a comment
            div {
                /* Another comment */
            }
            # Generator comment
        )");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->children.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
    }
}