#include "ProgramNode.h"
#include <sstream>

namespace CHTL
{
    std::string ProgramNode::ToString() const
    {
        std::stringstream ss;
        ss << "ProgramNode([";
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

    std::unique_ptr<AstNode> ProgramNode::clone() const
    {
        auto node = std::make_unique<ProgramNode>();
        for (const auto& child : children)
        {
            node->children.push_back(child->clone());
        }
        // Note: templates and customs are not cloned as they are non-owning pointers.
        // The ownership is with the children vector. The map should be repopulated after cloning.
        return node;
    }
}