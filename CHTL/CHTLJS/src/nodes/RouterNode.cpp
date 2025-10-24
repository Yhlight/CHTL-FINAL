#include "RouterNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> RouterNode::clone() const
    {
        auto node = std::make_unique<RouterNode>();
        node->urlPageMappings = urlPageMappings;
        node->rootPath = rootPath;
        if (rootContainer)
        {
            node->rootContainer = rootContainer->clone();
        }
        node->mode = mode;
        return node;
    }

    void RouterNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        // Serialize urlPageMappings
        size_t size = urlPageMappings.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [url, page] : urlPageMappings)
        {
            CHTL::Serializer::serialize(url, os);
            CHTL::Serializer::serialize(page, os);
        }

        // Serialize rootPath
        CHTL::Serializer::serialize(rootPath, os);

        // Serialize rootContainer
        bool has_container = rootContainer != nullptr;
        os.write(reinterpret_cast<const char*>(&has_container), sizeof(has_container));
        if (has_container)
        {
            rootContainer->serialize(os);
        }

        // Serialize mode
        CHTL::Serializer::serialize(mode, os);
    }

    std::unique_ptr<RouterNode> RouterNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<RouterNode>();

        // Deserialize urlPageMappings
        size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        node->urlPageMappings.resize(size);
        for (auto& [url, page] : node->urlPageMappings)
        {
            CHTL::Serializer::deserialize(url, is);
            CHTL::Serializer::deserialize(page, is);
        }

        // Deserialize rootPath
        CHTL::Serializer::deserialize(node->rootPath, is);

        // Deserialize rootContainer
        bool has_container;
        is.read(reinterpret_cast<char*>(&has_container), sizeof(has_container));
        if (has_container)
        {
            node->rootContainer = AstNode::deserialize(is);
        }

        // Deserialize mode
        CHTL::Serializer::deserialize(node->mode, is);

        return node;
    }
}
