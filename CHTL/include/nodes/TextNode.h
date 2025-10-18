#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    // 文本节点，例如 text { "content" }
    struct TextNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Text; }
        std::string ToString() const override;
    };
}