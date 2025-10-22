#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    struct StyleProperty; // Forward declaration

    // CSS 规则节点, e.g., .box { width: 100px; }
    struct StyleRuleNode : public AstNode
    {
        std::string selector;
        std::vector<std::unique_ptr<StyleProperty>> properties;

        NodeType GetType() const override { return NodeType::StyleRule; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<StyleRuleNode> deserialize(std::istream& is);
    };
}