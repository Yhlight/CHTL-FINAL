#include "AnimateNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> AnimateNode::clone() const
    {
        auto node = std::make_unique<AnimateNode>();
        if (target)
        {
            node->target = target->clone();
        }
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

    void AnimateNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        bool has_target = target != nullptr;
        os.write(reinterpret_cast<const char*>(&has_target), sizeof(has_target));
        if (has_target)
        {
            target->serialize(os);
        }

        os.write(reinterpret_cast<const char*>(&duration), sizeof(duration));
        CHTL::Serializer::serialize(easing, os);
        CHTL::Serializer::serialize(begin_state, os);
        CHTL::Serializer::serialize(when_states, os);
        CHTL::Serializer::serialize(end_state, os);
        os.write(reinterpret_cast<const char*>(&loop), sizeof(loop));
        CHTL::Serializer::serialize(direction, os);
        os.write(reinterpret_cast<const char*>(&delay), sizeof(delay));
        CHTL::Serializer::serialize(callback, os);
    }

    std::unique_ptr<AnimateNode> AnimateNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<AnimateNode>();

        bool has_target;
        is.read(reinterpret_cast<char*>(&has_target), sizeof(has_target));
        if (has_target)
        {
            node->target = AstNode::deserialize(is);
        }

        is.read(reinterpret_cast<char*>(&node->duration), sizeof(node->duration));
        CHTL::Serializer::deserialize(node->easing, is);
        CHTL::Serializer::deserialize(node->begin_state, is);
        CHTL::Serializer::deserialize(node->when_states, is);
        CHTL::Serializer::deserialize(node->end_state, is);
        is.read(reinterpret_cast<char*>(&node->loop), sizeof(node->loop));
        CHTL::Serializer::deserialize(node->direction, is);
        is.read(reinterpret_cast<char*>(&node->delay), sizeof(node->delay));
        CHTL::Serializer::deserialize(node->callback, is);

        return node;
    }
}
