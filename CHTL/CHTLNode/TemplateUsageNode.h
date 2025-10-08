#pragma once

#include "BaseNode.h"
#include "TemplateDeclarationNode.h" // For TemplateType
#include <string>

class TemplateUsageNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;

    TemplateUsageNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};