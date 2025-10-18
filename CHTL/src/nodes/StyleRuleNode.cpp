#include "nodes/StyleRuleNode.h"
#include "nodes/StylePropertyNode.h"
#include <sstream>

namespace CHTL
{
    std::string StyleRuleNode::ToString() const
    {
        std::stringstream ss;
        ss << "StyleRuleNode(selector: " << selector << ", properties: [";
        for (size_t i = 0; i < properties.size(); ++i)
        {
            ss << properties[i]->ToString();
            if (i < properties.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }
}