#include "CHTLJS/include/lexer/EnhancedLexer.h"
#include <cctype>
#include <unordered_set>

namespace CHTLJS
{
    // CHTL JS关键字集合
    static const std::unordered_set<std::string> keywords = {
        "ScriptLoader", "Listen", "Delegate", "Animate", "Vir", "Router",
        "load", "target", "click", "mouseenter", "mouseleave", "mousedown", "mouseup",
        "duration", "easing", "begin", "when", "end", "loop", "direction", "delay", "callback",
        "url", "page", "root", "mode", "at"
    };

    EnhancedLexer::EnhancedLexer(const std::string& input)
        : m_input(input)
    {
        readChar();
    }

    void EnhancedLexer::readChar()
    {
        if (m_readPosition >= m_input.length())
        {
            m_char = 0;
        }
        else
        {
            m_char = m_input[m_readPosition];
        }
        m_position = m_readPosition;
        m_readPosition++;
        if (m_char == '\n')
        {
            m_line++;
            m_column = 0;
        }
        else
        {
            m_column++;
        }
    }

    char EnhancedLexer::peekChar() const
    {
        if (m_readPosition >= m_input.length())
        {
            return 0;
        }
        return m_input[m_readPosition];
    }

    char EnhancedLexer::peekCharAt(size_t offset) const
    {
        size_t pos = m_readPosition + offset - 1;
        if (pos >= m_input.length())
        {
            return 0;
        }
        return m_input[pos];
    }

    void EnhancedLexer::skipWhitespace()
    {
        while (isspace(m_char))
        {
            readChar();
        }
    }

    bool EnhancedLexer::isLetter(char ch) const
    {
        return isalpha(ch) || ch == '_';
    }

    bool EnhancedLexer::isDigit(char ch) const
    {
        return std::isdigit(ch);
    }

    std::string EnhancedLexer::readIdentifier()
    {
        size_t start = m_position;
        while (isLetter(m_char) || isDigit(m_char))
        {
            readChar();
        }
        return m_input.substr(start, m_position - start);
    }

    std::string EnhancedLexer::readNumber()
    {
        size_t start = m_position;
        while (isDigit(m_char) || m_char == '.')
        {
            readChar();
        }
        return m_input.substr(start, m_position - start);
    }

    std::string EnhancedLexer::readString(char quote)
    {
        readChar(); // skip opening quote
        size_t start = m_position;
        while (m_char != quote && m_char != 0)
        {
            if (m_char == '\\')
            {
                readChar(); // skip escape char
            }
            readChar();
        }
        std::string str = m_input.substr(start, m_position - start);
        if (m_char == quote)
        {
            readChar(); // skip closing quote
        }
        return str;
    }

    bool EnhancedLexer::isKeyword(const std::string& str) const
    {
        return keywords.find(str) != keywords.end();
    }

    TokenType EnhancedLexer::getKeywordType(const std::string& str) const
    {
        return TokenType::IDENT;  // 所有关键字都作为IDENT返回，由Parser处理
    }

    Token EnhancedLexer::NextToken()
    {
        Token tok;
        tok.line = m_line;
        tok.column = m_column;

        skipWhitespace();

        // 处理 {{
        if (m_char == '{' && peekChar() == '{')
        {
            readChar();
            readChar();
            m_insideEnhancedSelector = true;
            return {TokenType::LBRACE_BRACE, "{{", tok.line, tok.column};
        }

        // 处理 }}
        if (m_char == '}' && peekChar() == '}')
        {
            readChar();
            readChar();
            m_insideEnhancedSelector = false;
            return {TokenType::RBRACE_BRACE, "}}", tok.line, tok.column};
        }

        // 处理响应式值 $
        if (m_char == '$')
        {
            if (m_insideReactive)
            {
                // 结束响应式值
                readChar();
                m_insideReactive = false;
                return {TokenType::DOLLAR, "$", tok.line, tok.column};
            }
            else
            {
                // 开始响应式值
                readChar();
                m_insideReactive = true;
                return {TokenType::DOLLAR, "$", tok.line, tok.column};
            }
        }

        // 处理 &->
        if (m_char == '&' && peekChar() == '-' && peekCharAt(1) == '>')
        {
            readChar();
            readChar();
            readChar();
            return {TokenType::BIND_OP, "&->", tok.line, tok.column};
        }

        // 处理 ->
        if (m_char == '-' && peekChar() == '>')
        {
            readChar();
            readChar();
            return {TokenType::ARROW, "->", tok.line, tok.column};
        }

        // 处理单字符token
        switch (m_char)
        {
            case '{':
                readChar();
                return {TokenType::LBRACE, "{", tok.line, tok.column};
            case '}':
                readChar();
                return {TokenType::RBRACE, "}", tok.line, tok.column};
            case '(':
                readChar();
                return {TokenType::LPAREN, "(", tok.line, tok.column};
            case ')':
                readChar();
                return {TokenType::RPAREN, ")", tok.line, tok.column};
            case '[':
                readChar();
                return {TokenType::LBRACKET, "[", tok.line, tok.column};
            case ']':
                readChar();
                return {TokenType::RBRACKET, "]", tok.line, tok.column};
            case ',':
                readChar();
                return {TokenType::COMMA, ",", tok.line, tok.column};
            case ':':
                readChar();
                return {TokenType::COLON, ":", tok.line, tok.column};
            case ';':
                readChar();
                return {TokenType::SEMICOLON, ";", tok.line, tok.column};
            case '=':
                readChar();
                return {TokenType::ASSIGN, "=", tok.line, tok.column};
            case 0:
                return {TokenType::END_OF_FILE, "", tok.line, tok.column};
        }

        // 处理字符串
        if (m_char == '"' || m_char == '\'' || m_char == '`')
        {
            char quote = m_char;
            std::string str = readString(quote);
            return {TokenType::STRING, str, tok.line, tok.column};
        }

        // 处理数字
        if (isDigit(m_char))
        {
            std::string num = readNumber();
            return {TokenType::NUMBER, num, tok.line, tok.column};
        }

        // 处理标识符和关键字
        if (isLetter(m_char))
        {
            std::string ident = readIdentifier();
            return {TokenType::IDENT, ident, tok.line, tok.column};
        }

        // 其他字符作为RAW_JS的一部分
        // 收集直到遇到特殊字符
        size_t start = m_position;
        while (m_char != 0 && 
               m_char != '{' && m_char != '}' && 
               m_char != '(' && m_char != ')' &&
               m_char != '[' && m_char != ']' &&
               m_char != ',' && m_char != ':' && m_char != ';' &&
               m_char != '=' && m_char != '&' && m_char != '-' && m_char != '$' &&
               m_char != '"' && m_char != '\'' && m_char != '`' &&
               !isLetter(m_char) && !isDigit(m_char) && !isspace(m_char))
        {
            readChar();
        }
        
        if (m_position > start)
        {
            return {TokenType::RAW_JS, m_input.substr(start, m_position - start), tok.line, tok.column};
        }

        // 如果什么都没匹配到，返回一个错误token
        char ch = m_char;
        readChar();
        return {TokenType::RAW_JS, std::string(1, ch), tok.line, tok.column};
    }

} // namespace CHTLJS
