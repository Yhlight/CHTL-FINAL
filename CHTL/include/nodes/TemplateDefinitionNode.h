#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    struct StyleProperty; // Forward declaration

    // 模板定义节点, e.g., [Template] @Style DefaultText { ... }
    struct TemplateDefinitionNode : public AstNode
    {
        std::string type; // e.g., "@Style", "@Element"
        std::string name;
        // For @Style or @Var templates
        std::vector<std::unique_ptr<StyleProperty>> properties;
        // For @Element templates
        std::vector<std::unique_ptr<AstNode>> body;

        NodeType GetType() const override { return NodeType::TemplateDefinition; }
        std::string ToString() const override;
    };
}