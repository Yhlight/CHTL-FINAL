#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include "CHTL/CHTLCommon/CompilerConfig.h"
#include "CHTL/CHTLCommon/CompilerError.h"
#include "CHTL/CHTLCommon/Validation.h"
#include "CHTL/CHTLCommon/GlobalStyleCollector.h"
#include "CHTL/CHTLParser/CSSExpression.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>

namespace CHTL {

// CSS 选择器类型
enum class SelectorType {
    CLASS,          // .box
    ID,             // #box
    PSEUDO_CLASS,   // :hover
    PSEUDO_ELEMENT  // ::before
};

// CSS 选择器结构
struct CSSSelector {
    SelectorType type;
    std::string name;
    
    CSSSelector(SelectorType t, std::string n)
        : type(t), name(std::move(n)) {}
};

// CSS 规则结构
struct CSSRule {
    std::string selector;  // 完整的选择器字符串（如 ".box", "#header"）
    std::map<std::string, std::string> properties;
    
    CSSRule(std::string sel) : selector(std::move(sel)) {}
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
    size_t currentDepth_;  // 当前嵌套深度
    std::vector<std::string> errors_;
    ElementNode* currentElement_;  // 当前正在解析的元素（用于自动添加类名/id）
    
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
    
    // CSS 选择器解析
    bool isSelector() const;
    CSSSelector parseSelector();
    CSSRule parseCSSRule();
    void parseInlineStyles(StyleNode* styleNode);
    
    // 辅助函数
    void synchronize();
    void reportError(const std::string& message);
    bool isElementStart() const;
};

} // namespace CHTL
