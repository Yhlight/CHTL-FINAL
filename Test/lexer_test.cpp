#include <gtest/gtest.h>
#include "CHTLLexer/Token.h"
#include "CHTLLexer/Lexer.h"

TEST(LexerTest, TestSimpleTextNode) {
    std::string input = "text { \"some text\" }";

    std::vector<Token> expected_tokens = {
        {TokenType::TOKEN_TEXT, "text"},
        {TokenType::TOKEN_LBRACE, "{"},
        {TokenType::TOKEN_STRING, "some text"},
        {TokenType::TOKEN_RBRACE, "}"},
        {TokenType::TOKEN_EOF, ""}
    };

    Lexer l(input);

    for (const auto& expected_token : expected_tokens) {
        Token t = l.NextToken();
        EXPECT_EQ(t.type, expected_token.type);
        EXPECT_EQ(t.literal, expected_token.literal);
    }
}