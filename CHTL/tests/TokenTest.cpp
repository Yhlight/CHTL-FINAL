#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Lexer.h"
#include "Token.h"

#include <vector>

// 辅助函数，用于收集一个Lexer产生的所有Token
std::vector<CHTL::Token> collectTokens(CHTL::Lexer& lexer)
{
    std::vector<CHTL::Token> tokens;
    CHTL::Token tok;
    do
    {
        tok = lexer.NextToken();
        tokens.push_back(tok);
    } while (tok.type != CHTL::TokenType::END_OF_FILE);
    return tokens;
}

TEST_CASE("Lexer correctly skips comments and whitespace", "[lexer]")
{
    SECTION("Skips single-line comments")
    {
        std::string input = R"(
            // This is a comment.
            // Another comment.
        )";
        CHTL::Lexer l(input);
        auto tokens = collectTokens(l);

        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == CHTL::TokenType::END_OF_FILE);
    }

    SECTION("Skips multi-line comments")
    {
        std::string input = R"(
            /* This is a
               multi-line comment. */
            /* Another one */
        )";
        CHTL::Lexer l(input);
        auto tokens = collectTokens(l);

        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == CHTL::TokenType::END_OF_FILE);
    }

    SECTION("Skips mixed comments and whitespace")
    {
        std::string input = R"(
            // Single line
            /* Multi-line
               comment here. */

            // Another single line
        )";
        CHTL::Lexer l(input);
        auto tokens = collectTokens(l);

        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == CHTL::TokenType::END_OF_FILE);
    }

    SECTION("Handles unterminated multi-line comment")
    {
        std::string input = "/* This comment is not closed";
        CHTL::Lexer l(input);
        auto tokens = collectTokens(l);

        // The lexer should just reach EOF
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == CHTL::TokenType::END_OF_FILE);
    }
}

TEST_CASE("Lexer correctly tokenizes generator comments", "[lexer]")
{
    SECTION("Tokenizes a single generator comment")
    {
        std::string input = "# this is a generator comment";
        CHTL::Lexer l(input);
        auto tokens = collectTokens(l);

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
        REQUIRE(tokens[0].literal == "# this is a generator comment");
        REQUIRE(tokens[1].type == CHTL::TokenType::END_OF_FILE);
    }
}
