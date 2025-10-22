#include "NamespaceNode.h"

namespace CHTL
{
    std::string NamespaceNode::ToString() const
    {
        return "[Namespace] " + name + " { ... }";
    }

    std::unique_ptr<AstNode> NamespaceNode::clone() const
    {
        auto node = std::make_unique<NamespaceNode>();
        node->name = this->name;
        for (const auto &child : this->children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }

    void NamespaceNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t name_len = name.length();
        os.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        os.write(name.c_str(), name_len);

        size_t child_count = children.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : children)
        {
            child->serialize(os);
        }
    }

    std::unique_ptr<NamespaceNode> NamespaceNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<NamespaceNode>();
        size_t name_len;
        is.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        node->name.resize(name_len);
        is.read(&node->name[0], name_len);

        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->children.push_back(AstNode::deserialize(is));
        }
        return node;
    }
}