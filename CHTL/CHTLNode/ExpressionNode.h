#ifndef CHTL_EXPRESSIONNODE_H
#define CHTL_EXPRESSIONNODE_H

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

// Base class for all expression nodes
class ExpressionNode : public BaseNode {
public:
    // Pure virtual functions from BaseNode will be implemented by concrete expression nodes
};

// Represents a literal value in an expression, like a number or a string
class LiteralNode : public ExpressionNode {
public:
    LiteralNode(Token token) : m_token(std::move(token)) {}

    std::string GetTokenLiteral() const override { return m_token.literal; }
    std::string ToString(int indent = 0) const override {
        return indentString(indent) + m_token.literal;
    }
private:
    Token m_token;
};


// Represents an infix operation, like 'width + 10' or 'height > 50'
class InfixExpressionNode : public ExpressionNode {
public:
    InfixExpressionNode(Token token, std::unique_ptr<ExpressionNode> left)
        : m_token(std::move(token)), m_left(std::move(left)), m_right(nullptr) {}

    std::string GetTokenLiteral() const override { return m_token.literal; }

    void SetRight(std::unique_ptr<ExpressionNode> right) {
        m_right = std::move(right);
    }

    std::string ToString(int indent = 0) const override {
        std::string out = indentString(indent) + "(";
        out += m_left ? m_left->ToString(0) : "null";
        out += " " + m_token.literal + " ";
        out += m_right ? m_right->ToString(0) : "null";
        out += ")";
        return out;
    }

private:
    Token m_token; // The operator token (e.g., +, <)
    std::unique_ptr<ExpressionNode> m_left;
    std::unique_ptr<ExpressionNode> m_right;
};

} // namespace CHTL

#endif // CHTL_EXPRESSIONNODE_H