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
        // Skip unknown tokens for now, can be used for error reporting later
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

    if (isalpha(c)) return identifier();

    switch (c) {
        case '{': return {TokenType::OpenBrace, "{", m_line, m_column};
        case '}': return {TokenType::CloseBrace, "}", m_line, m_column};
        case ';': return {TokenType::Semicolon, ";", m_line, m_column};
        case ':': return {TokenType::Colon, ":", m_line, m_column};
        case '=': return {TokenType::Equals, "=", m_line, m_column};
        case '"': return string('"');
        case '\'': return string('\'');
        case '#':
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            // We capture the comment content, excluding the '#'
            return {TokenType::GeneratorComment, m_source.substr(m_start + 1, m_current - m_start - 1), m_line, m_column};
    }

    // If it's none of the above, it's treated as an UnquotedLiteral
    // This will catch values like `red`, `100px`, `HelloWorld`, etc.
    // The parser will be responsible for interpreting them in context.
    const std::string terminators = " \t\r\n{};:='\"#";
    while (!isAtEnd() && terminators.find(peek()) == std::string::npos) {
        advance();
    }
    return {TokenType::UnquotedLiteral, m_source.substr(m_start, m_current - m_start), m_line, m_column};
}

char Lexer::advance() {
    if (!isAtEnd()) {
        m_current++;
        m_column++;
    }
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
                if (m_current + 1 < m_source.length()) {
                    if (m_source[m_current + 1] == '/') { // Single-line comment
                        while (peek() != '\n' && !isAtEnd()) advance();
                    } else if (m_source[m_current + 1] == '*') { // Multi-line comment
                        advance(); // consume /
                        advance(); // consume *
                        while (!isAtEnd()) {
                            if (peek() == '*' && m_current + 1 < m_source.length() && m_source[m_current + 1] == '/') {
                                break;
                            }
                            if (peek() == '\n') {
                                m_line++;
                                m_column = 0;
                            }
                            advance();
                        }
                        if (!isAtEnd()) {
                            advance(); // consume *
                            advance(); // consume /
                        }
                    } else {
                        return; // Not a comment, just a slash
                    }
                } else {
                     return; // End of file
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::string(char delimiter) {
    while (peek() != delimiter && !isAtEnd()) {
        if (peek() == '\n') {
            m_line++;
            m_column = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string.");
    }

    advance(); // The closing delimiter.

    std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
    return {TokenType::String, value, m_line, m_column};
}

Token Lexer::identifier() {
    while (isalnum(peek()) || peek() == '-') { // Allow hyphens in identifiers
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
