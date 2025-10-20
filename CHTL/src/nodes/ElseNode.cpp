#include "nodes/ElseNode.h"
#include <sstream>

namespace CHTL
{
    std::string ElseNode::ToString() const
    {
        std::stringstream ss;
        ss << "ElseNode(consequence: [";
        for (size_t i = 0; i < consequence.size(); ++i)
        {
            ss << consequence[i]->ToString();
            if (i < consequence.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> ElseNode::clone() const
    {
        auto node = std::make_unique<ElseNode>();
        for (const auto &child : this->consequence)
        {
            node->consequence.push_back(child->clone());
        }
        return node;
    }
}