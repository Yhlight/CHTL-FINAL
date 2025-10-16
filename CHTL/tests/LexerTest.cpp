#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Lexer.h"
#include "Token.h"
#include <vector>
#include <iostream>

// Helper function to run tests on a sequence of tokens
void runLexerTest(const std::string& input, const std::vector<CHTL::Token>& expected_tokens) {
    CHTL::Lexer l(input);
    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        CHTL::Token tok = l.NextToken();
        const auto& expected = expected_tokens[i];

        INFO("Test case " << i << ": " << "Expected: " << CHTL::TokenTypeToString(expected.type) << " ('" << expected.literal << "'), Got: " << CHTL::TokenTypeToString(tok.type) << " ('" << tok.literal << "')");

        REQUIRE(tok.type == expected.type);
        REQUIRE(tok.literal == expected.literal);
    }
}

TEST_CASE("Test Full Lexer Logic", "[lexer]")
{
    std::string input = R"~(
        // Test basic tokens
        let size = 10;
        let other_size = 20.5;
        let formula = (5 + 2) * 3 ** 2 / 4 % 3;

        // Test all operators and delimiters
        = : ; + - * / % ** > < ? , ( ) { } [ ] @ . &

        // Test comments
        // single line comment
        /* multi-line
           comment */

        // Test strings and keywords
        text { content: "Hello World"; }
        style {
            color: red;
            inherit: base;
        }

        // Test block keywords
        [Template] [Custom] [Import] [Namespace]

        // Test illegal characters
        ^`~
    )~";

    std::vector<CHTL::Token> expected = {
        // let size = 10;
        {CHTL::TokenType::IDENT, "let"},
        {CHTL::TokenType::IDENT, "size"},
        {CHTL::TokenType::COLON, "="},
        {CHTL::TokenType::NUMBER, "10"},
        {CHTL::TokenType::SEMICOLON, ";"},

        // let other_size = 20.5;
        {CHTL::TokenType::IDENT, "let"},
        {CHTL::TokenType::IDENT, "other_size"},
        {CHTL::TokenType::COLON, "="},
        {CHTL::TokenType::NUMBER, "20.5"},
        {CHTL::TokenType::SEMICOLON, ";"},

        // let formula = (5 + 2) * 3 ** 2 / 4 % 3;
        {CHTL::TokenType::IDENT, "let"},
        {CHTL::TokenType::IDENT, "formula"},
        {CHTL::TokenType::COLON, "="},
        {CHTL::TokenType::LPAREN, "("},
        {CHTL::TokenType::NUMBER, "5"},
        {CHTL::TokenType::PLUS, "+"},
        {CHTL::TokenType::NUMBER, "2"},
        {CHTL::TokenType::RPAREN, ")"},
        {CHTL::TokenType::ASTERISK, "*"},
        {CHTL::TokenType::NUMBER, "3"},
        {CHTL::TokenType::POWER, "**"},
        {CHTL::TokenType::NUMBER, "2"},
        {CHTL::TokenType::SLASH, "/"},
        {CHTL::TokenType::NUMBER, "4"},
        {CHTL::TokenType::MODULO, "%"},
        {CHTL::TokenType::NUMBER, "3"},
        {CHTL::TokenType::SEMICOLON, ";"},

        // = : ; + - * / % ** > < ? , ( ) { } [ ] @ . &
        {CHTL::TokenType::COLON, "="},
        {CHTL::TokenType::COLON, ":"},
        {CHTL::TokenType::SEMICOLON, ";"},
        {CHTL::TokenType::PLUS, "+"},
        {CHTL::TokenType::MINUS, "-"},
        {CHTL::TokenType::ASTERISK, "*"},
        {CHTL::TokenType::SLASH, "/"},
        {CHTL::TokenType::MODULO, "%"},
        {CHTL::TokenType::POWER, "**"},
        {CHTL::TokenType::GT, ">"},
        {CHTL::TokenType::LT, "<"},
        {CHTL::TokenType::QUESTION, "?"},
        {CHTL::TokenType::COMMA, ","},
        {CHTL::TokenType::LPAREN, "("},
        {CHTL::TokenType::RPAREN, ")"},
        {CHTL::TokenType::LBRACE, "{"},
        {CHTL::TokenType::RBRACE, "}"},
        {CHTL::TokenType::LBRACKET, "["},
        {CHTL::TokenType::RBRACKET, "]"},
        {CHTL::TokenType::AT, "@"},
        {CHTL::TokenType::DOT, "."},
        {CHTL::TokenType::AMPERSAND, "&"},

        // text { content: "Hello World"; }
        {CHTL::TokenType::KEYWORD_TEXT, "text"},
        {CHTL::TokenType::LBRACE, "{"},
        {CHTL::TokenType::IDENT, "content"},
        {CHTL::TokenType::COLON, ":"},
        {CHTL::TokenType::STRING, "Hello World"},
        {CHTL::TokenType::SEMICOLON, ";"},
        {CHTL::TokenType::RBRACE, "}"},

        // style { color: red; inherit: base; }
        {CHTL::TokenType::KEYWORD_STYLE, "style"},
        {CHTL::TokenType::LBRACE, "{"},
        {CHTL::TokenType::IDENT, "color"},
        {CHTL::TokenType::COLON, ":"},
        {CHTL::TokenType::IDENT, "red"},
        {CHTL::TokenType::SEMICOLON, ";"},
        {CHTL::TokenType::KEYWORD_INHERIT, "inherit"},
        {CHTL::TokenType::COLON, ":"},
        {CHTL::TokenType::IDENT, "base"},
        {CHTL::TokenType::SEMICOLON, ";"},
        {CHTL::TokenType::RBRACE, "}"},

        // [Template] [Custom] [Import] [Namespace]
        {CHTL::TokenType::KEYWORD_TEMPLATE, "[Template]"},
        {CHTL::TokenType::KEYWORD_CUSTOM, "[Custom]"},
        {CHTL::TokenType::KEYWORD_IMPORT, "[Import]"},
        {CHTL::TokenType::KEYWORD_NAMESPACE, "[Namespace]"},

        // ^`~
        {CHTL::TokenType::ILLEGAL, "^"},
        {CHTL::TokenType::ILLEGAL, "`"},
        {CHTL::TokenType::ILLEGAL, "~"},

        {CHTL::TokenType::END_OF_FILE, ""}
    };

    runLexerTest(input, expected);
}

TEST_CASE("Test More Block Keywords", "[lexer]")
{
    std::string input = "[Origin] [Configuration]";
    std::vector<CHTL::Token> expected = {
        {CHTL::TokenType::KEYWORD_ORIGIN, "[Origin]"},
        {CHTL::TokenType::KEYWORD_CONFIGURATION, "[Configuration]"},
        {CHTL::TokenType::END_OF_FILE, ""}
    };
    runLexerTest(input, expected);
}

TEST_CASE("Test Number with Unit", "[lexer]")
{
    std::string input = "100px 2.5rem 50%";
    std::vector<CHTL::Token> expected = {
        {CHTL::TokenType::NUMBER, "100"},
        {CHTL::TokenType::IDENT, "px"},
        {CHTL::TokenType::NUMBER, "2.5"},
        {CHTL::TokenType::IDENT, "rem"},
        {CHTL::TokenType::NUMBER, "50"},
        {CHTL::TokenType::MODULO, "%"},
        {CHTL::TokenType::END_OF_FILE, ""}
    };
    runLexerTest(input, expected);
}

TEST_CASE("Test Single Quoted String", "[lexer]")
{
    std::string input = R"('hello world')";
    std::vector<CHTL::Token> expected = {
        {CHTL::TokenType::STRING, "hello world"},
        {CHTL::TokenType::END_OF_FILE, ""}
    };
    runLexerTest(input, expected);
}


TEST_CASE("Test generator comments", "[lexer]")
{
    SECTION("Valid generator comment")
    {
        std::string input = "# this is a generator comment";
        std::vector<CHTL::Token> expected = {
            {CHTL::TokenType::COMMENT, "this is a generator comment"},
            {CHTL::TokenType::END_OF_FILE, ""}
        };
        runLexerTest(input, expected);
    }

    SECTION("Invalid generator comment without space")
    {
        std::string input = "#invalid";
        std::vector<CHTL::Token> expected = {
            {CHTL::TokenType::ILLEGAL, "#"},
            {CHTL::TokenType::IDENT, "invalid"},
            {CHTL::TokenType::END_OF_FILE, ""}
        };
        runLexerTest(input, expected);
    }
}
