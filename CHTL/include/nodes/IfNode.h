#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>

namespace CHTL
{
    struct Expression;
    struct IfNode : public AstNode
    {
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<AstNode>> consequence;
        std::unique_ptr<AstNode> alternative; // Can be an IfNode or ElseNode

        NodeType GetType() const override { return NodeType::If; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}