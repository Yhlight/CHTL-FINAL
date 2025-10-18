#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    // 自定义使用节点, e.g., @Style MyCustomStyle { ... };
    struct CustomUsageNode : public AstNode
    {
        std::string type; // e.g., "@Style"
        std::string name;
        // Specializations
        std::vector<std::unique_ptr<AstNode>> specializations;

        NodeType GetType() const override { return NodeType::CustomUsage; }
        std::string ToString() const override;
    };
}