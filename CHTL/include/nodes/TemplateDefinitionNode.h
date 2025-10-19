#pragma once

#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace CHTL
{
    struct Expression; // Forward declaration

    // 模板定义节点, e.g., [Template] @Style DefaultText { ... }
    struct TemplateDefinitionNode : public AstNode
    {
        std::string type; // e.g., "@Style", "@Element", "@Var"
        std::string name;
        std::vector<std::string> inherited_templates;
        // For @Var templates
        std::map<std::string, std::unique_ptr<Expression>> variables;
        // For @Style templates
        std::vector<std::unique_ptr<AstNode>> properties; // Can be StyleProperty or TemplateUsage
        // For @Element templates
        std::vector<std::unique_ptr<AstNode>> body;

        NodeType GetType() const override { return NodeType::TemplateDefinition; }
        std::string ToString() const override;
    };
}