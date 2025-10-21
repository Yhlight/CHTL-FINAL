#include "nodes/CommentNode.h"

namespace CHTL
{
    std::unique_ptr<AstNode> CommentNode::clone() const
    {
        auto node = std::make_unique<CommentNode>();
        node->value = this->value;
        return node;
    }
}
