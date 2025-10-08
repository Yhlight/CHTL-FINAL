#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateDeclarationNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> body;

    TemplateDeclarationNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};