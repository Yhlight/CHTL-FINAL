#include "IfNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
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
        return ss.str();
    }
}