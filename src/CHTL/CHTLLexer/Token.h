#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <ostream>

namespace CHTL {

/**
 * Token类型枚举
 * 定义CHTL语言中所有的词法单元类型
 */
enum class TokenType {
    // 文件结束
    END_OF_FILE,
    
    // 标识符和字面量
    IDENTIFIER,          // 标识符（元素名、属性名等）
    STRING_LITERAL,      // 字符串字面量 "..." 或 '...'
    UNQUOTED_LITERAL,    // 无修饰字面量
    NUMBER_LITERAL,      // 数字字面量
    
    // 关键字 - 基础块
    KEYWORD_TEXT,        // text
    KEYWORD_STYLE,       // style
    KEYWORD_SCRIPT,      // script
    
    // 关键字 - 模板和自定义
    KEYWORD_TEMPLATE,    // [Template]
    KEYWORD_CUSTOM,      // [Custom]
    KEYWORD_ORIGIN,      // [Origin]
    KEYWORD_IMPORT,      // [Import]
    KEYWORD_NAMESPACE,   // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    
    // 关键字 - 类型标识
    KEYWORD_AT_STYLE,    // @Style
    KEYWORD_AT_ELEMENT,  // @Element
    KEYWORD_AT_VAR,      // @Var
    KEYWORD_AT_HTML,     // @Html
    KEYWORD_AT_JAVASCRIPT, // @JavaScript
    KEYWORD_AT_CHTL,     // @Chtl
    KEYWORD_AT_CONFIG,   // @Config
    KEYWORD_AT_CJMOD,    // @CJmod
    
    // 关键字 - 操作符
    KEYWORD_INHERIT,     // inherit
    KEYWORD_DELETE,      // delete
    KEYWORD_INSERT,      // insert
    KEYWORD_AFTER,       // after
    KEYWORD_BEFORE,      // before
    KEYWORD_REPLACE,     // replace
    KEYWORD_AT_TOP,      // at top
    KEYWORD_AT_BOTTOM,   // at bottom
    KEYWORD_FROM,        // from
    KEYWORD_AS,          // as
    KEYWORD_EXCEPT,      // except
    KEYWORD_USE,         // use
    KEYWORD_HTML5,       // html5
    
    // 符号
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    SEMICOLON,           // ;
    COLON,               // :
    COMMA,               // ,
    DOT,                 // .
    HASH,                // #
    AT,                  // @
    AMPERSAND,           // &
    
    // 运算符
    EQUAL,               // =
    PLUS,                // +
    MINUS,               // -
    STAR,                // *
    SLASH,               // /
    PERCENT,             // %
    DOUBLE_STAR,         // **
    
    // 比较运算符
    GREATER,             // >
    LESS,                // <
    GREATER_EQUAL,       // >=
    LESS_EQUAL,          // <=
    EQUAL_EQUAL,         // ==
    NOT_EQUAL,           // !=
    
    // 逻辑运算符
    AND,                 // &&
    OR,                  // ||
    
    // 特殊符号
    QUESTION,            // ?
    ARROW,               // ->
    
    // 注释
    COMMENT_LINE,        // // 注释
    COMMENT_BLOCK,       // /* */ 注释
    COMMENT_GENERATOR,   // # 生成器注释
    
    // 错误和未知
    UNKNOWN,
    INVALID
};

/**
 * 源码位置信息
 */
struct SourceLocation {
    std::string filename;  // 文件名
    size_t line;          // 行号（从1开始）
    size_t column;        // 列号（从1开始）
    
    SourceLocation(const std::string& file = "", size_t ln = 1, size_t col = 1)
        : filename(file), line(ln), column(col) {}
};

/**
 * Token类
 * 表示词法分析产生的一个词法单元
 */
class Token {
public:
    Token(TokenType type, const std::string& lexeme, const SourceLocation& location)
        : type_(type), lexeme_(lexeme), location_(location) {}
    
    Token() : type_(TokenType::INVALID), lexeme_(""), location_() {}
    
    // Getters
    TokenType getType() const { return type_; }
    const std::string& getLexeme() const { return lexeme_; }
    const SourceLocation& getLocation() const { return location_; }
    
    // 判断是否为特定类型
    bool isType(TokenType type) const { return type_ == type; }
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    
    // 转换为字符串（用于调试）
    std::string toString() const;
    
    // 获取类型名称
    static std::string getTypeName(TokenType type);
    
private:
    TokenType type_;
    std::string lexeme_;
    SourceLocation location_;
};

// 输出运算符
std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace CHTL

#endif // CHTL_TOKEN_H
