#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <utility>

enum class TokenType {
    IDENTIFIER,
    STRING,
    LBRACE,     // {
    RBRACE,     // }
    SEMICOLON,  // ;
    COLON,      // :
    EQUALS,     // =
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}

    static std::string typeToString(TokenType t) {
        switch (t) {
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::STRING: return "STRING";
            case TokenType::LBRACE: return "LBRACE";
            case TokenType::RBRACE: return "RBRACE";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::COLON: return "COLON";
            case TokenType::EQUALS: return "EQUALS";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
            case TokenType::UNKNOWN: return "UNKNOWN";
        }
        return "UNKNOWN";
    }
};

#endif //CHTL_TOKEN_H
