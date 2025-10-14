#pragma once

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

// 解析错误异常
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, size_t line, size_t column)
        : std::runtime_error(message)
        , line_(line)
        , column_(column) {}
    
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }

private:
    size_t line_;
    size_t column_;
};

// 语法分析器
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    
    // 解析入口 - 返回 AST 根节点列表
    std::vector<NodePtr> parse();
    
    // 检查是否有错误
    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<std::string>& getErrors() const { return errors_; }

private:
    std::vector<Token> tokens_;
    size_t current_;
    std::vector<std::string> errors_;
    
    // Token 操作
    Token peek() const;
    Token previous() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    
    // 解析函数
    NodePtr parseStatement();
    NodePtr parseElement();
    NodePtr parseTextBlock();
    NodePtr parseStyleBlock();
    NodePtr parseScriptBlock();
    
    // 辅助函数
    void synchronize();
    void reportError(const std::string& message);
    bool isElementStart() const;
};

} // namespace CHTL
