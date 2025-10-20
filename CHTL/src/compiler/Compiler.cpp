#include "compiler/Compiler.h"
#include "loader/Loader.h"
#include "config/ConfigScanner.h"
#include "config/ConfigurationManager.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "lexer/Token.h"
#include <iostream>

namespace CHTL
{
    namespace
    {
        // Helper function to build the final keyword map for the lexer.
        // It starts with the default keywords and then overrides them with any
        // custom keywords defined in the active configuration.
        std::map<std::string, Token> BuildKeywordMap()
        {
            auto keywords = GetDefaultKeywords();
            auto& configManager = ConfigurationManager::GetInstance();

            // This is a simplified approach. A more robust implementation would
            // get the active config's keyword map. For now, we assume a single
            // "spanish" config might exist for demonstration.
            // Let's create a map from internal name to custom literal for the active config.

            // This part is tricky. ConfigurationManager doesn't directly expose the maps.
            // Let's adjust the logic. We can iterate through the default keywords and ask
            // the config manager for the custom literal for each one.

            std::map<std::string, std::string> internalToCustom;
            for(const auto& pair : keywords) {
                std::string internalName = pair.second.literal;
                std::string customLiteral = configManager.GetKeyword(internalName);
                if (internalName != customLiteral) {
                    internalToCustom[internalName] = customLiteral;
                }
            }

            // Now, create the final map from custom literal to Token
            std::map<std::string, Token> finalKeywords;
            for(const auto& pair : keywords) {
                std::string internalName = pair.second.literal;
                Token token = pair.second;
                if (internalToCustom.count(internalName)) {
                    finalKeywords[internalToCustom[internalName]] = token;
                } else {
                    finalKeywords[internalName] = token;
                }
            }

            return finalKeywords;
        }
    }

    void Compiler::Reset()
    {
        ConfigurationManager::Reset();
        Parser::ResetParsedFiles();
        m_errors.clear();
    }

    std::unique_ptr<ProgramNode> Compiler::Compile(const std::string& filepath)
    {
        m_errors.clear();

        // Load source file
        std::string source;
        try {
            source = Loader::ReadFile("", filepath); // Base path is empty for the main file
        } catch (const std::runtime_error& e) {
            // Handle file loading error
            std::cerr << "Error loading file: " << e.what() << std::endl;
            return nullptr;
        }

        // 1. First Pass: Scan for configurations
        ConfigScanner configScanner(source);
        configScanner.Scan();

        // The parser will also need to know how to handle 'use' statements
        // to switch configurations. For now, we assume a default or the first found config.
        // Let's assume a 'use' statement might set the active config.
        // We'll need to parse for that too. This suggests the Compiler might need a mini-parser.
        // For now, let's keep it simple and assume the active config is set by the end of the scan pass.
        // The integration test will manually set it.

        // 2. Build the keyword map for the lexer
        auto keywordMap = BuildKeywordMap();

        // 3. Second Pass: Main parsing
        Lexer lexer(source, keywordMap);
        Parser parser(lexer, filepath);

        auto program = parser.ParseProgram();

        if (!parser.GetErrors().empty()) {
            m_errors = parser.GetErrors();
        }

        return program;
    }

}
