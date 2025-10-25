#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.hpp"

TEST_CASE(LexerCanTokenizeComment, "Lexer can tokenize a simple comment") {
    std::string input = "// This is a comment";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::COMMENT);
    REQUIRE(tokens[0].value == " This is a comment");
}

TEST_CASE(LexerCanTokenizeMultiLineComment, "Lexer can tokenize a multi-line comment") {
    std::string input = "/* This is a multi-line comment */";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::MULTI_LINE_COMMENT);
    REQUIRE(tokens[0].value == " This is a multi-line comment ");
}

TEST_CASE(LexerCanTokenizeGeneratorComment, "Lexer can tokenize a generator comment") {
    std::string input = "# This is a generator comment";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::GENERATOR_COMMENT);
    REQUIRE(tokens[0].value == "This is a generator comment");
}

TEST_CASE(LexerCanTokenizeIdentifier, "Lexer can tokenize an identifier") {
    std::string input = "div_1";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[0].value == "div_1");
}

TEST_CASE(LexerCanTokenizeDoubleQuotedString, "Lexer can tokenize a double-quoted string") {
    std::string input = "\"hello world\"";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::STRING_LITERAL);
    REQUIRE(tokens[0].value == "hello world");
}

TEST_CASE(LexerCanTokenizePunctuation, "Lexer can tokenize punctuation") {
    std::string input = "{:;=}";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0].type == TokenType::LEFT_BRACE);
    REQUIRE(tokens[1].type == TokenType::COLON);
    REQUIRE(tokens[2].type == TokenType::SEMICOLON);
    REQUIRE(tokens[3].type == TokenType::EQUALS);
    REQUIRE(tokens[4].type == TokenType::RIGHT_BRACE);
}

TEST_CASE(LexerHandlesUnknownCharacters, "Lexer handles unknown characters") {
    std::string input = "@";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::UNKNOWN);
    REQUIRE(tokens[0].value == "@");
}

TEST_CASE(LexerCanTokenizeUnadornedLiteral, "Lexer can tokenize an unadorned literal") {
    std::string input = "color: red;";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[0].value == "color");
    REQUIRE(tokens[1].type == TokenType::COLON);
    REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[2].value == "red");
    REQUIRE(tokens[3].type == TokenType::SEMICOLON);
}
