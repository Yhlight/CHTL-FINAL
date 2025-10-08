#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h" // For TokenType
#include <memory>

class BinaryOpExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> left;
    TokenType op;
    std::unique_ptr<ExprNode> right;

    BinaryOpExprNode(std::unique_ptr<ExprNode> left, TokenType op, std::unique_ptr<ExprNode> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExpressionVisitor& visitor) override {
        visitor.visit(*this);
    }
};