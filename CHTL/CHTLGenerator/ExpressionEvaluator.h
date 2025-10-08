#pragma once

#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/NumericLiteralExprNode.h"
#include "../CHTLNode/IdentifierExprNode.h"
#include "../CHTLNode/BinaryOpExprNode.h"
#include <string>
#include <stdexcept>

// A simple struct to hold the result of an evaluation.
struct EvaluatedValue {
    double value;
    std::string unit;
    std::string stringValue; // For identifiers
    bool isNumeric = false;
};

class ExpressionEvaluator : public ExpressionVisitor {
public:
    EvaluatedValue evaluate(ExprNode& expr);

    void visit(NumericLiteralExprNode& node) override;
    void visit(IdentifierExprNode& node) override;
    void visit(BinaryOpExprNode& node) override;

private:
    EvaluatedValue result;
};