#include "StyleNode.h"
#include <sstream>

namespace CHTL
{
    std::string StyleNode::ToString() const
    {
        std::stringstream ss;
        ss << "StyleNode(children: [";
        for (size_t i = 0; i < children.size(); ++i)
        {
            ss << children[i]->ToString();
            if (i < children.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> StyleNode::clone() const
    {
        auto node = std::make_unique<StyleNode>();
        for (const auto& child : children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }

    void StyleNode::serialize(std::ostream& os) const
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

    std::unique_ptr<StyleNode> StyleNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<StyleNode>();
        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->children.push_back(AstNode::deserialize(is));
        }
        return node;
    }
}