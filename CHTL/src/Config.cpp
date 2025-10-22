#include "Config.h"

namespace CHTL
{
    Config::Config()
    {
        // Set default values
        m_settings["DEBUG_MODE"] = "false";
        m_settings["INDEX_INITIAL_COUNT"] = "0";
    }

    void Config::Load(const ConfigurationNode* config_node)
    {
        if (!config_node) return;

        for (const auto& pair : config_node->settings)
        {
            m_settings[pair.first] = pair.second;
        }
        m_loaded = true;
    }

    bool Config::IsDebugMode() const
    {
        return m_settings.at("DEBUG_MODE") == "true";
    }

    int Config::GetIndexInitialCount() const
    {
        try
        {
            return std::stoi(m_settings.at("INDEX_INITIAL_COUNT"));
        }
        catch (const std::invalid_argument& e)
        {
            return 0;
        }
    }
}
