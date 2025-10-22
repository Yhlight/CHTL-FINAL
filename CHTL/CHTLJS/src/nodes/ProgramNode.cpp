#include "ProgramNode.h"
#include "AstNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ProgramNode::clone() const
    {
        auto node = std::make_unique<ProgramNode>();
        for (const auto& child : children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }

    void ProgramNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t child_count = children.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : children)
        {
            child->serialize(os);
        }
    }

    std::unique_ptr<ProgramNode> ProgramNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ProgramNode>();
        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->children.push_back(AstNode::deserialize(is));
        }
        return node;
    }
}
