#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(std::string input);

    Token nextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();

    Token readIdentifier();
    Token readStringLiteral(char quote_type);
    Token readNumericLiteral();
    Token readBracketedKeyword();

    std::string m_input;
    size_t m_position;      // current position in input (points to current char)
    size_t m_readPosition;  // current reading position in input (after current char)
    char m_char;            // current char under examination
    int m_line;
    int m_column;

    static const std::unordered_map<std::string, TokenType> keywords;
};

} // namespace CHTL

#endif // CHTL_LEXER_H