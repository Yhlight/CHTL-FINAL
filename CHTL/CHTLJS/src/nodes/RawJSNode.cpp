#include "RawJSNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    std::string RawJSNode::ToString() const
    {
        return "RawJSNode(content: \"" + content + "\")";
    }

    std::unique_ptr<AstNode> RawJSNode::clone() const
    {
        auto node = std::make_unique<RawJSNode>();
        node->content = this->content;
        return node;
    }

    void RawJSNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        CHTL::Serializer::serialize(content, os);
    }

    std::unique_ptr<RawJSNode> RawJSNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<RawJSNode>();
        CHTL::Serializer::deserialize(node->content, is);
        return node;
    }

} // namespace CHTLJS
