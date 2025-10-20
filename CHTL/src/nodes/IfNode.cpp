#include "nodes/IfNode.h"
#include "nodes/ExpressionNode.h"
#include <sstream>

namespace CHTL
{
    std::string IfNode::ToString() const
    {
        std::stringstream ss;
        ss << "IfNode(condition: " << condition->ToString() << ", consequence: [";
        for (size_t i = 0; i < consequence.size(); ++i)
        {
            ss << consequence[i]->ToString();
            if (i < consequence.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        if (alternative)
        {
            ss << " else " << alternative->ToString();
        }
        return ss.str();
    }

    std::unique_ptr<AstNode> IfNode::clone() const
    {
        auto node = std::make_unique<IfNode>();
        if (this->condition)
        {
            node->condition.reset(static_cast<Expression *>(this->condition->clone().release()));
        }
        for (const auto &child : this->consequence)
        {
            node->consequence.push_back(child->clone());
        }
        if (this->alternative)
        {
            node->alternative = this->alternative->clone();
        }
        return node;
    }
}