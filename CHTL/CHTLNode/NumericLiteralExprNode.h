#pragma once

#include "ExpressionNode.h"
#include <string>

class NumericLiteralExprNode : public ExprNode {
public:
    double value;
    std::string unit;

    NumericLiteralExprNode(double value, const std::string& unit = "")
        : value(value), unit(unit) {}

    void accept(ExpressionVisitor& visitor) override {
        visitor.visit(*this);
    }
};