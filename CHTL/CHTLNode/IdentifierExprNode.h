#pragma once

#include "ExpressionNode.h"
#include <string>

class IdentifierExprNode : public ExprNode {
public:
    std::string name;

    IdentifierExprNode(const std::string& name) : name(name) {}

    void accept(ExpressionVisitor& visitor) override {
        visitor.visit(*this);
    }
};