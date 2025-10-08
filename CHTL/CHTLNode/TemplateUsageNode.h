#pragma once

#include "BaseNode.h"
#include "TemplateDeclarationNode.h" // For TemplateType
#include <string>
#include <vector>
#include <memory>

class TemplateUsageNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> specializations;

    TemplateUsageNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<TemplateUsageNode>(templateType, name);
        for (const auto& spec : specializations) {
            newNode->specializations.push_back(spec->clone());
        }
        return newNode;
    }
};