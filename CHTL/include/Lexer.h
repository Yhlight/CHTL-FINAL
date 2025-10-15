#pragma once

#include "Token.h"
#include <string>

namespace CHTL
{
    class Lexer
    {
    public:
        Lexer(const std::string& input);

        Token NextToken();

    private:
        void readChar();
        char peekChar() const;
        void skipWhitespace();
        void skipSingleLineComment();
        void skipMultiLineComment();
        std::string readIdentifier();
        std::string readString();
        std::string readNumber();
        std::string readGeneratorComment();
        static bool isLetter(char ch);

        std::string m_input;         // 输入的源代码
        size_t m_position = 0;       // 当前读取的位置
        size_t m_readPosition = 0;   // 下一个要读取的位置
        char m_char = 0;             // 当前正在查看的字符
        int m_line = 1;              // 当前行号
        int m_column = 0;            // 当前列号
    };

} // namespace CHTL
