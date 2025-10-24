#include "Lexer.h"
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        tokens.push_back(nextToken());
    }
    tokens.push_back({TokenType::EndOfFile, "", line, start});
    return tokens;
}

Token Lexer::nextToken() {
    start = current;
    char c = advance();
    switch (c) {
        case '{': return {TokenType::OpenBrace, "{", line, start};
        case '}': return {TokenType::CloseBrace, "}", line, start};
        case ':': return {TokenType::Colon, ":", line, start};
        case ';': return {TokenType::Semicolon, ";", line, start};
        case '/':
            if (peek() == '/') {
                while (peek() != '\n' && !isAtEnd()) advance();
                return {TokenType::Comment, source.substr(start, current - start), line, start};
            }
            break;
        case '"':
            while (peek() != '"' && !isAtEnd()) {
                if (peek() == '\n') line++;
                advance();
            }
            advance();
            return {TokenType::String, source.substr(start + 1, current - start - 2), line, start};
        case '\n':
            line++;
        case ' ':
        case '\r':
        case '\t':
            return nextToken();
    }

    if (isalpha(c)) {
        while (isalnum(peek())) advance();
        return {TokenType::Identifier, source.substr(start, current - start), line, start};
    }

    return {TokenType::Unknown, std::string(1, c), line, start};
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::advance() {
    return source[current++];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}
