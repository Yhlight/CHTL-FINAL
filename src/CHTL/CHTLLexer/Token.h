#pragma once

#include <string>
#include <ostream>

namespace CHTL {

// Token 类型枚举
enum class TokenType {
    // 基础类型
    END_OF_FILE,
    UNKNOWN,
    
    // 字面量
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量 "..." 或 '...'
    UNQUOTED_LITERAL,    // 无修饰字面量
    NUMBER,              // 数字
    
    // 关键字
    KEYWORD_TEXT,        // text
    KEYWORD_STYLE,       // style
    KEYWORD_SCRIPT,      // script
    
    // 括号
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    
    // 分隔符
    SEMICOLON,           // ;
    COLON,               // :
    COMMA,               // ,
    DOT,                 // .
    
    // 运算符
    EQUALS,              // =
    PLUS,                // +
    MINUS,               // -
    STAR,                // *
    SLASH,               // /
    PERCENT,             // %
    POWER,               // **
    
    // 注释
    COMMENT_LINE,        // // ...
    COMMENT_BLOCK,       // /* ... */
    COMMENT_GENERATOR,   // # ...
};

// Token 结构
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType type = TokenType::UNKNOWN, 
          std::string value = "", 
          size_t line = 0, 
          size_t column = 0)
        : type(type), value(std::move(value)), line(line), column(column) {}
    
    // 便于调试
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

// 辅助函数：将 TokenType 转换为字符串
const char* tokenTypeToString(TokenType type);

} // namespace CHTL
