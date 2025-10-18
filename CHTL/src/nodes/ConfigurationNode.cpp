#include "nodes/ConfigurationNode.h"
#include "nodes/NameConfigNode.h"
#include <sstream>

namespace CHTL
{
    std::string ConfigurationNode::ToString() const
    {
        std::stringstream ss;
        ss << "ConfigurationNode(";
        if (!name.empty())
        {
            ss << "name: " << name << ", ";
        }
        ss << "settings: [";
        for (auto it = settings.begin(); it != settings.end(); ++it)
        {
            ss << it->first << ": " << it->second;
            if (std::next(it) != settings.end())
            {
                ss << ", ";
            }
        }
        ss << "]";
        if (name_config)
        {
            ss << ", name_config: " << name_config->ToString();
        }
        ss << ")";
        return ss.str();
    }
}