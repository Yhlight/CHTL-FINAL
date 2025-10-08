#pragma once

#include "BaseNode.h"

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};