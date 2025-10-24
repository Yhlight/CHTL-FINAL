#include "Lexer.h"
#include <stdexcept>
#include <unordered_map>

namespace CHTL {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::Text},
    {"style", TokenType::Style},
    {"script", TokenType::Script},
};

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        m_start = m_current;
        Token token = scanToken();
        if (token.type != TokenType::Unknown) {
            tokens.push_back(token);
        }
    }
    tokens.push_back({TokenType::EndOfFile, "", m_line, m_column});
    return tokens;
}

Token Lexer::scanToken() {
    skipWhitespaceAndComments();
    if (isAtEnd()) return {TokenType::EndOfFile, "", m_line, m_column};

    m_start = m_current;
    char c = advance();

    switch (c) {
        case '{': return {TokenType::OpenBrace, "{", m_line, m_column};
        case '}': return {TokenType::CloseBrace, "}", m_line, m_column};
        case ';': return {TokenType::Semicolon, ";", m_line, m_column};
        case ':': return {TokenType::Colon, ":", m_line, m_column};
        case '=': return {TokenType::Equals, "=", m_line, m_column};
        case '"': return string();
    }

    if (isalpha(c)) {
        return identifier();
    }

    return {TokenType::Unknown, std::string(1, c), m_line, m_column};
}

char Lexer::advance() {
    m_current++;
    m_column++;
    return m_source[m_current - 1];
}

bool Lexer::isAtEnd() {
    return m_current >= m_source.length();
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return m_source[m_current];
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                m_line++;
                m_column = 0;
                advance();
                break;
            case '/':
                if (m_current + 1 < m_source.length() && m_source[m_current + 1] == '/') {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            m_line++;
            m_column = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string.");
    }

    advance(); // The closing ".

    std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
    return {TokenType::String, value, m_line, m_column};
}

Token Lexer::identifier() {
    while (isalnum(peek())) {
        advance();
    }

    std::string text = m_source.substr(m_start, m_current - m_start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return {it->second, text, m_line, m_column};
    }

    return {TokenType::Identifier, text, m_line, m_column};
}

} // namespace CHTL
