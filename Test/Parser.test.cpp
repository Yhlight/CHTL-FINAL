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
#include "CHTL/CHTLNode/TemplateDeclarationNode.h"
#include "CHTL/CHTLNode/TemplateUsageNode.h"
#include "CHTL/CHTLNode/CustomDeclarationNode.h"
#include "CHTL/CHTLNode/DeleteNode.h"
#include "CHTL/CHTLNode/InsertNode.h"
#include "CHTL/CHTLContext/CHTLContext.h"

TEST_CASE("Parser correctly parses simple structures", "[parser]") {
    SECTION("Parse a simple element") {
        CHTLContext context;
        Lexer lexer("div {}");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->children.size() == 1);

        auto* element = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->children.empty());
    }

    SECTION("Parse a text node") {
        CHTLContext context;
        Lexer lexer("text { \"hello world\" }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { style { width: 100 + 20; } }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { style {} }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->style != nullptr);
        REQUIRE(div->style->properties.empty());
    }

    SECTION("Parse a style block with properties") {
        CHTLContext context;
        Lexer lexer("div { style { color: red; font-size: 16; } }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { style { .box { color: blue; } } }");
        Parser parser(lexer.tokenize(), context);
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

TEST_CASE("Parser correctly handles templates", "[parser]") {
    SECTION("Parse a top-level style template declaration") {
        CHTLContext context;
        Lexer lexer("[Template] @Style DefaultText { color: black; }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.empty());

        REQUIRE(context.styleTemplates.count("DefaultText") == 1);
        auto* tpl = context.styleTemplates["DefaultText"].get();
        REQUIRE(tpl != nullptr);
        REQUIRE(tpl->templateType == TemplateType::STYLE);
        REQUIRE(tpl->name == "DefaultText");
        REQUIRE(tpl->body.size() == 1);
    }

    SECTION("Parse a style template usage") {
        CHTLContext context;
        Lexer lexer("div { style { @Style DefaultText; } }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->style != nullptr);
        REQUIRE(div->style->properties.empty());
        REQUIRE(div->style->rules.empty());
        REQUIRE(div->style->templateUsages.size() == 1);

        auto* usage = div->style->templateUsages[0].get();
        REQUIRE(usage->templateType == TemplateType::STYLE);
        REQUIRE(usage->name == "DefaultText");
    }

    SECTION("Parse an element template usage") {
        CHTLContext context;
        Lexer lexer("body { @Element Box; }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* body = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(body != nullptr);
        REQUIRE(body->children.size() == 1);
        auto* usage = dynamic_cast<TemplateUsageNode*>(body->children[0].get());
        REQUIRE(usage != nullptr);
        REQUIRE(usage->templateType == TemplateType::ELEMENT);
        REQUIRE(usage->name == "Box");
    }

    SECTION("Parse a top-level element template declaration") {
        CHTLContext context;
        Lexer lexer("[Template] @Element Box { div { text: \"hello\"; } }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.empty());

        REQUIRE(context.elementTemplates.count("Box") == 1);
        auto* tpl = context.elementTemplates["Box"].get();
        REQUIRE(tpl != nullptr);
        REQUIRE(tpl->templateType == TemplateType::ELEMENT);
        REQUIRE(tpl->name == "Box");
        REQUIRE(tpl->body.size() == 1); // The div

        auto* div = dynamic_cast<ElementNode*>(tpl->body[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->tagName == "div");
        REQUIRE(div->children.size() == 1);
    }
}

TEST_CASE("Parser correctly handles custom templates", "[parser]") {
    SECTION("Parse a top-level custom element declaration") {
        CHTLContext context;
        Lexer lexer("[Custom] @Element Card { div {} }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.empty());
        REQUIRE(context.customTemplates.count("Card") == 1);
        auto* tpl = context.customTemplates["Card"].get();
        REQUIRE(tpl != nullptr);
        REQUIRE(tpl->templateType == TemplateType::ELEMENT);
        REQUIRE(tpl->name == "Card");
        REQUIRE(tpl->body.size() == 1);
    }

    SECTION("Parse a custom element usage with delete") {
        CHTLContext context;
        Lexer lexer("body { @Element Card { delete div; } }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* body = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(body != nullptr);
        REQUIRE(body->children.size() == 1);

        auto* usage = dynamic_cast<TemplateUsageNode*>(body->children[0].get());
        // This test will require TemplateUsageNode to have a body for specializations
        // REQUIRE(usage->specializations.size() == 1);
        // auto* del = dynamic_cast<DeleteNode*>(usage->specializations[0].get());
        // REQUIRE(del != nullptr);
        // REQUIRE(del->targets[0] == "div");
    }

    SECTION("Parse a custom element usage with insert") {
        CHTLContext context;
        Lexer lexer("body { @Element Card { insert after div[0] { p {} } } }");
        Parser parser(lexer.tokenize(), context);
        auto ast = parser.parse();

        REQUIRE(ast->children.size() == 1);
        auto* body = dynamic_cast<ElementNode*>(ast->children[0].get());
        REQUIRE(body != nullptr);
        REQUIRE(body->children.size() == 1);

        // auto* usage = dynamic_cast<TemplateUsageNode*>(body->children[0].get());
        // REQUIRE(usage->specializations.size() == 1);
        // auto* insert = dynamic_cast<InsertNode*>(usage->specializations[0].get());
        // REQUIRE(insert != nullptr);
        // REQUIRE(insert->position == InsertPosition::AFTER);
        // REQUIRE(insert->targetSelector == "div[0]");
        // REQUIRE(insert->nodesToInsert.size() == 1);
    }
}

TEST_CASE("Parser handles nested structures", "[parser]") {
    SECTION("Parse nested elements") {
        CHTLContext context;
        Lexer lexer("html { body { div {} } }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer(R"(
            // This is a comment
            div {
                /* Another comment */
            }
            # generator comment
        )");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { id: my-id; }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { class = \"my-class\"; }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("a { href: \"/index.html\"; target = _blank; }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { id: main; h1 {} }");
        Parser parser(lexer.tokenize(), context);
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
        CHTLContext context;
        Lexer lexer("div { text: \"hello world\"; }");
        Parser parser(lexer.tokenize(), context);
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