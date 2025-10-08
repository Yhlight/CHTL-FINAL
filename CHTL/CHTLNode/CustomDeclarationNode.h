#pragma once

#include "BaseNode.h"
#include "TemplateDeclarationNode.h" // For TemplateType
#include <string>
#include <vector>
#include <memory>

class CustomDeclarationNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> body;

    CustomDeclarationNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<CustomDeclarationNode>(templateType, name);
        for (const auto& node : body) {
            newNode->body.push_back(node->clone());
        }
        return newNode;
    }
};