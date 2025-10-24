#include <gtest/gtest.h>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLLexer/Token.h"

TEST(LexerTest, HelloWorld) {
    std::string source = "text { \"Hello, World!\" }";
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, CHTL::TokenType::Text);
    EXPECT_EQ(tokens[0].lexeme, "text");

    EXPECT_EQ(tokens[1].type, CHTL::TokenType::OpenBrace);
    EXPECT_EQ(tokens[1].lexeme, "{");

    EXPECT_EQ(tokens[2].type, CHTL::TokenType::String);
    EXPECT_EQ(tokens[2].lexeme, "Hello, World!");

    EXPECT_EQ(tokens[3].type, CHTL::TokenType::CloseBrace);
    EXPECT_EQ(tokens[3].lexeme, "}");

    EXPECT_EQ(tokens[4].type, CHTL::TokenType::EndOfFile);
}
