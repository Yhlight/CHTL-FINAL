#pragma once

#include "AstNode.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTLJS
{
    struct DelegateNode : public AstNode
    {
        std::vector<std::string> targets;
        std::unordered_map<std::string, std::string> events;
        NodeType GetType() const override { return NodeType::Delegate; }
        std::string ToString() const override { return "DelegateNode"; }
        std::unique_ptr<AstNode> clone() const override;
    };

}
