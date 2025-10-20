#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "AstNode.h"

#include <string>
#include <memory>
#include <unordered_map>

// Create a default keyword map for testing purposes
static const std::unordered_map<std::string, CHTL::Token> s_default_keywords = {
    {"text", {CHTL::TokenType::KEYWORD_TEXT, "text"}},
    {"style", {CHTL::TokenType::KEYWORD_STYLE, "style"}},
    {"script", {CHTL::TokenType::KEYWORD_SCRIPT, "script"}},
    {"inherit", {CHTL::TokenType::KEYWORD_INHERIT, "inherit"}},
    {"delete", {CHTL::TokenType::KEYWORD_DELETE, "delete"}},
    {"insert", {CHTL::TokenType::KEYWORD_INSERT, "insert"}},
    {"after", {CHTL::TokenType::KEYWORD_AFTER, "after"}},
    {"before", {CHTL::TokenType::KEYWORD_BEFORE, "before"}},
    {"replace", {CHTL::TokenType::KEYWORD_REPLACE, "replace"}},
    {"at", {CHTL::TokenType::KEYWORD_AT, "at"}},
    {"top", {CHTL::TokenType::KEYWORD_TOP, "top"}},
    {"bottom", {CHTL::TokenType::KEYWORD_BOTTOM, "bottom"}},
    {"from", {CHTL::TokenType::KEYWORD_FROM, "from"}},
    {"as", {CHTL::TokenType::KEYWORD_AS, "as"}},
    {"except", {CHTL::TokenType::KEYWORD_EXCEPT, "except"}},
    {"use", {CHTL::TokenType::KEYWORD_USE, "use"}},
    {"html5", {CHTL::TokenType::KEYWORD_HTML5, "html5"}},
    {"if", {CHTL::TokenType::KEYWORD_IF, "if"}},
    {"else", {CHTL::TokenType::KEYWORD_ELSE, "else"}},
    {"[Template]", {CHTL::TokenType::KEYWORD_TEMPLATE, "[Template]"}},
    {"[Custom]", {CHTL::TokenType::KEYWORD_CUSTOM, "[Custom]"}},
    {"[Import]", {CHTL::TokenType::KEYWORD_IMPORT, "[Import]"}},
    {"[Namespace]", {CHTL::TokenType::KEYWORD_NAMESPACE, "[Namespace]"}},
    {"[Origin]", {CHTL::TokenType::KEYWORD_ORIGIN, "[Origin]"}},
    {"[Configuration]", {CHTL::TokenType::KEYWORD_CONFIGURATION, "[Configuration]"}},
    {"[Name]", {CHTL::TokenType::KEYWORD_NAME, "[Name]"}},
};


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


TEST_CASE("Parser correctly parses simple element and text nodes", "[parser]")
{
    SECTION("Parses a simple div with a text node")
    {
        std::string input = R"(
            div {
                text { "Hello" }
            }
        )";

        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 1);

        // Check the element node
        auto* element_node_ptr = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(element_node_ptr != nullptr);
        REQUIRE(element_node_ptr->tag_name == "div");
        REQUIRE(element_node_ptr->children.size() == 1);

        // Check the text node inside the element
        auto* text_node_ptr = dynamic_cast<CHTL::TextNode*>(element_node_ptr->children[0].get());
        REQUIRE(text_node_ptr != nullptr);
        REQUIRE(text_node_ptr->value == "Hello");
    }

    SECTION("Parses multiple top-level elements")
    {
        std::string input = R"(
            div {}
            span {}
        )";

        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 2);

        auto* first_element = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(first_element != nullptr);
        REQUIRE(first_element->tag_name == "div");

        auto* second_element = dynamic_cast<CHTL::ElementNode*>(program->children[1].get());
        REQUIRE(second_element != nullptr);
        REQUIRE(second_element->tag_name == "span");
    }
}

TEST_CASE("Parser correctly parses attributes", "[parser]")
{
    SECTION("Parses various attribute types")
    {
        std::string input = R"(
            div {
                id: box;
                class = "container";
                width: 100;
                height = 25.5;
            }
        )";

        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 1);

        auto* el = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(el != nullptr);
        REQUIRE(el->tag_name == "div");
        REQUIRE(el->attributes.size() == 4);

        // Check attributes
        REQUIRE(el->attributes[0].name == "id");
        REQUIRE(el->attributes[0].value == "box");

        REQUIRE(el->attributes[1].name == "class");
        REQUIRE(el->attributes[1].value == "container");

        REQUIRE(el->attributes[2].name == "width");
        REQUIRE(el->attributes[2].value == "100");

        REQUIRE(el->attributes[3].name == "height");
        REQUIRE(el->attributes[3].value == "25.5");
    }
}

TEST_CASE("Parser correctly parses style blocks", "[parser]")
{
    SECTION("Parses a style block with multiple properties")
    {
        std::string input = R"(
            div {
                style {
                    width: 100;
                    color: 1;
                    background-color = 2;
                }
            }
        )";

        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);
        REQUIRE(program->children.size() == 1);

        auto* el = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(el != nullptr);
        REQUIRE(el->children.size() == 1);

        auto* style_node = dynamic_cast<CHTL::StyleNode*>(el->children[0].get());
        REQUIRE(style_node != nullptr);
        REQUIRE(style_node->children.size() == 3);

        // Check style properties
        auto* prop1 = dynamic_cast<CHTL::StyleProperty*>(style_node->children[0].get());
        REQUIRE(prop1 != nullptr);
        REQUIRE(prop1->name == "width");
        REQUIRE(prop1->value->ToString() == "100.000000");

        auto* prop2 = dynamic_cast<CHTL::StyleProperty*>(style_node->children[1].get());
        REQUIRE(prop2 != nullptr);
        REQUIRE(prop2->name == "color");
        REQUIRE(prop2->value->ToString() == "1.000000");

        auto* prop3 = dynamic_cast<CHTL::StyleProperty*>(style_node->children[2].get());
        REQUIRE(prop3 != nullptr);
        REQUIRE(prop3->name == "background-color");
        REQUIRE(prop3->value->ToString() == "2.000000");
    }
}

TEST_CASE("Parser correctly parses infix expressions", "[parser]")
{
    SECTION("Parses a simple addition expression")
    {
        std::string input = "style { width: 100 + 50; }";

        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
        auto program = p.ParseProgram();

        checkParserErrors(p);
        REQUIRE(program != nullptr);

        auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
        REQUIRE(style_node != nullptr);
        REQUIRE(style_node->children.size() == 1);

        auto* prop = dynamic_cast<CHTL::StyleProperty*>(style_node->children[0].get());
        REQUIRE(prop != nullptr);
        auto* infix_expr = dynamic_cast<CHTL::InfixExpression*>(prop->value.get());
        REQUIRE(infix_expr != nullptr);
        REQUIRE(infix_expr->op == "+");

        auto* left = dynamic_cast<CHTL::NumberLiteral*>(infix_expr->left.get());
        REQUIRE(left != nullptr);
        REQUIRE(left->value == 100.0);

        auto* right = dynamic_cast<CHTL::NumberLiteral*>(infix_expr->right.get());
        REQUIRE(right != nullptr);
        REQUIRE(right->value == 50.0);
    }
}
