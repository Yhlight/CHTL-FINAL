#pragma once

#include "CHTLJS/include/lexer/Token.h"
#include <string>
#include <vector>

namespace CHTLJS
{
    /**
     * @class EnhancedLexer
     * @brief 增强的词法分析器，能够识别所有CHTL JS特殊语法
     * 
     * 支持:
     * - {{ ... }} 增强选择器
     * - &-> 事件绑定操作符
     * - -> 箭头操作符（方法调用）
     * - $ ... $ 响应式值
     * - CHTL JS关键字（ScriptLoader, Listen, Router等）
     */
    class EnhancedLexer
    {
    public:
        EnhancedLexer(const std::string& input);
        
        Token NextToken();
        
    private:
        void readChar();
        char peekChar() const;
        char peekCharAt(size_t offset) const;
        void skipWhitespace();
        bool isLetter(char ch) const;
        bool isDigit(char ch) const;
        std::string readIdentifier();
        std::string readNumber();
        std::string readString(char quote);
        
        // 检查是否是CHTL JS关键字
        bool isKeyword(const std::string& str) const;
        TokenType getKeywordType(const std::string& str) const;
        
        std::string m_input;
        size_t m_position = 0;
        size_t m_readPosition = 0;
        char m_char = 0;
        int m_line = 1;
        int m_column = 0;
        
        bool m_insideEnhancedSelector = false;  // 跟踪是否在{{}}内部
        bool m_insideReactive = false;  // 跟踪是否在$$内部
    };

} // namespace CHTLJS
