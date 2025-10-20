#pragma once

#include "lexer/Token.h"
#include <string>
#include <unordered_map>

namespace CHTL
{
    class Lexer
    {
    public:
        Lexer(const std::string& input, const std::unordered_map<std::string, Token>& keywords);

        Token NextToken();
        std::string readRawBlockContent();

        // Getters for parser to query state
        size_t GetPosition() const { return m_position; }
        size_t GetReadPosition() const { return m_readPosition; }
        const std::string& GetInput() const { return m_input; } // Needed for raw content parsing

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

        std::string m_input;         // 输入的源代码
        size_t m_position = 0;       // 当前读取的位置
        size_t m_readPosition = 0;   // 下一个要读取的位置
        char m_char = 0;             // 当前正在查看的字符
        int m_line = 1;              // 当前行号
        int m_column = 0;            // 当前列号
        const std::unordered_map<std::string, Token>& m_keywords;
    };

} // namespace CHTL
