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

    std::unique_ptr<BaseNode> clone() const override {
        auto newRule = std::make_unique<StyleRuleNode>(selector);
        for (const auto& prop : properties) {
            newRule->properties.push_back(std::unique_ptr<StylePropertyNode>(static_cast<StylePropertyNode*>(prop->clone().release())));
        }
        return newRule;
    }
};