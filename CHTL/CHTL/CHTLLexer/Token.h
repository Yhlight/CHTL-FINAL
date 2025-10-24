#pragma once

#include <string>
#include <vector>

enum class TokenType {
    Identifier,
    String,
    OpenBrace,
    CloseBrace,
    Colon,
    Semicolon,
    Equals,
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
