#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Lexer.h"
#include "Token.h"
#include <vector>

TEST_CASE("Test generator comments", "[lexer]")
{
    SECTION("Valid generator comment")
    {
        std::string input = "# this is a generator comment";
        CHTL::Lexer l(input);
        CHTL::Token tok = l.NextToken();
        REQUIRE(tok.type == CHTL::TokenType::COMMENT);
        REQUIRE(tok.literal == "this is a generator comment");
        tok = l.NextToken();
        REQUIRE(tok.type == CHTL::TokenType::END_OF_FILE);
    }

    SECTION("Invalid generator comment without space")
    {
        std::string input = "#invalid";
        CHTL::Lexer l(input);
        CHTL::Token tok = l.NextToken();
        // According to CHTL.md, a generator comment must be '# + space + content'.
        // So, '#invalid' should be treated as illegal.
        // For now, let's expect the '#' to be illegal and the rest to be an identifier.
        // This might need adjustment after fixing the lexer logic.
        REQUIRE(tok.type == CHTL::TokenType::ILLEGAL);
        REQUIRE(tok.literal == "#");
    }
}
