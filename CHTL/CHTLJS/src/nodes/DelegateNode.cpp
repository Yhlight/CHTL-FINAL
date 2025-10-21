#include "CHTLJS/include/nodes/DelegateNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> DelegateNode::clone() const
    {
        auto node = std::make_unique<DelegateNode>();
        node->targets = targets;
        node->events = events;
        return node;
    }
}
