#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source, const std::string& filename)
    : source_(source)
    , filename_(filename)
    , current_(0)
    , line_(1)
    , column_(1)
    , lineStart_(0)
    , hasPeeked_(false)
    , peekedToken_()
{
}

Token Lexer::nextToken() {
    if (hasPeeked_) {
        hasPeeked_ = false;
        return peekedToken_;
    }
    return scanToken();
}

Token Lexer::peekToken() {
    if (!hasPeeked_) {
        peekedToken_ = scanToken();
        hasPeeked_ = true;
    }
    return peekedToken_;
}

SourceLocation Lexer::getCurrentLocation() const {
    return SourceLocation(filename_, line_, column_);
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = source_[current_++];
    
    if (c == '\n') {
        line_++;
        lineStart_ = current_;
        column_ = 1;
    } else {
        column_++;
    }
    
    return c;
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
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
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::skipLineComment() {
    // 跳过 // 直到行尾
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::skipBlockComment() {
    // 跳过 /* ... */
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance(); // *
            advance(); // /
            break;
        }
        advance();
    }
}

Token Lexer::scanGeneratorComment() {
    SourceLocation loc = getCurrentLocation();
    
    // 先检查是否是生成器注释（# + 空格）
    if (peek() == '#' && peekNext() == ' ') {
        // 这是生成器注释
        advance(); // 跳过 #
        advance(); // 跳过空格
        
        // 读取注释内容直到行尾
        size_t start = current_;
        while (peek() != '\n' && !isAtEnd()) {
            advance();
        }
        
        std::string comment = "# " + source_.substr(start, current_ - start);
        return Token(TokenType::COMMENT_GENERATOR, comment, loc);
    } else {
        // 只是普通的 # 符号
        advance();
        return Token(TokenType::HASH, "#", loc);
    }
}

Token Lexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", getCurrentLocation());
    }
    
    SourceLocation loc = getCurrentLocation();
    char c = advance();
    
    // 标识符和关键字
    if (isAlpha(c)) {
        current_--; // 回退
        column_--;
        return scanIdentifierOrKeyword();
    }
    
    // 数字
    if (isDigit(c)) {
        current_--;
        column_--;
        return scanNumber();
    }
    
    // 符号和运算符
    switch (c) {
        case '{': return Token(TokenType::LEFT_BRACE, "{", loc);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", loc);
        case '[':
            // 检查是否是关键字 [Template], [Custom] 等
            {
                size_t saved = current_;
                size_t savedLine = line_;
                size_t savedCol = column_;
                
                std::string bracket = "[";
                while (!isAtEnd() && peek() != ']') {
                    bracket += advance();
                }
                if (peek() == ']') {
                    bracket += advance();
                    
                    // 检查是否是关键字
                    TokenType type = globalMap_.lookup(bracket);
                    if (type != TokenType::IDENTIFIER) {
                        return Token(type, bracket, loc);
                    }
                }
                
                // 不是关键字，回退
                current_ = saved;
                line_ = savedLine;
                column_ = savedCol;
                return Token(TokenType::LEFT_BRACKET, "[", loc);
            }
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", loc);
        case '(': return Token(TokenType::LEFT_PAREN, "(", loc);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", loc);
        case ';': return Token(TokenType::SEMICOLON, ";", loc);
        case ',': return Token(TokenType::COMMA, ",", loc);
        case '.': return Token(TokenType::DOT, ".", loc);
        case '@':
            // 检查是否是 @Style, @Element 等
            {
                size_t saved = current_;
                size_t savedLine = line_;
                size_t savedCol = column_;
                
                std::string at = "@";
                while (!isAtEnd() && isAlpha(peek())) {
                    at += advance();
                }
                
                // 检查是否是关键字
                TokenType type = globalMap_.lookup(at);
                if (type != TokenType::IDENTIFIER) {
                    return Token(type, at, loc);
                }
                
                // 不是关键字，回退
                current_ = saved;
                line_ = savedLine;
                column_ = savedCol;
                return Token(TokenType::AT, "@", loc);
            }
        case '&':
            if (match('&')) {
                return Token(TokenType::AND, "&&", loc);
            }
            return Token(TokenType::AMPERSAND, "&", loc);
        case ':': return Token(TokenType::COLON, ":", loc);
        case '?': return Token(TokenType::QUESTION, "?", loc);
        case '+': return Token(TokenType::PLUS, "+", loc);
        case '%': return Token(TokenType::PERCENT, "%", loc);
        case '#':
            // 可能是生成器注释，回退一个字符让scanGeneratorComment重新处理
            current_--;
            column_--;
            return scanGeneratorComment();
        case '-':
            if (match('>')) {
                return Token(TokenType::ARROW, "->", loc);
            }
            return Token(TokenType::MINUS, "-", loc);
        case '*':
            if (match('*')) {
                return Token(TokenType::DOUBLE_STAR, "**", loc);
            }
            return Token(TokenType::STAR, "*", loc);
        case '/':
            if (match('/')) {
                skipLineComment();
                return scanToken(); // 递归获取下一个token
            }
            if (match('*')) {
                skipBlockComment();
                return scanToken(); // 递归获取下一个token
            }
            return Token(TokenType::SLASH, "/", loc);
        case '=':
            if (match('=')) {
                return Token(TokenType::EQUAL_EQUAL, "==", loc);
            }
            return Token(TokenType::EQUAL, "=", loc);
        case '!':
            if (match('=')) {
                return Token(TokenType::NOT_EQUAL, "!=", loc);
            }
            return Token(TokenType::UNKNOWN, "!", loc);
        case '>':
            if (match('=')) {
                return Token(TokenType::GREATER_EQUAL, ">=", loc);
            }
            return Token(TokenType::GREATER, ">", loc);
        case '<':
            if (match('=')) {
                return Token(TokenType::LESS_EQUAL, "<=", loc);
            }
            return Token(TokenType::LESS, "<", loc);
        case '|':
            if (match('|')) {
                return Token(TokenType::OR, "||", loc);
            }
            return Token(TokenType::UNKNOWN, "|", loc);
        case '"':
        case '\'':
            current_--;
            column_--;
            return scanString(c);
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), loc);
    }
}

Token Lexer::scanIdentifierOrKeyword() {
    SourceLocation loc = getCurrentLocation();
    size_t start = current_;
    
    // 第一个字符必须是字母或下划线
    if (isAlpha(peek())) {
        advance();
    }
    
    // 后续字符可以是字母、数字或下划线
    while (isAlphaNumeric(peek()) || peek() == '_') {
        advance();
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    
    // 检查是否是关键字
    TokenType type = globalMap_.lookup(lexeme);
    
    return Token(type, lexeme, loc);
}

Token Lexer::scanNumber() {
    SourceLocation loc = getCurrentLocation();
    size_t start = current_;
    
    // 整数部分
    while (isDigit(peek())) {
        advance();
    }
    
    // 小数部分
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // 消耗 .
        while (isDigit(peek())) {
            advance();
        }
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    return Token(TokenType::NUMBER_LITERAL, lexeme, loc);
}

Token Lexer::scanString(char quote) {
    SourceLocation loc = getCurrentLocation();
    size_t start = current_;
    
    advance(); // 消耗开始引号
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\\') {
            advance(); // 转义字符
            if (!isAtEnd()) advance();
        } else {
            advance();
        }
    }
    
    if (isAtEnd()) {
        // 未闭合的字符串
        return Token(TokenType::INVALID, source_.substr(start, current_ - start), loc);
    }
    
    advance(); // 消耗结束引号
    
    std::string lexeme = source_.substr(start, current_ - start);
    return Token(TokenType::STRING_LITERAL, lexeme, loc);
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

} // namespace CHTL
