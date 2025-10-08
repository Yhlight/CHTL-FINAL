#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t current = 0;
    int line = 1;
    int column = 1;

    Token nextToken();
    char advance();
    char peek();
    char peekNext();
    bool isAtEnd();
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& value);
    Token text();
    Token identifier();
    Token stringLiteral();
    Token singleLineComment();
    Token multiLineComment();
    Token generatorComment();
};