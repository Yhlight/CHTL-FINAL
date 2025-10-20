#pragma once

#include "lexer/Lexer.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL
{
    // Forward declarations
    struct ConfigurationNode;
    struct NameConfigNode;
    struct UseNode;
    struct ImportNode;

    // Scans a single file content for configurations, use statements, and config import paths.
    // It does not perform recursive scanning.
    class ConfigScanner
    {
    public:
        ConfigScanner(Lexer& lexer);

        // Runs the scan on the provided lexer's content.
        void Scan();

        // Data gathered after scanning
        std::string GetUsedConfigurationName() const { return m_used_config_name; }
        std::vector<std::unique_ptr<ImportNode>>&& GetConfigImports() { return std::move(m_config_imports); }
        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        void nextToken();
        bool expectPeek(TokenType t);

        std::unique_ptr<ConfigurationNode> parseConfigurationStatement();
        std::unique_ptr<NameConfigNode> parseNameConfigNode();
        std::unique_ptr<UseNode> parseUseStatement();
        std::unique_ptr<ImportNode> parseConfigImportStatement();

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::string m_used_config_name;
        std::vector<std::unique_ptr<ImportNode>> m_config_imports;
        std::vector<std::string> m_errors;
    };
}