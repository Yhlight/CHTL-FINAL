#include "Compiler.h"
#include "loader/Loader.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "config/ConfigScanner.h"
#include "config/ConfigurationManager.h"
#include "lexer/Token.h"
#include <algorithm>
#include <filesystem>
#include <queue>

namespace CHTL
{
    Compiler::Compiler()
    {
        ConfigurationManager::GetInstance().Reset();
    }

    std::unique_ptr<ProgramNode> Compiler::Compile(const std::string& file_path)
    {
        m_errors.clear();
        m_scanned_files.clear();
        ConfigurationManager::GetInstance().Reset();

        // Phase 1
        ScanForConfigs(file_path);
        if (!m_errors.empty()) return nullptr;

        // Phase 2
        std::string content;
        try {
            content = Loader::ReadFile("", file_path);
        } catch (const std::runtime_error& e) {
            m_errors.push_back(e.what());
            return nullptr;
        }

        auto default_keywords = GetDefaultKeywords();

        Lexer scanner_lexer(content, default_keywords);
        ConfigScanner main_scanner(scanner_lexer);
        main_scanner.Scan();

        std::string used_config = main_scanner.GetUsedConfigurationName();
        if (!used_config.empty()) {
            if (!ConfigurationManager::GetInstance().SetActiveConfiguration(used_config)) {
                m_errors.push_back("Configuration '" + used_config + "' specified in 'use' statement was not found.");
                return nullptr;
            }
        }

        auto final_keywords = default_keywords;
        auto custom_keywords_str = ConfigurationManager::GetInstance().GetActiveKeywords();

        if (!custom_keywords_str.empty()) {
            std::unordered_map<std::string, std::string> config_key_to_old_literal;
            for(const auto& pair : default_keywords) {
                 config_key_to_old_literal[TokenTypeToString(pair.second.type)] = pair.first;
            }

            for(const auto& pair : custom_keywords_str) {
                const std::string& config_key = pair.first; // e.g., "KEYWORD_STYLE"
                const std::string& new_literal = pair.second; // e.g., "estilo"

                if(config_key_to_old_literal.count(config_key)) {
                    std::string old_literal = config_key_to_old_literal.at(config_key); // e.g., "style"

                    if(final_keywords.count(old_literal)) {
                        TokenType type = final_keywords.at(old_literal).type;
                        final_keywords.erase(old_literal);
                        final_keywords[new_literal] = {type, new_literal};
                    }
                }
            }
        }

        Lexer phase2_lexer(content, final_keywords);
        Parser parser(phase2_lexer, final_keywords, file_path);
        auto program = parser.ParseProgram();

        m_errors.insert(m_errors.end(), parser.GetErrors().begin(), parser.GetErrors().end());
        if (!m_errors.empty()) return nullptr;
        return program;
    }

    void Compiler::ScanForConfigs(const std::string& file_path)
    {
        std::queue<std::string> files_to_scan;
        files_to_scan.push(file_path);

        while(!files_to_scan.empty())
        {
            std::string current_path_str = files_to_scan.front();
            files_to_scan.pop();

            std::filesystem::path canonical_path = std::filesystem::weakly_canonical(current_path_str);
            if (m_scanned_files.count(canonical_path.string())) continue;
            m_scanned_files.insert(canonical_path.string());

            std::string content;
            try {
                content = Loader::ReadFile("", current_path_str);
            } catch (const std::runtime_error& e) {
                m_errors.push_back(e.what());
                continue;
            }

            Lexer lexer(content, GetDefaultKeywords());
            ConfigScanner scanner(lexer);
            scanner.Scan();

            for(const auto& err : scanner.GetErrors()) m_errors.push_back(err);

            auto imports = scanner.GetConfigImports();
            for (auto& import_node : imports) {
                 std::filesystem::path parent_dir = canonical_path.parent_path();
                 files_to_scan.push((parent_dir / import_node->path).string());
            }
        }
    }
}