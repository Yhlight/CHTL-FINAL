#pragma once

#include <string>
#include <unordered_map>

namespace CHTL
{
    // 定义Token的类型
    enum class TokenType
    {
        // 特殊Token
        ILLEGAL, // 非法字符
        END_OF_FILE, // 文件结束符
        COMMENT,     // # comment

        // 标识符 + 字面量
        IDENT, // 标识符: div, my_var, px
        STRING, // "hello world"
        NUMBER, // 123, 1.5

        // 关键字
        KEYWORD_TEXT,       // text
        KEYWORD_STYLE,      // style
        KEYWORD_TEMPLATE,   // [Template]
        KEYWORD_CUSTOM,     // [Custom]
        KEYWORD_DELETE,     // delete
        KEYWORD_IMPORT,     // [Import]
        KEYWORD_FROM,       // from
        KEYWORD_NAMESPACE,  // [Namespace]

        // 运算符
        COLON,      // :
        SEMICOLON,  // ;
        PLUS,       // +
        MINUS,      // -
        ASTERISK,   // *
        SLASH,      // /
        MODULO,     // %
        POWER,      // **
        GT,         // >
        LT,         // <

        // 分隔符
        LPAREN,     // (
        RPAREN,     // )
        LBRACE,     // {
        RBRACE,     // }
        LBRACKET,   // [
        RBRACKET,   // ]
        AT,         // @
        DOT,        // .
        AMPERSAND,  // &
        QUESTION,   // ?
        COMMA,      // ,
    };

    // Token结构体
    struct Token
    {
        TokenType type;
        std::string literal; // Token字面量
        int line = 0;       // Token所在行号
        int column = 0;     // Token所在列号

        std::string ToString() const;
    };

    // 将TokenType转换为字符串以便打印
    std::string TokenTypeToString(TokenType type);

    // 关键字映射表
    extern std::unordered_map<std::string, TokenType> keywords;

} // namespace CHTL
