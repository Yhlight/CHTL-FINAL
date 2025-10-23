#include "UseNode.h"
#include <sstream>
#include "Serialize.h"

namespace CHTL
{
    std::string UseNode::ToString() const
    {
        std::stringstream ss;
        ss << "UseNode(path: [";
        for (size_t i = 0; i < path.size(); ++i)
        {
            ss << path[i];
            if (i < path.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> UseNode::clone() const
    {
        auto node = std::make_unique<UseNode>();
        node->path = this->path;
        return node;
    }

    void UseNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        Serializer::serialize(path, os);
    }

    std::unique_ptr<UseNode> UseNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<UseNode>();
        Serializer::deserialize(node->path, is);
        return node;
    }
}