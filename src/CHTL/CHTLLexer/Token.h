#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <utility>

enum class TokenType {
    IDENTIFIER,
    STRING,
    NUMBER,
    LBRACE,     // {
    RBRACE,     // }
    LBRACKET,   // [
    RBRACKET,   // ]
    LPAREN,     // (
    RPAREN,     // )
    AT,         // @
    SEMICOLON,  // ;
    COLON,      // :
    EQUALS,     // =
    IMPORT,     // import
    AS,         // as
    FROM,       // from
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
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::LBRACE: return "LBRACE";
            case TokenType::RBRACE: return "RBRACE";
            case TokenType::LBRACKET: return "LBRACKET";
            case TokenType::RBRACKET: return "RBRACKET";
            case TokenType::LPAREN: return "LPAREN";
            case TokenType::RPAREN: return "RPAREN";
            case TokenType::AT: return "AT";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::COLON: return "COLON";
            case TokenType::EQUALS: return "EQUALS";
            case TokenType::IMPORT: return "IMPORT";
            case TokenType::AS: return "AS";
            case TokenType::FROM: return "FROM";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
            case TokenType::UNKNOWN: return "UNKNOWN";
        }
        return "UNKNOWN";
    }
};

#endif //CHTL_TOKEN_H
