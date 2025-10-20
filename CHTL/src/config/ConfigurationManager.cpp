#include "config/ConfigurationManager.h"
#include <iostream>

namespace CHTL
{

    ConfigurationManager ConfigurationManager::s_instance;

    ConfigurationManager &ConfigurationManager::GetInstance()
    {
        return s_instance;
    }

    void ConfigurationManager::AddKeywordMapping(const std::string &configName, const std::string &internalName, const std::string &customLiteral)
    {
        m_configurations[configName][internalName] = customLiteral;
    }

    bool ConfigurationManager::SetActiveConfiguration(const std::string &configName)
    {
        if (m_configurations.count(configName))
        {
            m_activeConfigName = configName;
            return true;
        }
        return false;
    }

    std::string ConfigurationManager::GetKeyword(const std::string &internalName) const
    {
        // Find the active configuration
        auto config_it = m_configurations.find(m_activeConfigName);
        if (config_it != m_configurations.end())
        {
            // Find the keyword in the active configuration's map
            auto keyword_it = config_it->second.find(internalName);
            if (keyword_it != config_it->second.end())
            {
                return keyword_it->second;
            }
        }
        // Fallback to the internal name if not found
        return internalName;
    }

    void ConfigurationManager::Reset()
    {
        s_instance.m_configurations.clear();
        s_instance.m_activeConfigName = "default";
    }

}
