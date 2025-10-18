#include "nodes/StyleNode.h"
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
}