#include "TextNode.h"

namespace CHTL
{
    std::string TextNode::ToString() const
    {
        return "TextNode(\"" + value + "\")";
    }

    std::unique_ptr<AstNode> TextNode::clone() const
    {
        auto node = std::make_unique<TextNode>();
        node->value = this->value;
        return node;
    }
}