#ifndef CHTL_CONDITIONALEXPRESSIONNODE_H
#define CHTL_CONDITIONALEXPRESSIONNODE_H

#include "ExpressionNode.h"
#include <memory>

namespace CHTL {

class ConditionalExpressionNode : public ExpressionNode {
public:
    ConditionalExpressionNode(Token token)
        : m_token(std::move(token)), m_condition(nullptr), m_consequence(nullptr), m_alternative(nullptr) {}

    std::string GetTokenLiteral() const override { return m_token.literal; }

    void SetCondition(std::unique_ptr<ExpressionNode> condition) {
        m_condition = std::move(condition);
    }

    void SetConsequence(std::unique_ptr<ExpressionNode> consequence) {
        m_consequence = std::move(consequence);
    }

    void SetAlternative(std::unique_ptr<ExpressionNode> alternative) {
        m_alternative = std::move(alternative);
    }

    std::string ToString(int indent = 0) const override {
        std::string out = indentString(indent) + "(";
        out += m_condition ? m_condition->ToString(0) : "null";
        out += " ? ";
        out += m_consequence ? m_consequence->ToString(0) : "null";
        out += " : ";
        out += m_alternative ? m_alternative->ToString(0) : "null";
        out += ")";
        return out;
    }

private:
    Token m_token; // The '?' token
    std::unique_ptr<ExpressionNode> m_condition;
    std::unique_ptr<ExpressionNode> m_consequence;
    std::unique_ptr<ExpressionNode> m_alternative;
};

} // namespace CHTL

#endif // CHTL_CONDITIONALEXPRESSIONNODE_H