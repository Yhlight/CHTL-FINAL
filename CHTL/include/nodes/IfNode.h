#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>

namespace CHTL
{
    struct Expression;
    struct StyleProperty;

    struct IfNode : public AstNode
    {
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<StyleProperty>> consequence;
        // TODO: Add support for else-if and else chains

        NodeType GetType() const override { return NodeType::If; }
        std::string ToString() const override;
    };
}