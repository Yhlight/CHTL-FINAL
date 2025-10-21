#pragma once

#include "AstNode.h"
#include <memory>
#include <string>

namespace CHTL
{
    struct Expression; // Forward declaration

    // 样式属性结构体, e.g., width: 100 + 50;
    struct StyleProperty : public AstNode
    {
        std::string name;
        std::unique_ptr<Expression> value;

        NodeType GetType() const override { return NodeType::StyleProperty; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}