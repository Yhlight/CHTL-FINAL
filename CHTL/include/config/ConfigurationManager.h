#ifndef CHTL_CONFIGURATIONMANAGER_H
#define CHTL_CONFIGURATIONMANAGER_H

#include <map>
#include <string>
#include <vector>

namespace CHTL
{

    class ConfigurationManager
    {
    public:
        // Delete copy constructor and assignment operator
        ConfigurationManager(const ConfigurationManager &) = delete;
        ConfigurationManager &operator=(const ConfigurationManager &) = delete;

        static ConfigurationManager &GetInstance();

        void AddKeywordMapping(const std::string &configName, const std::string &internalName, const std::string &customLiteral);
        bool SetActiveConfiguration(const std::string &configName);
        std::string GetKeyword(const std::string &internalName) const;

        // For testing purposes
        static void Reset();

    private:
        ConfigurationManager() = default;

        static ConfigurationManager s_instance;

        // map<ConfigName, map<InternalKeyword, CustomLiteral>>
        std::map<std::string, std::map<std::string, std::string>> m_configurations;
        std::string m_activeConfigName = "default";
    };

}

#endif // CHTL_CONFIGURATIONMANAGER_H
