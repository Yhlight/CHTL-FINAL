#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/StylePropertyNode.h"
#include "CHTL/CHTLNode/NumericLiteralExprNode.h"
#include "CHTL/CHTLNode/IdentifierExprNode.h"
#include "CHTL/CHTLNode/BinaryOpExprNode.h"

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
        REQUIRE(text->content == "hello world");
    }
}

TEST_CASE("Parser correctly handles expressions in styles", "[parser]") {
    SECTION("Parse a style property with an arithmetic expression") {
        Lexer lexer("div { style { width: 100 + 20; } }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->style != nullptr);
        REQUIRE(div->style->properties.size() == 1);

        auto* prop = div->style->properties[0].get();
        REQUIRE(prop->key == "width");

        auto* binaryOp = dynamic_cast<BinaryOpExprNode*>(prop->value.get());
        REQUIRE(binaryOp != nullptr);
        REQUIRE(binaryOp->op == TokenType::PLUS);

        auto* left = dynamic_cast<NumericLiteralExprNode*>(binaryOp->left.get());
        REQUIRE(left != nullptr);
        REQUIRE(left->value == 100);

        auto* right = dynamic_cast<NumericLiteralExprNode*>(binaryOp->right.get());
        REQUIRE(right != nullptr);
        REQUIRE(right->value == 20);
    }
}

TEST_CASE("Parser correctly handles style blocks", "[parser]") {
    SECTION("Parse an empty style block") {
        Lexer lexer("div { style {} }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->style != nullptr);
        REQUIRE(div->style->properties.empty());
    }

    SECTION("Parse a style block with properties") {
        Lexer lexer("div { style { color: red; font-size: 16; } }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->style != nullptr);
        REQUIRE(div->style->properties.size() == 2);

        auto* prop1 = div->style->properties[0].get();
        REQUIRE(prop1->key == "color");
        auto* value1 = dynamic_cast<IdentifierExprNode*>(prop1->value.get());
        REQUIRE(value1 != nullptr);
        REQUIRE(value1->name == "red");

        auto* prop2 = div->style->properties[1].get();
        REQUIRE(prop2->key == "font-size");
        auto* value2 = dynamic_cast<NumericLiteralExprNode*>(prop2->value.get());
        REQUIRE(value2 != nullptr);
        REQUIRE(value2->value == 16);
    }

    SECTION("Parse a style block with a CSS rule") {
        Lexer lexer("div { style { .box { color: blue; } } }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->style != nullptr);
        REQUIRE(div->style->properties.empty());
        REQUIRE(div->style->rules.size() == 1);

        auto* rule = div->style->rules[0].get();
        REQUIRE(rule->selector == ".box");
        REQUIRE(rule->properties.size() == 1);

        auto* prop = rule->properties[0].get();
        REQUIRE(prop->key == "color");
        auto* value = dynamic_cast<IdentifierExprNode*>(prop->value.get());
        REQUIRE(value != nullptr);
        REQUIRE(value->name == "blue");
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
            # generator comment
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

TEST_CASE("Parser correctly handles attributes", "[parser]") {
    SECTION("Parse an element with a simple attribute (colon)") {
        Lexer lexer("div { id: my-id; }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->attributes.size() == 1);
        REQUIRE(element->attributes["id"] == "my-id");
        REQUIRE(element->children.empty());
    }

    SECTION("Parse an element with a simple attribute (equals)") {
        Lexer lexer("div { class = \"my-class\"; }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->attributes.size() == 1);
        REQUIRE(element->attributes["class"] == "my-class");
        REQUIRE(element->children.empty());
    }

    SECTION("Parse an element with multiple attributes") {
        Lexer lexer("a { href: \"/index.html\"; target = _blank; }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "a");
        REQUIRE(element->attributes.size() == 2);
        REQUIRE(element->attributes["href"] == "/index.html");
        REQUIRE(element->attributes["target"] == "_blank");
        REQUIRE(element->children.empty());
    }

    SECTION("Parse a mix of attributes and child elements") {
        Lexer lexer("div { id: main; h1 {} }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->tagName == "div");
        REQUIRE(div->attributes.size() == 1);
        REQUIRE(div->attributes["id"] == "main");
        REQUIRE(div->children.size() == 1);

        auto* h1 = dynamic_cast<ElementNode*>(div->children[0].get());
        REQUIRE(h1 != nullptr);
        REQUIRE(h1->tagName == "h1");
    }

    SECTION("Parse the 'text' attribute as a child node") {
        Lexer lexer("div { text: \"hello world\"; }");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->tagName == "div");

        REQUIRE(div->attributes.find("text") == div->attributes.end());
        REQUIRE(div->attributes.empty());

        REQUIRE(div->children.size() == 1);
        auto* text = dynamic_cast<TextNode*>(div->children[0].get());
        REQUIRE(text != nullptr);
        REQUIRE(text->content == "hello world");
    }
}