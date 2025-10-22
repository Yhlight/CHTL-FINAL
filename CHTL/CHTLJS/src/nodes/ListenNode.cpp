#include "ListenNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ListenNode::clone() const
    {
        auto node = std::make_unique<ListenNode>();
        if (target)
        {
            node->target = target->clone();
        }
        node->events = events;
        return node;
    }

    void ListenNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        bool has_target = target != nullptr;
        os.write(reinterpret_cast<const char*>(&has_target), sizeof(has_target));
        if (has_target)
        {
            target->serialize(os);
        }

        CHTL::Serializer::serialize(events, os);
    }

    std::unique_ptr<ListenNode> ListenNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ListenNode>();

        bool has_target;
        is.read(reinterpret_cast<char*>(&has_target), sizeof(has_target));
        if (has_target)
        {
            node->target = AstNode::deserialize(is);
        }

        CHTL::Serializer::deserialize(node->events, is);

        return node;
    }
}
