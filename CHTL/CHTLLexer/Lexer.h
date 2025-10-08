#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& input);
    Token NextToken();

private:
    void readChar();
    char peekChar();
    std::string readIdentifier();
    std::string readString();
    void skipWhitespace();

    std::string input;
    int position;      // current position in input (points to current char)
    int readPosition;  // current reading position in input (after current char)
    char ch;           // current char under examination
};