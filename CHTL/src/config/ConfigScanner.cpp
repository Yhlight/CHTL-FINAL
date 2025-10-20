#include "config/ConfigScanner.h"
#include "config/ConfigurationManager.h"
#include <cctype>
#include <stdexcept>

namespace CHTL
{

    ConfigScanner::ConfigScanner(const std::string &source) : m_source(source) {}

    void ConfigScanner::Scan()
    {
        while (m_cursor < m_source.length())
        {
            SkipWhitespace();
            if (Match("[Configuration]"))
            {
                ParseConfigurationBlock();
            }
            else
            {
                // If not a config block, just advance cursor to avoid infinite loops
                if (m_cursor < m_source.length())
                {
                    m_cursor++;
                }
            }
        }
    }

    void ConfigScanner::ParseConfigurationBlock()
    {
        SkipWhitespace();
        if (m_cursor >= m_source.length() || m_source[m_cursor] != '{') {
            // No block to parse, or malformed.
            return;
        }
        m_cursor++; // Consume '{'
        SkipWhitespace();

        while (m_cursor < m_source.length() && m_source[m_cursor] != '}')
        {
            if (Match("[Name:"))
            {
                ParseNameBlock();
            }
            else
            {
                m_cursor++;
            }
            SkipWhitespace();
        }

        if (m_cursor < m_source.length() && m_source[m_cursor] == '}') {
            m_cursor++; // Consume '}'
        }
    }

    void ConfigScanner::ParseNameBlock()
    {
        SkipWhitespace();
        std::string configName = ParseIdentifier();
        SkipWhitespace();
        if (!Match("]"))
        {
            // Error: expected closing bracket for Name block
            return;
        }

        SkipWhitespace();
        while (m_cursor < m_source.length() && m_source[m_cursor] != '[' && m_source[m_cursor] != '}')
        {
            SkipWhitespace();
            if (m_cursor >= m_source.length() || m_source[m_cursor] == '[' || m_source[m_cursor] == '}') break;

            std::string internalName = ParseIdentifier();
            SkipWhitespace();
            if (!Match("="))
            {
                // Error: expected '=' after keyword identifier
                break;
            }
            SkipWhitespace();
            std::string customLiteral = ParseStringLiteral();
            SkipWhitespace();

            if (!internalName.empty() && !customLiteral.empty())
            {
                ConfigurationManager::GetInstance().AddKeywordMapping(configName, internalName, customLiteral);
            }

            if (m_cursor < m_source.length() && m_source[m_cursor] == ';') {
                m_cursor++;
            }
        }
    }

    void ConfigScanner::SkipWhitespace()
    {
        while (m_cursor < m_source.length() && std::isspace(m_source[m_cursor]))
        {
            m_cursor++;
        }
    }

    bool ConfigScanner::Match(const std::string &expected)
    {
        if (m_source.substr(m_cursor, expected.length()) == expected)
        {
            m_cursor += expected.length();
            return true;
        }
        return false;
    }

    std::string ConfigScanner::ParseIdentifier()
    {
        size_t start = m_cursor;
        while (m_cursor < m_source.length() && (std::isalnum(m_source[m_cursor]) || m_source[m_cursor] == '_'))
        {
            m_cursor++;
        }
        return m_source.substr(start, m_cursor - start);
    }

    std::string ConfigScanner::ParseStringLiteral()
    {
        if (m_cursor >= m_source.length() || m_source[m_cursor] != '"')
        {
            return ""; // Or throw error
        }
        m_cursor++; // Consume opening quote
        size_t start = m_cursor;
        while (m_cursor < m_source.length() && m_source[m_cursor] != '"')
        {
            m_cursor++;
        }
        std::string literal = m_source.substr(start, m_cursor - start);
        if (m_cursor < m_source.length() && m_source[m_cursor] == '"')
        {
            m_cursor++; // Consume closing quote
        }
        return literal;
    }

}
