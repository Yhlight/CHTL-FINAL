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
};