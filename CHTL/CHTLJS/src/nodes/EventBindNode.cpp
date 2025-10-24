#include "EventBindNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> EventBindNode::clone() const
    {
        auto node = std::make_unique<EventBindNode>();
        if (target)
        {
            node->target = target->clone();
        }
        node->eventNames = eventNames;
        node->handler = handler;
        node->isBlock = isBlock;
        node->blockEvents = blockEvents;
        return node;
    }

    void EventBindNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        bool has_target = target != nullptr;
        os.write(reinterpret_cast<const char*>(&has_target), sizeof(has_target));
        if (has_target)
        {
            target->serialize(os);
        }

        // Serialize eventNames
        size_t size = eventNames.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& name : eventNames)
        {
            CHTL::Serializer::serialize(name, os);
        }

        // Serialize handler
        CHTL::Serializer::serialize(handler, os);

        // Serialize isBlock
        os.write(reinterpret_cast<const char*>(&isBlock), sizeof(isBlock));

        // Serialize blockEvents
        size = blockEvents.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [events, h] : blockEvents)
        {
            size_t eventSize = events.size();
            os.write(reinterpret_cast<const char*>(&eventSize), sizeof(eventSize));
            for (const auto& evt : events)
            {
                CHTL::Serializer::serialize(evt, os);
            }
            CHTL::Serializer::serialize(h, os);
        }
    }

    std::unique_ptr<EventBindNode> EventBindNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<EventBindNode>();

        bool has_target;
        is.read(reinterpret_cast<char*>(&has_target), sizeof(has_target));
        if (has_target)
        {
            node->target = AstNode::deserialize(is);
        }

        // Deserialize eventNames
        size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        node->eventNames.resize(size);
        for (auto& name : node->eventNames)
        {
            CHTL::Serializer::deserialize(name, is);
        }

        // Deserialize handler
        CHTL::Serializer::deserialize(node->handler, is);

        // Deserialize isBlock
        is.read(reinterpret_cast<char*>(&node->isBlock), sizeof(node->isBlock));

        // Deserialize blockEvents
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        node->blockEvents.resize(size);
        for (auto& [events, h] : node->blockEvents)
        {
            size_t eventSize;
            is.read(reinterpret_cast<char*>(&eventSize), sizeof(eventSize));
            events.resize(eventSize);
            for (auto& evt : events)
            {
                CHTL::Serializer::deserialize(evt, is);
            }
            CHTL::Serializer::deserialize(h, is);
        }

        return node;
    }
}
