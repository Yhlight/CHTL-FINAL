#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLCommon/CompilerConfig.h"
#include "CHTL/CHTLCommon/CompilerError.h"
#include "CHTL/CHTLCommon/Validation.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

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
    size_t currentDepth_;  // 当前嵌套深度
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
