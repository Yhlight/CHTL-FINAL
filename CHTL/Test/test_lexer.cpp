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

TEST_CASE(LexerCanTokenizeCssValues, "Lexer can tokenize CSS values") {
    std::string input = "100px";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == TokenType::NUMBER);
    REQUIRE(tokens[0].value == "100");
    REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].value == "px");
}

TEST_CASE(LexerCanTokenizeCssSelectors, "Lexer can tokenize CSS selectors") {
    std::string input = ".class#id&";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0].type == TokenType::DOT);
    REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].value == "class");
    REQUIRE(tokens[2].type == TokenType::HASH);
    REQUIRE(tokens[3].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[3].value == "id");
    REQUIRE(tokens[4].type == TokenType::AMPERSAND);
}

TEST_CASE(LexerCanTokenizeArithmeticOperators, "Lexer can tokenize arithmetic operators") {
    std::string input = "+-*/%**";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 6);
    REQUIRE(tokens[0].type == TokenType::PLUS);
    REQUIRE(tokens[1].type == TokenType::MINUS);
    REQUIRE(tokens[2].type == TokenType::STAR);
    REQUIRE(tokens[3].type == TokenType::SLASH);
    REQUIRE(tokens[4].type == TokenType::PERCENT);
    REQUIRE(tokens[5].type == TokenType::POWER);
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
