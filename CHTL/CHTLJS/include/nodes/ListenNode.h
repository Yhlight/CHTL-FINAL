#pragma once

#include "AstNode.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTLJS
{
    struct ListenNode : public AstNode
    {
        std::unique_ptr<AstNode> target;
        std::unordered_map<std::string, std::string> events;
        NodeType GetType() const override { return NodeType::Listen; }
        std::string ToString() const override { return "ListenNode"; }
        std::unique_ptr<AstNode> clone() const override;
    };

}
