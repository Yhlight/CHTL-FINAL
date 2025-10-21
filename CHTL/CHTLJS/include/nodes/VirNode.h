#pragma once

#include "AstNode.h"
#include <string>
#include <unordered_map>

namespace CHTLJS
{
    struct VirNode : public AstNode
    {
        std::string name;
        std::unordered_map<std::string, std::string> properties;
        NodeType GetType() const override { return NodeType::Vir; }
        std::string ToString() const override { return "VirNode"; }
        std::unique_ptr<AstNode> clone() const override;
    };

}
