#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

enum class InsertPosition {
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM
};

class InsertNode : public BaseNode {
public:
    InsertPosition position;
    std::string targetSelector; // e.g., "div[0]"
    std::vector<std::unique_ptr<BaseNode>> nodesToInsert;

    InsertNode(InsertPosition pos, const std::string& target)
        : position(pos), targetSelector(target) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<InsertNode>(position, targetSelector);
        for (const auto& node : nodesToInsert) {
            newNode->nodesToInsert.push_back(node->clone());
        }
        return newNode;
    }
};