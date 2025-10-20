#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "lexer/Lexer.h"
#include "lexer/Token.h"

#include <vector>
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
        CHTL::Lexer l(input, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
        auto tokens = collectTokens(l);

        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == CHTL::TokenType::END_OF_FILE);
    }

    SECTION("Handles unterminated multi-line comment")
    {
        std::string input = "/* This comment is not closed";
        CHTL::Lexer l(input, s_default_keywords);
        auto tokens = collectTokens(l);

        // The lexer should just reach EOF
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == CHTL::TokenType::END_OF_FILE);
    }
}
