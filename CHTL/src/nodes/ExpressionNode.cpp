#include "nodes/ExpressionNode.h"
#include <sstream>

namespace CHTL
{
    std::string ConditionalExpression::ToString() const
    {
        std::stringstream ss;
        ss << "(" << condition->ToString() << " ? " << consequence->ToString();
        if (alternative)
        {
            ss << " : " << alternative->ToString();
        }
        ss << ")";
        return ss.str();
    }

    std::string VariableAccessNode::ToString() const
    {
        return template_name + "(" + variable_name + ")";
    }

    std::string ExpressionListNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExpressionList([";
        for (size_t i = 0; i < expressions.size(); ++i)
        {
            ss << expressions[i]->ToString();
            if (i < expressions.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }
}