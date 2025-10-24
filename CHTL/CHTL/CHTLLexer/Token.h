#pragma once

#include <string>
#include <vector>

enum class TokenType {
    Identifier,
    String,
    Number,
    OpenBrace,
    CloseBrace,
    Colon,
    Semicolon,
    Equals,
    Ampersand,
    Plus,
    Minus,
    Star,
    Slash,
    Comment,
    MultiLineComment,
    GeneratorComment,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};
