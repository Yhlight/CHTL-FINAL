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
}