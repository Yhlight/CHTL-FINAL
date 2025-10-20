#include "ConfigurationNode.h"
#include "NameConfigNode.h"
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

    std::unique_ptr<AstNode> ConfigurationNode::clone() const
    {
        auto node = std::make_unique<ConfigurationNode>();
        node->name = this->name;
        node->settings = this->settings;
        if (this->name_config)
        {
            node->name_config.reset(static_cast<NameConfigNode *>(this->name_config->clone().release()));
        }
        return node;
    }
}