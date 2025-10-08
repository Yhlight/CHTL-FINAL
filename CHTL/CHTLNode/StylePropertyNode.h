#pragma once

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <memory>

class StylePropertyNode : public BaseNode {
public:
    std::string key;
    std::unique_ptr<ExprNode> value;

    StylePropertyNode(const std::string& key, std::unique_ptr<ExprNode> value)
        : key(key), value(std::move(value)) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};