#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>

namespace CHTLJS
{
    struct ProgramNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override { return "ProgramNode"; }
        std::unique_ptr<AstNode> clone() const override;
    };
}
