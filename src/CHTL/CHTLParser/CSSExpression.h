#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLCommon/CSSValue.h"
#include "CHTL/CHTLCommon/CompilerError.h"
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CSS 表达式解析器和求值器
 * 
 * 严格遵循 CHTL.md 第175-202行规范：
 * - 支持算术运算符: + - * / % **
 * - 运算符优先级: ** > * / % > + -
 * - 括号表达式
 * - 左结合
 * - 单位验证
 * 
 * 语法（递归下降）:
 * expression  → term (('+' | '-') term)*
 * term        → factor (('*' | '/' | '%') factor)*
 * factor      → unary ('**' unary)?
 * unary       → ('-' | '+')? primary
 * primary     → NUMBER UNIT?
 *             | '(' expression ')'
 */
class CSSExpression {
public:
    /**
     * @brief 从 token 流解析并求值表达式
     * @param tokens Token 流
     * @param start 起始位置（引用，会被更新）
     * @return 求值结果
     */
    static CSSValue evaluate(const std::vector<Token>& tokens, size_t& start);
    
    /**
     * @brief 判断 token 序列是否包含表达式（是否有运算符）
     */
    static bool isExpression(const std::vector<Token>& tokens, size_t start);
    
private:
    CSSExpression(const std::vector<Token>& tokens, size_t& current)
        : tokens_(tokens), current_(current) {}
    
    // 递归下降解析
    CSSValue parseExpression();
    CSSValue parseTerm();
    CSSValue parseFactor();
    CSSValue parseUnary();
    CSSValue parsePrimary();
    
    // Token 操作
    Token peek() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool isAtEnd() const;
    
    const std::vector<Token>& tokens_;
    size_t& current_;
};

// ===== 实现 =====

inline CSSValue CSSExpression::evaluate(const std::vector<Token>& tokens, size_t& start) {
    CSSExpression parser(tokens, start);
    return parser.parseExpression();
}

inline bool CSSExpression::isExpression(const std::vector<Token>& tokens, size_t start) {
    // 检查是否包含运算符
    for (size_t i = start; i < tokens.size(); i++) {
        TokenType type = tokens[i].type;
        
        // 遇到分号或右花括号，停止
        if (type == TokenType::SEMICOLON || type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        // 检查是否是运算符
        if (type == TokenType::PLUS || type == TokenType::MINUS ||
            type == TokenType::STAR || type == TokenType::SLASH ||
            type == TokenType::PERCENT || type == TokenType::POWER ||
            type == TokenType::LEFT_PAREN || type == TokenType::RIGHT_PAREN) {
            return true;
        }
    }
    
    return false;
}

inline CSSValue CSSExpression::parseExpression() {
    // expression → term (('+' | '-') term)*
    CSSValue left = parseTerm();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens_[current_ - 1];
        CSSValue right = parseTerm();
        
        try {
            if (op.type == TokenType::PLUS) {
                left = left + right;
            } else {
                left = left - right;
            }
        } catch (const std::exception& e) {
            throw ParseError(e.what(), op.line, op.column);
        }
    }
    
    return left;
}

inline CSSValue CSSExpression::parseTerm() {
    // term → factor (('*' | '/' | '%') factor)*
    CSSValue left = parseFactor();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        Token op = tokens_[current_ - 1];
        CSSValue right = parseFactor();
        
        try {
            if (op.type == TokenType::STAR) {
                left = left * right;
            } else if (op.type == TokenType::SLASH) {
                left = left / right;
            } else {
                left = left % right;
            }
        } catch (const std::exception& e) {
            throw ParseError(e.what(), op.line, op.column);
        }
    }
    
    return left;
}

inline CSSValue CSSExpression::parseFactor() {
    // factor → unary ('**' unary)?
    CSSValue left = parseUnary();
    
    if (match(TokenType::POWER)) {
        Token op = tokens_[current_ - 1];
        CSSValue right = parseUnary();
        
        try {
            left = left.power(right);
        } catch (const std::exception& e) {
            throw ParseError(e.what(), op.line, op.column);
        }
    }
    
    return left;
}

inline CSSValue CSSExpression::parseUnary() {
    // unary → ('-' | '+')? primary
    if (match(TokenType::MINUS)) {
        return -parseUnary();
    }
    
    if (match(TokenType::PLUS)) {
        return +parseUnary();
    }
    
    return parsePrimary();
}

inline CSSValue CSSExpression::parsePrimary() {
    // primary → NUMBER UNIT? | '(' expression ')'
    
    // 括号表达式
    if (match(TokenType::LEFT_PAREN)) {
        CSSValue value = parseExpression();
        consume(TokenType::RIGHT_PAREN, "期望 ')' 来闭合表达式");
        return value;
    }
    
    // 数字（可能带单位）
    if (check(TokenType::NUMBER)) {
        Token numToken = advance();
        double number = std::stod(numToken.value);
        
        // 检查是否有单位
        std::string unit;
        
        // 单位可能是 IDENTIFIER 或 PERCENT
        if (check(TokenType::IDENTIFIER)) {
            unit = advance().value;
        } else if (check(TokenType::PERCENT)) {
            unit = advance().value;  // "%"
        }
        
        return CSSValue(number, unit);
    }
    
    Token current = peek();
    throw ParseError("期望数字或表达式", current.line, current.column);
}

inline Token CSSExpression::peek() const {
    if (isAtEnd()) {
        return tokens_.back();
    }
    return tokens_[current_];
}

inline Token CSSExpression::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return tokens_[current_ - 1];
}

inline bool CSSExpression::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

inline bool CSSExpression::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

inline Token CSSExpression::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    
    Token current = peek();
    throw ParseError(message, current.line, current.column);
}

inline bool CSSExpression::isAtEnd() const {
    if (current_ >= tokens_.size()) {
        return true;
    }
    
    TokenType type = tokens_[current_].type;
    return type == TokenType::END_OF_FILE || 
           type == TokenType::SEMICOLON || 
           type == TokenType::RIGHT_BRACE;
}

} // namespace CHTL
