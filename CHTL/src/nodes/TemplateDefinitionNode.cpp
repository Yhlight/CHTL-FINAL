#include "TemplateDefinitionNode.h"
#include "StylePropertyNode.h"
#include <sstream>

namespace CHTL
{
    std::string TemplateDefinitionNode::ToString() const
    {
        std::stringstream ss;
        ss << "TemplateDefinition(" << type << " " << name << " { ";
        if (!properties.empty())
        {
            for (size_t i = 0; i < properties.size(); ++i)
            {
                ss << properties[i]->ToString();
                if (i < properties.size() - 1)
                {
                    ss << ", ";
                }
            }
        }
        if (!body.empty())
        {
            for (size_t i = 0; i < body.size(); ++i)
            {
                ss << body[i]->ToString();
                if (i < body.size() - 1)
                {
                    ss << ", ";
                }
            }
        }
        ss << " })";
        return ss.str();
    }
}