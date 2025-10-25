#include "CHTLLexer.h"
#include <iostream>

CHTLLexer::CHTLLexer(const std::string& filepath) : file(filepath), lookahead(nullptr) {
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        exit(1);
    }
    advance();
}

void CHTLLexer::advance() {
    file.get(currentChar);
}

void CHTLLexer::skipWhitespace() {
    while (!file.eof() && isspace(currentChar)) {
        advance();
    }
}

Token CHTLLexer::parseIdentifier() {
    std::string value;
    while (!file.eof() && (isalnum(currentChar) || currentChar == '_' || currentChar == '-' || currentChar == '.')) {
        value += currentChar;
        advance();
    }
    return Token(TokenType::IDENTIFIER, value);
}

Token CHTLLexer::parseString() {
    std::string value;
    advance(); // Consume the opening quote
    while (!file.eof() && currentChar != '"') {
        value += currentChar;
        advance();
    }
    advance(); // Consume the closing quote
    return Token(TokenType::STRING, value);
}

Token CHTLLexer::getNextToken() {
    if (lookahead) {
        Token token = *lookahead;
        lookahead.reset();
        return token;
    }
    return getNextTokenInternal();
}

Token CHTLLexer::peekToken() {
    if (!lookahead) {
        lookahead = std::make_unique<Token>(getNextTokenInternal());
    }
    return *lookahead;
}

Token CHTLLexer::getNextTokenInternal() {
    while (!file.eof()) {
        skipWhitespace();

        if (isalnum(currentChar) || currentChar == '_') {
            return parseIdentifier();
        }

        if (currentChar == '"') {
            return parseString();
        }

        switch (currentChar) {
            case '{': advance(); return Token(TokenType::LBRACE, "{");
            case '}': advance(); return Token(TokenType::RBRACE, "}");
            case '[': advance(); return Token(TokenType::LBRACKET, "[");
            case ']': advance(); return Token(TokenType::RBRACKET, "]");
            case '@': advance(); return Token(TokenType::AT, "@");
            case ';': advance(); return Token(TokenType::SEMICOLON, ";");
            case ':': advance(); return Token(TokenType::COLON, ":");
            case '=': advance(); return Token(TokenType::EQUALS, "=");
            default:
                if (!file.eof()) {
                    char unknownChar = currentChar;
                    advance();
                    return Token(TokenType::UNKNOWN, std::string(1, unknownChar));
                }
        }
    }
    return Token(TokenType::END_OF_FILE, "");
}
