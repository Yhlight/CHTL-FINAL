#pragma once

#include <string>

namespace CHTL {

    enum class TokenType {
        // Special
        ILLEGAL,
        END_OF_FILE,

        // Identifiers + literals
        IDENT,          // add, foobar, x, y, ...
        NUMBER,         // 12345
        STRING,         // "foobar"

        // Operators
        ASSIGN,         // =
        PLUS,           // +
        MINUS,          // -
        BANG,           // !
        ASTERISK,       // *
        SLASH,          // /
        POWER,          // **
        MODULO,         // %

        LT,             // <
        GT,             // >

        EQ,             // ==
        NOT_EQ,         // !=

        // Delimiters
        COMMA,          // ,
        SEMICOLON,      // ;
        COLON,          // :

        LPAREN,         // (
        RPAREN,         // )
        LBRACE,         // {
        RBRACE,         // }
        LBRACKET,       // [
        RBRACKET,       // ]
        DOT,            // .
        HASH,           // #
        AMPERSAND,      // &
        QUESTION,       // ?
        AND,            // &&
        OR,             // ||

        // Keywords
        TEXT,
        STYLE,
        SCRIPT,
        IF,
        ELSE,
        TEMPLATE,
        ELEMENT,
        VAR,
        INHERIT,
        CUSTOM,
        DELETE,
        INSERT,
        AFTER,
        BEFORE,
        REPLACE,
        AT_TOP,
        AT_BOTTOM,
        ORIGIN,
        HTML,
        JAVASCRIPT,
        CHTL,
        AS,
        FROM,
        NAMESPACE,
        EXCEPT,
        CONFIGURATION,
        CONFIG,
        USE,
        HTML5,
    };

    struct Token {
        TokenType type;
        std::string literal;
    };

} // namespace CHTL
