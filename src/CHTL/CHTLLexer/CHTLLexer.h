#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <fstream>
#include <string>

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& filepath);
    Token getNextToken();

private:
    std::ifstream file;
    char currentChar;

    void advance();
    void skipWhitespace();
    Token parseIdentifier();
    Token parseString();
};

#endif //CHTL_LEXER_H
