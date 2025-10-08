#include "Lexer.h"
#include <cctype>
#include <unordered_map>

static std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::TOKEN_TEXT}
};

Lexer::Lexer(const std::string& input) : input(input), position(0), readPosition(0), ch(0) {
    readChar();
}

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0; // EOF
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition += 1;
}

char Lexer::peekChar() {
    if (readPosition >= input.length()) {
        return 0;
    }
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while (isspace(ch)) {
        readChar();
    }
}

std::string Lexer::readIdentifier() {
    int startPosition = position;
    while (isalpha(ch) || ch == '_') {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readString() {
    readChar(); // consume opening "
    int startPosition = position;
    while (ch != '"' && ch != 0) {
        readChar();
    }
    std::string result = input.substr(startPosition, position - startPosition);
    readChar(); // consume closing "
    return result;
}

Token Lexer::NextToken() {
    Token tok;

    skipWhitespace();

    switch (ch) {
        case '{':
            tok = {TokenType::TOKEN_LBRACE, "{"};
            break;
        case '}':
            tok = {TokenType::TOKEN_RBRACE, "}"};
            break;
        case ';':
            tok = {TokenType::TOKEN_SEMICOLON, ";"};
            break;
        case '"':
            tok.type = TokenType::TOKEN_STRING;
            tok.literal = readString();
            return tok; // readString already advanced the char
        case 0:
            tok = {TokenType::TOKEN_EOF, ""};
            break;
        default:
            if (isalpha(ch) || ch == '_') {
                tok.literal = readIdentifier();
                if (keywords.count(tok.literal)) {
                    tok.type = keywords[tok.literal];
                } else {
                    tok.type = TokenType::TOKEN_IDENTIFIER;
                }
                return tok; // readIdentifier already advanced the char
            } else {
                tok = {TokenType::TOKEN_ILLEGAL, std::string(1, ch)};
            }
            break;
    }

    readChar();
    return tok;
}