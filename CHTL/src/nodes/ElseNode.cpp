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
}