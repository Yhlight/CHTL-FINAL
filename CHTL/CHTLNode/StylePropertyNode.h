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

    std::unique_ptr<BaseNode> clone() const override {
        auto valueClone = value ? std::unique_ptr<ExprNode>(static_cast<ExprNode*>(value->clone().release())) : nullptr;
        return std::make_unique<StylePropertyNode>(key, std::move(valueClone));
    }
};