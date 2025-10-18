#include "nodes/UseNode.h"
#include <sstream>

namespace CHTL
{
    std::string UseNode::ToString() const
    {
        std::stringstream ss;
        ss << "UseNode(path: [";
        for (size_t i = 0; i < path.size(); ++i)
        {
            ss << path[i];
            if (i < path.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }
}