#include <gtest/gtest.h>
#include "CHTLLexer/Lexer.h"
#include "CHTLLexer/Token.h"
#include <vector>

TEST(LexerTest, ExtendedSyntax) {
    std::string input = R"(
// This is a comment
div {
    id: "main-box";
    class = 'some-class'; # another comment
    width: 100 + 5; /* multi-line
                       comment */
    height: 10 * 2;
    margin: 10 / 2;
    padding: 10 % 3;
    font-size: 2 ** 4;
    z-index: (1 > 0) && (2 < 3) || (4 != 5) ? 1 : 0;
}
)";

    std::vector<Token> expected_tokens = {
        {TokenType::TOKEN_COMMENT, "// This is a comment"},
        {TokenType::TOKEN_IDENTIFIER, "div"},
        {TokenType::TOKEN_LBRACE, "{"},
        {TokenType::TOKEN_IDENTIFIER, "id"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_STRING, "main-box"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_IDENTIFIER, "class"},
        {TokenType::TOKEN_EQUAL, "="},
        {TokenType::TOKEN_SINGLE_QUOTE_STRING, "some-class"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_COMMENT, "# another comment"},
        {TokenType::TOKEN_IDENTIFIER, "width"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_IDENTIFIER, "100"},
        {TokenType::TOKEN_PLUS, "+"},
        {TokenType::TOKEN_IDENTIFIER, "5"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_COMMENT, "/* multi-line\n                       comment */"},
        {TokenType::TOKEN_IDENTIFIER, "height"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_IDENTIFIER, "10"},
        {TokenType::TOKEN_ASTERISK, "*"},
        {TokenType::TOKEN_IDENTIFIER, "2"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_IDENTIFIER, "margin"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_IDENTIFIER, "10"},
        {TokenType::TOKEN_SLASH, "/"},
        {TokenType::TOKEN_IDENTIFIER, "2"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_IDENTIFIER, "padding"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_IDENTIFIER, "10"},
        {TokenType::TOKEN_PERCENT, "%"},
        {TokenType::TOKEN_IDENTIFIER, "3"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_IDENTIFIER, "font-size"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_IDENTIFIER, "2"},
        {TokenType::TOKEN_DOUBLE_ASTERISK, "**"},
        {TokenType::TOKEN_IDENTIFIER, "4"},
        {TokenType::TOKEN_SEMICOLON, ";"},
        {TokenType::TOKEN_IDENTIFIER, "z-index"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_LPAREN, "("},
        {TokenType::TOKEN_IDENTIFIER, "1"},
        {TokenType::TOKEN_GT, ">"},
        {TokenType::TOKEN_IDENTIFIER, "0"},
        {TokenType::TOKEN_RPAREN, ")"},
        {TokenType::TOKEN_AND, "&&"},
        {TokenType::TOKEN_LPAREN, "("},
        {TokenType::TOKEN_IDENTIFIER, "2"},
        {TokenType::TOKEN_LT, "<"},
        {TokenType::TOKEN_IDENTIFIER, "3"},
        {TokenType::TOKEN_RPAREN, ")"},
        {TokenType::TOKEN_OR, "||"},
        {TokenType::TOKEN_LPAREN, "("},
        {TokenType::TOKEN_IDENTIFIER, "4"},
        {TokenType::TOKEN_BANG, "!="},
        {TokenType::TOKEN_IDENTIFIER, "5"},
        {TokenType::TOKEN_RPAREN, ")"},
        {TokenType::TOKEN_QUESTION, "?"},
        {TokenType::TOKEN_IDENTIFIER, "1"},
        {TokenType::TOKEN_COLON, ":"},
        {TokenType::TOKEN_IDENTIFIER, "0"},
        {TokenType::TOKEN_SEMICOLON, ";"},
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