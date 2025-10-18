#include "ExceptNode.h"
#include <sstream>

namespace CHTL
{
    std::string ExceptNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExceptNode(constraints: [";
        for (size_t i = 0; i < constraints.size(); ++i)
        {
            ss << "{";
            for (size_t j = 0; j < constraints[i].path.size(); ++j)
            {
                ss << constraints[i].path[j];
                if (j < constraints[i].path.size() - 1) ss << " ";
            }
            ss << "}";
            if (i < constraints.size() - 1) ss << ", ";
        }
        ss << "])";
        return ss.str();
    }
}