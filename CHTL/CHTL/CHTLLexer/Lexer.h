#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

    class Lexer {
    public:
        Lexer(const std::string& input);
        Token NextToken();

    private:
        void ReadChar();
        char PeekChar() const;
        void SkipWhitespace();
        std::string ReadIdentifier();
        std::string ReadNumber();
        std::string ReadString();
        TokenType LookupIdent(const std::string& ident);

        std::string input;
        size_t position;      // current position in input (points to current char)
        size_t readPosition;  // current reading position in input (after current char)
        char ch;              // current char under examination
    };

} // namespace CHTL
