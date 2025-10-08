#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <string>
#include <vector>
#include <memory>

class StyleRuleNode : public BaseNode {
public:
    std::string selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;

    StyleRuleNode(const std::string& selector) : selector(selector) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};