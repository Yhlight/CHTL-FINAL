#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(std::string source)
    : source_(std::move(source))
    , current_(0)
    , line_(1)
    , column_(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        Token token = nextToken();
        // 跳过注释（如果需要保留注释，可以修改此处）
        if (token.type != TokenType::COMMENT_LINE && 
            token.type != TokenType::COMMENT_BLOCK) {
            tokens.push_back(token);
        }
    }
    tokens.push_back(makeToken(TokenType::END_OF_FILE));
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE);
    }
    
    char c = advance();
    
    // 数字
    if (isDigit(c)) {
        current_--; // 回退
        column_--;
        return scanNumber();
    }
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        current_--;
        column_--;
        return scanIdentifierOrKeyword();
    }
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        return scanStringLiteral(c);
    }
    
    // 注释
    if (c == '/') {
        if (peek() == '/' || peek() == '*') {
            current_--;
            column_--;
            return scanComment();
        }
        return makeToken(TokenType::SLASH, "/");
    }
    
    if (c == '#') {
        if (peek() == ' ') {
            current_--;
            column_--;
            return scanComment();
        }
    }
    
    // 单字符 tokens
    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case '[': return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': return makeToken(TokenType::RIGHT_BRACKET, "]");
        case '(': return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': return makeToken(TokenType::RIGHT_PAREN, ")");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case ':': return makeToken(TokenType::COLON, ":");
        case ',': return makeToken(TokenType::COMMA, ",");
        case '.': return makeToken(TokenType::DOT, ".");
        case '=': return makeToken(TokenType::EQUALS, "=");
        case '+': return makeToken(TokenType::PLUS, "+");
        case '-': return makeToken(TokenType::MINUS, "-");
        case '%': return makeToken(TokenType::PERCENT, "%");
        case '*':
            if (peek() == '*') {
                advance();
                return makeToken(TokenType::POWER, "**");
            }
            return makeToken(TokenType::STAR, "*");
    }
    
    return makeToken(TokenType::UNKNOWN, std::string(1, c));
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source_[current_++];
    column_++;
    if (c == '\n') {
        line_++;
        column_ = 1;
    }
    return c;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    advance();
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

Token Lexer::makeToken(TokenType type, std::string value) {
    return Token(type, std::move(value), line_, column_ - value.length());
}

Token Lexer::scanStringLiteral(char quote) {
    std::string value;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\') {
            advance(); // 跳过反斜杠
            if (!isAtEnd()) {
                value += advance();
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        // 未闭合的字符串
        return Token(TokenType::UNKNOWN, value, startLine, startColumn);
    }
    
    advance(); // 消费结束引号
    return Token(TokenType::STRING_LITERAL, value, startLine, startColumn);
}

Token Lexer::scanNumber() {
    size_t start = current_;
    size_t startColumn = column_;
    
    while (isDigit(peek())) {
        advance();
    }
    
    // 小数
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // 消费 '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    std::string value = source_.substr(start, current_ - start);
    return Token(TokenType::NUMBER, value, line_, startColumn);
}

Token Lexer::scanIdentifierOrKeyword() {
    size_t start = current_;
    size_t startColumn = column_;
    
    while (isAlphaNumeric(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    
    std::string value = source_.substr(start, current_ - start);
    TokenType type = identifierType(value);
    
    return Token(type, value, line_, startColumn);
}

Token Lexer::scanUnquotedLiteral() {
    size_t start = current_;
    size_t startColumn = column_;
    
    while (!isAtEnd() && peek() != ';' && peek() != ',' && 
           peek() != '{' && peek() != '}' && !std::isspace(peek())) {
        advance();
    }
    
    std::string value = source_.substr(start, current_ - start);
    return Token(TokenType::UNQUOTED_LITERAL, value, line_, startColumn);
}

Token Lexer::scanComment() {
    size_t startLine = line_;
    size_t startColumn = column_;
    std::string value;
    
    if (peek() == '/' && peekNext() == '/') {
        // 单行注释
        advance(); advance(); // 跳过 //
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        return Token(TokenType::COMMENT_LINE, value, startLine, startColumn);
    }
    
    if (peek() == '/' && peekNext() == '*') {
        // 多行注释
        advance(); advance(); // 跳过 /*
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '/') {
                advance(); advance(); // 跳过 */
                break;
            }
            value += advance();
        }
        return Token(TokenType::COMMENT_BLOCK, value, startLine, startColumn);
    }
    
    if (peek() == '#' && peekNext() == ' ') {
        // 生成器注释
        advance(); advance(); // 跳过 '# '
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        return Token(TokenType::COMMENT_GENERATOR, value, startLine, startColumn);
    }
    
    return makeToken(TokenType::UNKNOWN);
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

TokenType Lexer::identifierType(const std::string& text) const {
    // 检查关键字
    if (text == "text") return TokenType::KEYWORD_TEXT;
    if (text == "style") return TokenType::KEYWORD_STYLE;
    if (text == "script") return TokenType::KEYWORD_SCRIPT;
    
    return TokenType::IDENTIFIER;
}

} // namespace CHTL
