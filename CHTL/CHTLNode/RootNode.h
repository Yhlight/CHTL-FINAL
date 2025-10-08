#pragma once

#include "BaseNode.h"

class RootNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<RootNode>();
        for (const auto& child : children) {
            newNode->children.push_back(child->clone());
        }
        return newNode;
    }
};