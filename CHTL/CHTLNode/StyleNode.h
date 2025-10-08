#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include "StyleRuleNode.h"
#include "TemplateUsageNode.h"
#include <vector>
#include <memory>

class StyleNode : public BaseNode {
public:
    // For inline properties like `color: red;`
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    // For full CSS rules like `.box { ... }`
    std::vector<std::unique_ptr<StyleRuleNode>> rules;
    // For template usages like `@Style DefaultText;`
    std::vector<std::unique_ptr<TemplateUsageNode>> templateUsages;

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const {
        auto newNode = std::make_unique<StyleNode>();
        for (const auto& prop : properties) {
            newNode->properties.push_back(std::unique_ptr<StylePropertyNode>(static_cast<StylePropertyNode*>(prop->clone().release())));
        }
        for (const auto& rule : rules) {
            newNode->rules.push_back(std::unique_ptr<StyleRuleNode>(static_cast<StyleRuleNode*>(rule->clone().release())));
        }
        for (const auto& usage : templateUsages) {
            newNode->templateUsages.push_back(std::unique_ptr<TemplateUsageNode>(static_cast<TemplateUsageNode*>(usage->clone().release())));
        }
        return newNode;
    }
};