#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

class DeleteNode : public BaseNode {
public:
    std::vector<std::string> targets;

    DeleteNode(const std::vector<std::string>& targets) : targets(targets) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<DeleteNode>(targets);
    }
};