#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>

namespace CHTL
{
    // 样式节点, e.g., style { ... }
    struct StyleNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Style; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<StyleNode> deserialize(std::istream& is);
    };
}