#include "Lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE, "");

    char c = advance();

    if (c == '/' && peek() == '/') {
        return singleLineComment();
    }
    if (c == '/' && peek() == '*') {
        return multiLineComment();
    }
    if (c == '#') {
        if (peek() == ' ') {
            return generatorComment();
        } else {
            return makeToken(TokenType::HASH, "#");
        }
    }
    if (c == '"' || c == '\'') {
        return stringLiteral(c);
    }
    if (isalpha(c) || c == '_') {
        // Backtrack to include the first character
        current--;
        return identifier();
    }
    if (isdigit(c)) {
        // Backtrack to include the first character
        current--;
        return number();
    }

    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case ':': return makeToken(TokenType::COLON, ":");
        case '=': return makeToken(TokenType::EQUALS, "=");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case '.': return makeToken(TokenType::DOT, ".");
        case '&': return makeToken(TokenType::AMPERSAND, "&");
        case '+': return makeToken(TokenType::PLUS, "+");
        case '-': return makeToken(TokenType::MINUS, "-");
        case '/': return makeToken(TokenType::SLASH, "/");
        case '%': return makeToken(TokenType::PERCENT, "%");
        case '*':
            if (peek() == '*') {
                advance();
                return makeToken(TokenType::DOUBLE_STAR, "**");
            }
            return makeToken(TokenType::STAR, "*");
        default:
            return makeToken(TokenType::UNKNOWN, std::string(1, c));
    }
}

char Lexer::advance() {
    if (!isAtEnd()) {
        column++;
        return source[current++];
    }
    return '\0';
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                line++;
                column = 1;
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, line, column - (int)value.length()};
}

Token Lexer::identifier() {
    size_t start = current;
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string value = source.substr(start, current - start);
    if (value == "text") {
        return makeToken(TokenType::TEXT_KEYWORD, value);
    }
    if (value == "style") {
        return makeToken(TokenType::STYLE_KEYWORD, value);
    }
    return makeToken(TokenType::IDENTIFIER, value);
}

Token Lexer::stringLiteral(char quoteType) {
    size_t start = current - 1; // Include the opening quote
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string
        return makeToken(TokenType::UNKNOWN, source.substr(start, current - start));
    }

    advance(); // Consume the closing quote
    return makeToken(TokenType::STRING_LITERAL, source.substr(start, current - start));
}


Token Lexer::singleLineComment() {
    size_t start = current - 1; // Include the first '/'
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    return makeToken(TokenType::SINGLE_LINE_COMMENT, source.substr(start, current - start));
}

Token Lexer::multiLineComment() {
    size_t start = current - 1; // Include the first '/'
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated comment
        return makeToken(TokenType::UNKNOWN, source.substr(start, current - start));
    }

    advance(); // Consume '*'
    advance(); // Consume '/'
    return makeToken(TokenType::MULTI_LINE_COMMENT, source.substr(start, current - start));
}

Token Lexer::generatorComment() {
    size_t start = current - 1; // Include the '#'
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    return makeToken(TokenType::GENERATOR_COMMENT, source.substr(start, current - start));
}

Token Lexer::number() {
    size_t start = current;
    while (isdigit(peek())) {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) {
            advance();
        }
    }

    return makeToken(TokenType::NUMERIC_LITERAL, source.substr(start, current - start));
}