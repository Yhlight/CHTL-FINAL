#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    // 文本节点，例如 text { "content" }
    struct TextNode : public AstNode
    {
        std::string value;

        TextNode() = default;
        TextNode(std::string val) : value(std::move(val)) {}

        NodeType GetType() const override { return NodeType::Text; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<TextNode> deserialize(std::istream& is);
    };
}