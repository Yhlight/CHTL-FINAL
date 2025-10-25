#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <fstream>
#include <string>
#include <memory>

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& filepath);
    Token getNextToken();
    Token peekToken();

private:
    std::ifstream file;
    char currentChar;
    std::unique_ptr<Token> lookahead;

    void advance();
    void skipWhitespace();
    Token parseIdentifier();
    Token parseString();
    Token getNextTokenInternal();
};

#endif //CHTL_LEXER_H
