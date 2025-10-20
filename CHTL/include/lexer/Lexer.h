#pragma once

#include "lexer/Token.h"
#include <string>
#include <map>

namespace CHTL
{
    class Lexer
    {
    public:
        // The constructor now accepts a map of keywords to support dynamic keywords.
        Lexer(const std::string& input, std::map<std::string, Token> keywords);

        Token NextToken();
        std::string readRawBlockContent();

        // Getters for parser to query state
        size_t GetPosition() const { return m_position; }
        size_t GetReadPosition() const { return m_readPosition; }
        const std::string& GetInput() const { return m_input; }

    private:
        void readChar();
        char peekChar() const;
        void skipWhitespace();
        void skipSingleLineComment();
        void skipMultiLineComment();
        std::string readIdentifier();
        std::string readString(char quote_type);
        std::string readNumber();
        std::string readComment();
        Token readBlockKeyword();
        static bool isLetter(char ch);

        std::string m_input;
        size_t m_position = 0;
        size_t m_readPosition = 0;
        char m_char = 0;
        int m_line = 1;
        int m_column = 0;

        // The lexer now holds its own copy of the keyword map.
        std::map<std::string, Token> m_keywords;
    };

} // namespace CHTL
