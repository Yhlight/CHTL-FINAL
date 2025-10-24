#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    Token scanToken();
    char advance();
    bool isAtEnd();
    char peek();
    void skipWhitespaceAndComments();
    Token string(char delimiter);
    Token identifier();
    Token unquotedLiteral();

    const std::string m_source;
    int m_start = 0;
    int m_current = 0;
    int m_line = 1;
    int m_column = 1;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
