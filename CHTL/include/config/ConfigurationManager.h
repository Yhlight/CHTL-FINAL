#pragma once

#include "nodes/ConfigurationNode.h"
#include "lexer/Token.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL
{
    class ConfigurationManager
    {
    public:
        static ConfigurationManager& GetInstance();

        // Prohibit copy and assignment
        ConfigurationManager(const ConfigurationManager&) = delete;
        void operator=(const ConfigurationManager&) = delete;

        // Registers a configuration. Returns an error message if registration fails (e.g., duplicate name or multiple defaults).
        std::string RegisterConfiguration(std::unique_ptr<ConfigurationNode> config);

        // Sets the active configuration group for the current compilation unit.
        bool SetActiveConfiguration(const std::string& name);

        // Retrieves the currently active configuration. Returns the default config if no named one is active.
        const ConfigurationNode* GetActiveConfiguration() const;

        // Gets the keyword map from the active configuration. Returns default keywords if none active.
        std::unordered_map<std::string, std::string> GetActiveKeywords() const;

        // Clears all stored configurations. Useful for testing.
        void Reset();

    private:
        ConfigurationManager() = default;

        std::unordered_map<std::string, std::unique_ptr<ConfigurationNode>> m_named_configs;
        std::unique_ptr<ConfigurationNode> m_unnamed_config;
        const ConfigurationNode* m_active_config = nullptr;
    };
}