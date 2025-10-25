#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <fstream>
#include <string>
#include <memory>

enum class LexerMode {
    NORMAL,
    SCRIPT_CONTENT
};

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& filepath);
    Token getNextToken();
    Token peekToken();
    void setMode(LexerMode newMode);

private:
    std::ifstream file;
    char currentChar;
    std::unique_ptr<Token> lookahead;
    LexerMode mode = LexerMode::NORMAL;

    void advance();
    void skipWhitespace();
    Token parseIdentifier();
    Token parseString();
    Token parseNumber();
    Token getNextTokenInternal();
};

#endif //CHTL_LEXER_H
