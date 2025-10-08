#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    ILLEGAL,
    END_OF_FILE,

    // Identifiers & Literals
    IDENTIFIER,         // main, div, color
    STRING_LITERAL,     // "hello world", 'hello world'
    NUMERIC_LITERAL,    // 100, 3.14
    UNQUOTED_LITERAL,   // This is some text

    // Operators
    COLON,              // :
    EQUAL,              // =
    PLUS,               // +
    MINUS,              // -
    ASTERISK,           // *
    SLASH,              // /
    PERCENT,            // %
    POWER,              // **
    GREATER_THAN,       // >
    LESS_THAN,          // <
    GREATER_EQUAL,      // >=
    LESS_EQUAL,         // <=
    EQUAL_EQUAL,        // ==
    NOT_EQUAL,          // !=
    LOGICAL_AND,        // &&
    LOGICAL_OR,         // ||
    QUESTION_MARK,      // ?
    ARROW,              // ->
    EVENT_BIND_OP,      // &->

    // Punctuation
    L_BRACE,            // {
    R_BRACE,            // }
    L_PAREN,            // (
    R_PAREN,            // )
    L_SQ_BRACKET,       // [
    R_SQ_BRACKET,       // ]
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    HASH,               // #
    AMPERSAND,          // &
    DOLLAR,             // $
    AT_SIGN,            // @

    // Comments
    SINGLE_LINE_COMMENT, // //
    MULTI_LINE_COMMENT, // /* ... */

    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT,
    KEYWORD_TOP,
    KEYWORD_BOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_USE,
    KEYWORD_HTML5,

    // Block Keywords
    KEYWORD_CUSTOM,         // [Custom]
    KEYWORD_TEMPLATE,       // [Template]
    KEYWORD_ORIGIN,         // [Origin]
    KEYWORD_IMPORT,         // [Import]
    KEYWORD_NAMESPACE,      // [Namespace]
    KEYWORD_CONFIGURATION,  // [Configuration]
    KEYWORD_INFO,           // [Info]
    KEYWORD_EXPORT,         // [Export]

    // Type Keywords
    TYPE_STYLE,         // @Style
    TYPE_ELEMENT,       // @Element
    TYPE_VAR,           // @Var
    TYPE_HTML,          // @Html
    TYPE_JAVASCRIPT,    // @JavaScript
    TYPE_CHTL,          // @Chtl
    TYPE_CJMOD,         // @CJmod
    TYPE_CONFIG,        // @Config
};

// Function to convert TokenType to string for debugging
std::string tokenTypeToString(TokenType type);

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int column;

    Token(TokenType type, std::string literal, int line, int column)
        : type(type), literal(std::move(literal)), line(line), column(column) {}
};

} // namespace CHTL

#endif // CHTL_TOKEN_H