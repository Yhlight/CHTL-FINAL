#pragma once

#include "BaseNode.h"

// Forward declarations for expression nodes
class NumericLiteralExprNode;
class BinaryOpExprNode;
class IdentifierExprNode;

class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual void visit(NumericLiteralExprNode& node) = 0;
    virtual void visit(BinaryOpExprNode& node) = 0;
    virtual void visit(IdentifierExprNode& node) = 0;
};

class ExprNode : public BaseNode {
public:
    virtual void accept(ExpressionVisitor& visitor) = 0;

    // We still need to implement the main ASTVisitor accept method
    // but expression nodes won't be visited by the main visitors directly.
    void accept(ASTVisitor& visitor) override {}
};