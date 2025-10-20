#pragma once

#include "lexer/Token.h" // We can reuse the main Token definitions
#include <string>
#include <vector>

namespace CHTL
{
    class ConfigLexer
    {
    public:
        ConfigLexer(const std::string& input);

        Token NextToken();

    private:
        void readChar();
        char peekChar() const;
        void skipWhitespace();
        std::string readIdentifier();
        static bool isLetter(char ch);

        std::string m_input;
        size_t m_position = 0;
        size_t m_readPosition = 0;
        char m_char = 0;
        int m_line = 1;
        int m_column = 0;
    };

} // namespace CHTL
