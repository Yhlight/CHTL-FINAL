#include "CHTLJS/include/nodes/DelegateNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> DelegateNode::clone() const
    {
        auto node = std::make_unique<DelegateNode>();
        if (parent_target)
        {
            node->parent_target = parent_target->clone();
        }
        node->child_targets = child_targets;
        node->events = events;
        return node;
    }
}
