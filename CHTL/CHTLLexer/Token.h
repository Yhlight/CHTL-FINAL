#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // CHTL
    TEXT,
    IDENTIFIER,

    // Symbols
    LEFT_BRACE,
    RIGHT_BRACE,
    COLON,
    EQUALS,
    SEMICOLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    DOT,
    AT,
    HASH,
    DOLLAR,

    // Keywords
    TEXT_KEYWORD,
    STYLE_KEYWORD,
    SCRIPT_KEYWORD,
    ELEMENT_KEYWORD,

    // Literals
    STRING_LITERAL,
    NUMERIC_LITERAL,

    // Comments
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    GENERATOR_COMMENT,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    DOUBLE_STAR,
    GREATER_THAN,
    LESS_THAN,
    QUESTION_MARK,
    AMPERSAND,
    PIPE,

    // Special
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};