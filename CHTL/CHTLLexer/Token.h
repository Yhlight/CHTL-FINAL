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

    // Literals
    TOKEN_IDENTIFIER,
    TOKEN_STRING,

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