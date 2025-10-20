#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "config/ConfigLexer.h"
#include "lexer/Token.h"

TEST_CASE("Test ConfigLexer tokenization", "[config_lexer]")
{
    SECTION("Simple bracketed identifier")
    {
        std::string input = "[Template]";
        CHTL::ConfigLexer lexer(input);

        std::vector<CHTL::TokenType> expected_tokens = {
            CHTL::TokenType::LBRACKET,
            CHTL::TokenType::IDENT,
            CHTL::TokenType::RBRACKET,
            CHTL::TokenType::END_OF_FILE,
        };

        for (const auto& expected_type : expected_tokens)
        {
            CHTL::Token tok = lexer.NextToken();
            REQUIRE(tok.type == expected_type);
        }
    }

    SECTION("Simple At identifier")
    {
        std::string input = "@Style";
        CHTL::ConfigLexer lexer(input);

        std::vector<CHTL::TokenType> expected_tokens = {
            CHTL::TokenType::AT,
            CHTL::TokenType::IDENT,
            CHTL::TokenType::END_OF_FILE,
        };

        for (const auto& expected_type : expected_tokens)
        {
            CHTL::Token tok = lexer.NextToken();
            REQUIRE(tok.type == expected_type);
        }
    }

    SECTION("Group of At identifiers")
    {
        std::string input = "[@Style, @style, @CSS]";
        CHTL::ConfigLexer lexer(input);

        std::vector<CHTL::TokenType> expected_tokens = {
            CHTL::TokenType::LBRACKET,
            CHTL::TokenType::AT,
            CHTL::TokenType::IDENT,
            CHTL::TokenType::COMMA,
            CHTL::TokenType::AT,
            CHTL::TokenType::IDENT,
            CHTL::TokenType::COMMA,
            CHTL::TokenType::AT,
            CHTL::TokenType::IDENT,
            CHTL::TokenType::RBRACKET,
            CHTL::TokenType::END_OF_FILE,
        };

        for (const auto& expected_type : expected_tokens)
        {
            CHTL::Token tok = lexer.NextToken();
            REQUIRE(tok.type == expected_type);
        }
    }

    SECTION("Plain identifier")
    {
        std::string input = "inherit";
        CHTL::ConfigLexer lexer(input);

        CHTL::Token tok = lexer.NextToken();
        REQUIRE(tok.type == CHTL::TokenType::IDENT);
        REQUIRE(tok.literal == "inherit");

        tok = lexer.NextToken();
        REQUIRE(tok.type == CHTL::TokenType::END_OF_FILE);
    }
}
