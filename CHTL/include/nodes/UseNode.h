#pragma once

#include "AstNode.h"
#include <vector>
#include <string>

namespace CHTL
{
    struct UseNode : public AstNode
    {
        std::vector<std::string> path; // e.g., {"[Configuration]", "@Config", "Basic"} or {"html5"}

        NodeType GetType() const override { return NodeType::Use; }
        std::string ToString() const override;
    };
}