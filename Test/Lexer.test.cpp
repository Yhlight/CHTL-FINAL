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