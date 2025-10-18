#include "nodes/ElementNode.h"
#include <sstream>

namespace CHTL
{
    std::string ElementNode::ToString() const
    {
        std::stringstream ss;
        ss << "ElementNode(tag: " << tag_name << ", attributes: [";
        for (size_t i = 0; i < attributes.size(); ++i)
        {
            ss << attributes[i].name << ": \"" << attributes[i].value << "\"";
            if (i < attributes.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "], children: [";
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