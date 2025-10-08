#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"

TEST_CASE("Lexer correctly tokenizes comments", "[lexer]") {
    SECTION("Single-line comment") {
        Lexer lexer("// this is a comment\n");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::SINGLE_LINE_COMMENT);
        REQUIRE(tokens[0].value == "// this is a comment");
        REQUIRE(tokens[1].type == TokenType::END_OF_FILE);
    }

    SECTION("Multi-line comment") {
        Lexer lexer("/* this is a \n multi-line comment */");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::MULTI_LINE_COMMENT);
        REQUIRE(tokens[0].value == "/* this is a \n multi-line comment */");
        REQUIRE(tokens[1].type == TokenType::END_OF_FILE);
    }

    SECTION("Generator comment") {
        Lexer lexer("# this is a generator comment\n");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::GENERATOR_COMMENT);
        REQUIRE(tokens[0].value == "# this is a generator comment");
        REQUIRE(tokens[1].type == TokenType::END_OF_FILE);
    }
}

TEST_CASE("Lexer correctly tokenizes text nodes", "[lexer]") {
    SECTION("Simple text node") {
        Lexer lexer("text { \"this is text\" }");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens[0].type == TokenType::TEXT_KEYWORD);
        REQUIRE(tokens[1].type == TokenType::LEFT_BRACE);
        REQUIRE(tokens[2].type == TokenType::STRING_LITERAL);
        REQUIRE(tokens[2].value == "\"this is text\"");
        REQUIRE(tokens[3].type == TokenType::RIGHT_BRACE);
        REQUIRE(tokens[4].type == TokenType::END_OF_FILE);
    }
}

TEST_CASE("Lexer correctly tokenizes literals", "[lexer]") {
    SECTION("Single-quoted string literal") {
        Lexer lexer("'hello single quotes'");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].value == "'hello single quotes'");
        REQUIRE(tokens[1].type == TokenType::END_OF_FILE);
    }

    SECTION("Unquoted text in text node") {
        Lexer lexer("text { this is text }");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 7);
        REQUIRE(tokens[0].type == TokenType::TEXT_KEYWORD);
        REQUIRE(tokens[1].type == TokenType::LEFT_BRACE);
        REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[2].value == "this");
        REQUIRE(tokens[3].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[3].value == "is");
        REQUIRE(tokens[4].type == TokenType::TEXT_KEYWORD);
        REQUIRE(tokens[4].value == "text");
        REQUIRE(tokens[5].type == TokenType::RIGHT_BRACE);
        REQUIRE(tokens[6].type == TokenType::END_OF_FILE);
    }

    SECTION("Numeric literals") {
        Lexer lexer("123 45.6");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].type == TokenType::NUMERIC_LITERAL);
        REQUIRE(tokens[0].value == "123");
        REQUIRE(tokens[1].type == TokenType::NUMERIC_LITERAL);
        REQUIRE(tokens[1].value == "45.6");
        REQUIRE(tokens[2].type == TokenType::END_OF_FILE);
    }
}

TEST_CASE("Lexer correctly tokenizes operators", "[lexer]") {
    SECTION("Arithmetic operators") {
        Lexer lexer("+ - * / % **");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 7);
        REQUIRE(tokens[0].type == TokenType::PLUS);
        REQUIRE(tokens[1].type == TokenType::MINUS);
        REQUIRE(tokens[2].type == TokenType::STAR);
        REQUIRE(tokens[3].type == TokenType::SLASH);
        REQUIRE(tokens[4].type == TokenType::PERCENT);
        REQUIRE(tokens[5].type == TokenType::DOUBLE_STAR);
        REQUIRE(tokens[6].type == TokenType::END_OF_FILE);
    }
}

TEST_CASE("Lexer correctly tokenizes custom keywords", "[lexer]") {
    SECTION("Custom keywords") {
        Lexer lexer("[Custom] delete insert after before replace");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 9);
        REQUIRE(tokens[0].type == TokenType::LEFT_BRACKET);
        REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[1].value == "Custom");
        REQUIRE(tokens[2].type == TokenType::RIGHT_BRACKET);
        REQUIRE(tokens[3].type == TokenType::DELETE_KEYWORD);
        REQUIRE(tokens[4].type == TokenType::INSERT_KEYWORD);
        REQUIRE(tokens[5].type == TokenType::AFTER_KEYWORD);
        REQUIRE(tokens[6].type == TokenType::BEFORE_KEYWORD);
        REQUIRE(tokens[7].type == TokenType::REPLACE_KEYWORD);
        REQUIRE(tokens[8].type == TokenType::END_OF_FILE);
    }
}

TEST_CASE("Lexer correctly tokenizes template syntax", "[lexer]") {
    SECTION("Template declaration") {
        Lexer lexer("[Template] @Style DefaultText");
        auto tokens = lexer.tokenize();
        REQUIRE(tokens.size() == 7);
        REQUIRE(tokens[0].type == TokenType::LEFT_BRACKET);
        REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[1].value == "Template");
        REQUIRE(tokens[2].type == TokenType::RIGHT_BRACKET);
        REQUIRE(tokens[3].type == TokenType::AT);
        REQUIRE(tokens[4].type == TokenType::STYLE_KEYWORD);
        REQUIRE(tokens[5].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[5].value == "DefaultText");
        REQUIRE(tokens[6].type == TokenType::END_OF_FILE);
    }
}