#include "CHTLJS/include/nodes/AnimateNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> AnimateNode::clone() const
    {
        auto node = std::make_unique<AnimateNode>();
        node->target = target;
        node->duration = duration;
        node->easing = easing;
        node->begin_state = begin_state;
        node->when_states = when_states;
        node->end_state = end_state;
        node->loop = loop;
        node->direction = direction;
        node->delay = delay;
        node->callback = callback;
        return node;
    }
}
