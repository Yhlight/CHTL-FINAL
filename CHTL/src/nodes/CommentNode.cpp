#include "nodes/CommentNode.h"

namespace CHTL
{
    std::unique_ptr<AstNode> CommentNode::clone() const
    {
        auto node = std::make_unique<CommentNode>();
        node->value = this->value;
        return node;
    }

    void CommentNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t len = value.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(value.c_str(), len);
    }

    std::unique_ptr<CommentNode> CommentNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<CommentNode>();
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->value.resize(len);
        is.read(&node->value[0], len);
        return node;
    }
}
