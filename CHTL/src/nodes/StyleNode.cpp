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
}