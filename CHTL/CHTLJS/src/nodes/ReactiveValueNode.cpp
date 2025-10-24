#include "ReactiveValueNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ReactiveValueNode::clone() const
    {
        auto node = std::make_unique<ReactiveValueNode>();
        node->variableName = variableName;
        return node;
    }

    void ReactiveValueNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        CHTL::Serializer::serialize(variableName, os);
    }

    std::unique_ptr<ReactiveValueNode> ReactiveValueNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ReactiveValueNode>();
        CHTL::Serializer::deserialize(node->variableName, is);
        return node;
    }
}
