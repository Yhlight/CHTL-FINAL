#pragma once

#include "config/ConfigLexer.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL
{
    // Represents a single keyword definition, e.g., "[", "Template", "]"
    using KeywordRule = std::vector<Token>;

    class ConfigParser
    {
    public:
        ConfigParser(ConfigLexer& lexer);

        // Parses a single keyword definition like "[Template]"
        KeywordRule ParseRule();

        // Parses a group of options like "[@Style, @style, @CSS]"
        std::vector<KeywordRule> ParseRuleGroup();

        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        void nextToken();

        ConfigLexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors;
    };

} // namespace CHTL
