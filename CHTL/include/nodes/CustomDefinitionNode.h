#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    // 自定义定义节点, e.g., [Custom] @Style MyStyle { ... }
    struct CustomDefinitionNode : public AstNode
    {
        std::string type; // e.g., "@Style", "@Element"
        std::string name;
        // Can contain properties for @Style, a body for @Element, and also specializations
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::CustomDefinition; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}