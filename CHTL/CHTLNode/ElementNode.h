#pragma once

#include "BaseNode.h"
#include <map>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};