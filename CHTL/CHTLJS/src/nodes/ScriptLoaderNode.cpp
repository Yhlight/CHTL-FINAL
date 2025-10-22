#include "ScriptLoaderNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ScriptLoaderNode::clone() const
    {
        auto node = std::make_unique<ScriptLoaderNode>();
        node->paths = paths;
        return node;
    }

    void ScriptLoaderNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        CHTL::Serializer::serialize(paths, os);
    }

    std::unique_ptr<ScriptLoaderNode> ScriptLoaderNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ScriptLoaderNode>();
        CHTL::Serializer::deserialize(node->paths, is);
        return node;
    }
}
