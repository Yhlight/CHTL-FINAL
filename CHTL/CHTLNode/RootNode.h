#pragma once

#include "BaseNode.h"

class RootNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};