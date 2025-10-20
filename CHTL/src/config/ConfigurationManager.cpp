#include "config/ConfigurationManager.h"
#include "nodes/NameConfigNode.h" // Include for NameConfigNode definition

namespace CHTL
{
    ConfigurationManager& ConfigurationManager::GetInstance()
    {
        static ConfigurationManager instance;
        return instance;
    }

    std::string ConfigurationManager::RegisterConfiguration(std::unique_ptr<ConfigurationNode> config)
    {
        if (!config->name.empty()) // It's a named configuration
        {
            if (m_named_configs.count(config->name))
            {
                return "Error: Duplicate configuration name '" + config->name + "'.";
            }
            m_named_configs[config->name] = std::move(config);
        }
        else // It's an unnamed (default) configuration
        {
            if (m_unnamed_config)
            {
                return "Error: Multiple unnamed (default) configuration blocks are not allowed.";
            }
            m_unnamed_config = std::move(config);
        }
        return ""; // Success
    }

    bool ConfigurationManager::SetActiveConfiguration(const std::string& name)
    {
        auto it = m_named_configs.find(name);
        if (it != m_named_configs.end())
        {
            m_active_config = it->second.get();
            return true;
        }
        return false;
    }

    const ConfigurationNode* ConfigurationManager::GetActiveConfiguration() const
    {
        if (m_active_config)
        {
            return m_active_config;
        }
        return m_unnamed_config.get();
    }

    std::unordered_map<std::string, std::string> ConfigurationManager::GetActiveKeywords() const
    {
        const auto* active_config = GetActiveConfiguration();
        if (active_config && active_config->name_config)
        {
            return active_config->name_config->settings;
        }
        return {}; // Return empty map if no active/default config or no [Name] block
    }

    void ConfigurationManager::Reset()
    {
        m_named_configs.clear();
        m_unnamed_config.reset();
        m_active_config = nullptr;
    }
}