#include "AstNode.h"
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

    std::string StyleNode::ToString() const
    {
        std::stringstream ss;
        ss << "StyleNode(properties: [";
        for (size_t i = 0; i < properties.size(); ++i)
        {
            ss << properties[i].name << ": \"" << properties[i].value << "\"";
            if (i < properties.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::string ProgramNode::ToString() const
    {
        std::stringstream ss;
        ss << "ProgramNode([";
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
