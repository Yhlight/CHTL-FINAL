#pragma once

#include "ExpressionNode.h"
#include <string>
#include <memory>

namespace CHTL
{
    struct ReactiveValueNode : public Expression
    {
        std::string name;

        NodeType GetType() const override { return NodeType::ReactiveValue; }
        std::string ToString() const override { return "$" + name + "$"; }
        std::unique_ptr<AstNode> clone() const override
        {
            auto node = std::make_unique<ReactiveValueNode>();
            node->name = name;
            return node;
        }
    };
}
