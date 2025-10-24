#pragma once

#include "Token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    Token nextToken();
    char peek();
    char advance();
    bool isAtEnd();

    std::string source;
    int start = 0;
    int current = 0;
    int line = 1;
};
