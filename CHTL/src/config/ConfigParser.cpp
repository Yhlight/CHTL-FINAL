#include "config/ConfigParser.h"

namespace CHTL
{
    ConfigParser::ConfigParser(ConfigLexer& lexer)
        : m_lexer(lexer)
    {
        // Initialize with two tokens to populate both current and peek
        nextToken();
        nextToken();
    }

    void ConfigParser::nextToken()
    {
        m_currentToken = m_peekToken;
        m_peekToken = m_lexer.NextToken();
    }

    KeywordRule ConfigParser::ParseRule()
    {
        KeywordRule rule;
        // A single rule is just a sequence of tokens until a comma or the end of the group (RBRACKET)
        while (m_currentToken.type != TokenType::COMMA &&
               m_currentToken.type != TokenType::RBRACKET &&
               m_currentToken.type != TokenType::END_OF_FILE)
        {
            rule.push_back(m_currentToken);
            nextToken();
        }
        return rule;
    }

    std::vector<KeywordRule> ConfigParser::ParseRuleGroup()
    {
        std::vector<KeywordRule> ruleGroup;

        if (m_currentToken.type != TokenType::LBRACKET)
        {
            // It's not a group, so parse it as a single rule.
            ruleGroup.push_back(ParseRule());
            return ruleGroup;
        }

        nextToken(); // Consume '['

        // Handle empty group like "[]"
        if (m_currentToken.type == TokenType::RBRACKET) {
            nextToken(); // Consume ']'
            return ruleGroup;
        }

        // Parse the first rule
        ruleGroup.push_back(ParseRule());

        // Continue parsing subsequent rules separated by commas
        while (m_currentToken.type == TokenType::COMMA)
        {
            nextToken(); // Consume ','
            ruleGroup.push_back(ParseRule());
        }

        // Expect a closing bracket
        if (m_currentToken.type != TokenType::RBRACKET)
        {
            m_errors.push_back("Expected ']' to end rule group.");
            return {};
        }
        nextToken(); // Consume ']'

        return ruleGroup;
    }

} // namespace CHTL
