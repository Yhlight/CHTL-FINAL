#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    TOKEN_TEXT,

    // Symbols
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_EQUAL,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_DOUBLE_ASTERISK,
    TOKEN_HASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_BANG,
    TOKEN_QUESTION,
    TOKEN_AND,
    TOKEN_OR,

    // Literals
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_SINGLE_QUOTE_STRING,

    // Comments
    TOKEN_COMMENT,

    // End of file
    TOKEN_EOF,

    // Illegal
    TOKEN_ILLEGAL
};

struct Token {
    TokenType type;
    std::string literal;

    bool operator==(const Token& other) const {
        return type == other.type && literal == other.literal;
    }
};