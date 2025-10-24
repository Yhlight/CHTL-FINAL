#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // Keywords
    Text,
    Style,
    Script,

    // Punctuation
    OpenBrace,     // {
    CloseBrace,    // }
    Semicolon,     // ;
    Colon,         // :
    Equals,        // =

    // Literals
    String,
    Identifier,
    UnquotedLiteral,
    Number,

    // Other
    Comment,
    GeneratorComment, // #
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H
