#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    struct OriginNode : public AstNode
    {
        std::string type; // e.g., "@Html", "@Style"
        std::string name; // Optional: e.g., box in [Origin] @Html box { ... }
        std::string content;

        NodeType GetType() const override { return NodeType::Origin; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}