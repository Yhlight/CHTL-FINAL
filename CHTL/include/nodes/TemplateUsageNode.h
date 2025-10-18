#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    // 模板使用节点, e.g., @Style DefaultText;
    struct TemplateUsageNode : public AstNode
    {
        std::string type; // e.g., "@Style"
        std::string name;

        NodeType GetType() const override { return NodeType::TemplateUsage; }
        std::string ToString() const override;
    };
}