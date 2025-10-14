#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <string_view>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(std::string source);
    
    // 获取所有 tokens
    std::vector<Token> tokenize();
    
    // 获取下一个 token
    Token nextToken();
    
    // 检查是否到达文件末尾
    bool isAtEnd() const;

private:
    std::string source_;
    size_t current_;
    size_t line_;
    size_t column_;
    
    // 辅助函数
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    
    void skipWhitespace();
    Token makeToken(TokenType type, std::string value = "");
    
    // 扫描不同类型的 token
    Token scanStringLiteral(char quote);
    Token scanNumber();
    Token scanIdentifierOrKeyword();
    Token scanBracketKeyword();  // 扫描 [Template], [Custom] 等
    Token scanUnquotedLiteral();
    Token scanComment();
    
    // 检查函数
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    TokenType identifierType(const std::string& text) const;
};

} // namespace CHTL
