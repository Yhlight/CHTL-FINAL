#pragma once

#include <string>
#include <unordered_map>

namespace CHTL
{
    struct ConfigurationNode; // Forward declaration to break circular dependency

    class Config
    {
    public:
        Config();
        void Load(const ConfigurationNode* config_node);
        bool IsLoaded() const { return m_loaded; }

        bool IsDebugMode() const;
        int GetIndexInitialCount() const;

    private:
        std::unordered_map<std::string, std::string> m_settings;
        bool m_loaded = false;
    };
}
