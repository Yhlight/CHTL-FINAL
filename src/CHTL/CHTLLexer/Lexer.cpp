#include "Lexer.h"
#include "CHTL/CHTLCommon/CompilerError.h"
#include "CHTL/CHTLCommon/CompilerConfig.h"
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
    
    // 标识符或关键字（包括 @ 前缀）
    if (isAlpha(c) || c == '_' || c == '@') {
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
        case '[':
            // 检查是否是 CHTL 特殊语法块 [Template], [Custom] 等
            if (isAlpha(peek())) {
                current_--;
                column_--;
                return scanBracketKeyword();
            }
            return makeToken(TokenType::LEFT_BRACKET, "[");
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
    value.reserve(CompilerConfig::STRING_RESERVE_SIZE);
    size_t startLine = line_;
    size_t startColumn = column_;
    
    while (!isAtEnd() && peek() != quote) {
        // 检查字符串长度限制
        if (value.length() >= CompilerConfig::MAX_LINE_LENGTH) {
            throw LexerError(
                "字符串字面量过长",
                line_,
                startColumn,
                "字符串超过最大长度限制 " + std::to_string(CompilerConfig::MAX_LINE_LENGTH)
            );
        }
        
        if (peek() == '\\') {
            advance(); // 跳过反斜杠
            if (!isAtEnd()) {
                char escaped = advance();
                // 处理转义字符
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        // 未闭合的字符串 - 抛出异常
        throw LexerError(
            "未闭合的字符串字面量",
            startLine,
            startColumn,
            "字符串缺少结束引号 " + std::string(1, quote)
        );
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
    
    // 检查是否是 @ 前缀
    if (peek() == '@') {
        advance(); // 消费 @
    }
    
    while (isAlphaNumeric(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    
    std::string value = source_.substr(start, current_ - start);
    TokenType type = identifierType(value);
    
    return Token(type, value, line_, startColumn);
}

Token Lexer::scanBracketKeyword() {
    size_t start = current_;
    size_t startColumn = column_;
    
    // 消费 '['
    advance();
    
    // 扫描关键字名称
    while (isAlphaNumeric(peek())) {
        advance();
    }
    
    // 检查是否有 ']'
    if (peek() == ']') {
        advance(); // 消费 ']'
    } else {
        // 不是有效的关键字，返回 UNKNOWN
        return makeToken(TokenType::UNKNOWN, source_.substr(start, current_ - start));
    }
    
    std::string value = source_.substr(start, current_ - start);
    
    // 识别特殊语法块关键字
    if (value == "[Template]") return Token(TokenType::KEYWORD_TEMPLATE, value, line_, startColumn);
    if (value == "[Custom]") return Token(TokenType::KEYWORD_CUSTOM, value, line_, startColumn);
    if (value == "[Import]") return Token(TokenType::KEYWORD_IMPORT, value, line_, startColumn);
    if (value == "[Origin]") return Token(TokenType::KEYWORD_ORIGIN, value, line_, startColumn);
    if (value == "[Namespace]") return Token(TokenType::KEYWORD_NAMESPACE, value, line_, startColumn);
    if (value == "[Configuration]") return Token(TokenType::KEYWORD_CONFIGURATION, value, line_, startColumn);
    if (value == "[Info]") return Token(TokenType::KEYWORD_INFO, value, line_, startColumn);
    if (value == "[Export]") return Token(TokenType::KEYWORD_EXPORT, value, line_, startColumn);
    
    // 不是已知的关键字，返回为 UNKNOWN
    return Token(TokenType::UNKNOWN, value, line_, startColumn);
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
    // 检查基础关键字
    if (text == "text") return TokenType::KEYWORD_TEXT;
    if (text == "style") return TokenType::KEYWORD_STYLE;
    if (text == "script") return TokenType::KEYWORD_SCRIPT;
    
    // 检查 @ 前缀关键字
    if (text == "@Style") return TokenType::KEYWORD_AT_STYLE;
    if (text == "@Element") return TokenType::KEYWORD_AT_ELEMENT;
    if (text == "@Var") return TokenType::KEYWORD_AT_VAR;
    if (text == "@Html") return TokenType::KEYWORD_AT_HTML;
    if (text == "@JavaScript") return TokenType::KEYWORD_AT_JAVASCRIPT;
    if (text == "@Chtl") return TokenType::KEYWORD_AT_CHTL;
    if (text == "@Config") return TokenType::KEYWORD_AT_CONFIG;
    if (text == "@CJmod") return TokenType::KEYWORD_AT_CJMOD;
    
    // 检查其他关键字
    if (text == "from") return TokenType::KEYWORD_FROM;
    if (text == "as") return TokenType::KEYWORD_AS;
    if (text == "inherit") return TokenType::KEYWORD_INHERIT;
    if (text == "delete") return TokenType::KEYWORD_DELETE;
    if (text == "insert") return TokenType::KEYWORD_INSERT;
    if (text == "use") return TokenType::KEYWORD_USE;
    if (text == "except") return TokenType::KEYWORD_EXCEPT;
    if (text == "after") return TokenType::KEYWORD_AFTER;
    if (text == "before") return TokenType::KEYWORD_BEFORE;
    if (text == "replace") return TokenType::KEYWORD_REPLACE;
    
    return TokenType::IDENTIFIER;
}

} // namespace CHTL
