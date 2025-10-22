#include "VirNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> VirNode::clone() const
    {
        auto node = std::make_unique<VirNode>();
        node->name = name;
        node->properties = properties;
        return node;
    }

    void VirNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        CHTL::Serializer::serialize(name, os);
        CHTL::Serializer::serialize(properties, os);
    }

    std::unique_ptr<VirNode> VirNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<VirNode>();
        CHTL::Serializer::deserialize(node->name, is);
        CHTL::Serializer::deserialize(node->properties, is);
        return node;
    }
}
