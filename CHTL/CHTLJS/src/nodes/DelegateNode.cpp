#include "DelegateNode.h"
#include "AstNode.h"
#include "Serialize.h"

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

    void DelegateNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        bool has_parent_target = parent_target != nullptr;
        os.write(reinterpret_cast<const char*>(&has_parent_target), sizeof(has_parent_target));
        if (has_parent_target)
        {
            parent_target->serialize(os);
        }

        CHTL::Serializer::serialize(child_targets, os);
        CHTL::Serializer::serialize(events, os);
    }

    std::unique_ptr<DelegateNode> DelegateNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<DelegateNode>();

        bool has_parent_target;
        is.read(reinterpret_cast<char*>(&has_parent_target), sizeof(has_parent_target));
        if (has_parent_target)
        {
            node->parent_target = AstNode::deserialize(is);
        }

        CHTL::Serializer::deserialize(node->child_targets, is);
        CHTL::Serializer::deserialize(node->events, is);

        return node;
    }
}
