#include "CHTLJS/include/nodes/ListenNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ListenNode::clone() const
    {
        auto node = std::make_unique<ListenNode>();
        node->events = events;
        return node;
    }
}
