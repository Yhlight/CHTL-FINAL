#include "catch.hpp"
#include "CHTLParser/CHTLParser.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TextNode.hpp"
#include "CHTLNode/StyleNode.hpp"
#include "CHTLNode/PropertyNode.hpp"
#include "CHTLNode/RuleNode.hpp"
#include "CHTLNode/ExpressionNode.hpp"
#include "CHTLNode/LiteralNode.hpp"
#include "CHTLNode/BinaryOpNode.hpp"

TEST_CASE(ParserCanParseSimpleElement, "Parser can parse a simple element") {
    std::string input = "div {}";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* element = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(element != nullptr);
    REQUIRE(element->tag_name == "div");
    REQUIRE(element->children.empty());
}

TEST_CASE(ParserCanParseNestedElements, "Parser can parse nested elements") {
    std::string input = "div { span {} }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->tag_name == "div");
    REQUIRE(div->children.size() == 1);

    ElementNode* span = dynamic_cast<ElementNode*>(div->children[0].get());
    REQUIRE(span != nullptr);
    REQUIRE(span->tag_name == "span");
    REQUIRE(span->children.empty());
}

TEST_CASE(ParserCanParseTextNodes, "Parser can parse text nodes") {
    std::string input = "div { text { \"hello\" } }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->tag_name == "div");
    REQUIRE(div->children.size() == 1);

    TextNode* text = dynamic_cast<TextNode*>(div->children[0].get());
    REQUIRE(text != nullptr);
    REQUIRE(text->text == "hello");
}

TEST_CASE(ParserCanParseAttributes, "Parser can parse attributes") {
    std::string input = "div { id: \"box\"; }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->tag_name == "div");
    REQUIRE(div->attributes.size() == 1);
    REQUIRE(div->attributes["id"] == "box");
    REQUIRE(div->children.empty());
}

TEST_CASE(ParserCanParseUnadornedAttributes, "Parser can parse unadorned attributes") {
    std::string input = "div { class: box; }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->tag_name == "div");
    REQUIRE(div->attributes.size() == 1);
    REQUIRE(div->attributes["class"] == "box");
    REQUIRE(div->children.empty());
}

TEST_CASE(ParserCanParseStyleBlocks, "Parser can parse style blocks") {
    std::string input = "div { style { width: 100px; } }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);

    StyleNode* style = dynamic_cast<StyleNode*>(div->children[0].get());
    REQUIRE(style != nullptr);
    REQUIRE(style->properties.size() == 1);
    REQUIRE(style->properties[0]->key == "width");
    LiteralNode* literal = dynamic_cast<LiteralNode*>(style->properties[0]->value.get());
    REQUIRE(literal != nullptr);
    REQUIRE(literal->value == "100px");
}

TEST_CASE(ParserCanParseCssRules, "Parser can parse CSS rules") {
    std::string input = "div { style { .box { color: red; } } }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);

    StyleNode* style = dynamic_cast<StyleNode*>(div->children[0].get());
    REQUIRE(style != nullptr);
    REQUIRE(style->rules.size() == 1);
    REQUIRE(style->rules[0]->selector == ".box");
    REQUIRE(style->rules[0]->properties.size() == 1);
    REQUIRE(style->rules[0]->properties[0]->key == "color");
    LiteralNode* literal = dynamic_cast<LiteralNode*>(style->rules[0]->properties[0]->value.get());
    REQUIRE(literal != nullptr);
    REQUIRE(literal->value == "red");
}

TEST_CASE(ParserCanParseExpressions, "Parser can parse expressions") {
    std::string input = "div { style { width: 100px + 50px; } }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();

    REQUIRE(root != nullptr);
    ElementNode* div = dynamic_cast<ElementNode*>(root.get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);

    StyleNode* style = dynamic_cast<StyleNode*>(div->children[0].get());
    REQUIRE(style != nullptr);
    REQUIRE(style->properties.size() == 1);
    REQUIRE(style->properties[0]->key == "width");

    BinaryOpNode* binOp = dynamic_cast<BinaryOpNode*>(style->properties[0]->value.get());
    REQUIRE(binOp != nullptr);
    REQUIRE(binOp->op == "+");

    LiteralNode* left = dynamic_cast<LiteralNode*>(binOp->left.get());
    REQUIRE(left != nullptr);
    REQUIRE(left->value == "100px");

    LiteralNode* right = dynamic_cast<LiteralNode*>(binOp->right.get());
    REQUIRE(right != nullptr);
    REQUIRE(right->value == "50px");
}
