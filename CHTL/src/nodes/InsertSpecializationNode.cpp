#include "InsertSpecializationNode.h"
#include <sstream>

namespace CHTL
{
    std::string InsertSpecializationNode::ToString() const
    {
        std::stringstream ss;
        ss << "InsertSpecializationNode(position: " << position;
        if (!target_selector.empty())
        {
            ss << ", target: " << target_selector;
        }
        ss << ", content: [";
        for (size_t i = 0; i < content.size(); ++i)
        {
            ss << content[i]->ToString();
            if (i < content.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> InsertSpecializationNode::clone() const
    {
        auto node = std::make_unique<InsertSpecializationNode>();
        node->position = this->position;
        node->target_selector = this->target_selector;
        for (const auto &child : this->content)
        {
            node->content.push_back(child->clone());
        }
        return node;
    }
}