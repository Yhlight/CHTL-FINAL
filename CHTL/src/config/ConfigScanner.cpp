#include "config/ConfigScanner.h"
#include "config/ConfigurationManager.h"
#include "nodes/ConfigurationNode.h"
#include "nodes/NameConfigNode.h"
#include "nodes/UseNode.h"
#include "nodes/ImportNode.h"

namespace CHTL
{

ConfigScanner::ConfigScanner(Lexer& lexer) : m_lexer(lexer)
{
    nextToken();
    nextToken();
}

void ConfigScanner::nextToken()
{
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.NextToken();
}

bool ConfigScanner::expectPeek(TokenType t)
{
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    }
    m_errors.push_back("Expected next token to be " + TokenTypeToString(t) + ", got " + TokenTypeToString(m_peekToken.type) + " instead.");
    return false;
}

void ConfigScanner::Scan()
{
    while (m_currentToken.type != TokenType::END_OF_FILE)
    {
        if (m_currentToken.type == TokenType::KEYWORD_CONFIGURATION)
        {
            auto node = parseConfigurationStatement();
            if (node) {
                std::string err = ConfigurationManager::GetInstance().RegisterConfiguration(std::move(node));
                if (!err.empty()) m_errors.push_back(err);
            }
        }
        else if (m_currentToken.type == TokenType::KEYWORD_USE)
        {
            auto node = parseUseStatement();
            if (node && node->path.size() >= 2 && node->path[0] == "@Config") {
                if (!m_used_config_name.empty()) {
                    m_errors.push_back("Multiple 'use @Config' statements are not allowed.");
                }
                m_used_config_name = node->path[1];
            }
        }
        else if (m_currentToken.type == TokenType::KEYWORD_IMPORT)
        {
            auto node = parseConfigImportStatement();
            if (node) {
                m_config_imports.push_back(std::move(node));
            }
        }
        nextToken();
    }
}

std::unique_ptr<ImportNode> ConfigScanner::parseConfigImportStatement()
{
    bool is_config_import = false;
    if (m_peekToken.type == TokenType::KEYWORD_CONFIGURATION) {
        is_config_import = true;
    } else if (m_peekToken.type == TokenType::AT) {
        Lexer temp_lexer = m_lexer;
        temp_lexer.NextToken(); // Skip '@' from the lexer's perspective
        if (temp_lexer.NextToken().literal == "Config") {
            is_config_import = true;
        }
    }

    if (!is_config_import) {
        while(m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE) { nextToken(); }
        return nullptr;
    }

    nextToken(); // Consume [Import]
    auto node = std::make_unique<ImportNode>();

    // Consume until 'from'
    while(m_currentToken.type != TokenType::KEYWORD_FROM && m_currentToken.type != TokenType::END_OF_FILE) { nextToken(); }
    if (m_currentToken.type != TokenType::KEYWORD_FROM) { m_errors.push_back("Expected 'from' in import."); return nullptr; }
    nextToken(); // consume 'from'

    if (m_currentToken.type != TokenType::STRING) { m_errors.push_back("Expected path string in import."); return nullptr; }
    node->path = m_currentToken.literal;

    // Consume until semicolon
    while(m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE) { nextToken(); }

    return node;
}

std::unique_ptr<UseNode> ConfigScanner::parseUseStatement()
{
    auto node = std::make_unique<UseNode>();
    nextToken(); // consume 'use'
    while (m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE) {
        if(m_currentToken.type == TokenType::AT) node->path.push_back("@");
        else node->path.push_back(m_currentToken.literal);
        nextToken();
    }
    return node;
}

std::unique_ptr<ConfigurationNode> ConfigScanner::parseConfigurationStatement()
{
    auto node = std::make_unique<ConfigurationNode>();
    if (m_peekToken.type == TokenType::AT) {
        nextToken();
        if (m_peekToken.type != TokenType::IDENT || m_peekToken.literal != "Config") {
             m_errors.push_back("Expected 'Config' keyword after '@' for named configuration.");
             return nullptr;
        }
        nextToken();
        if (m_peekToken.type != TokenType::IDENT) {
            m_errors.push_back("Expected configuration name after '@Config'.");
            return nullptr;
        }
        nextToken();
        node->name = m_currentToken.literal;
    }
    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();
    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
        if (m_currentToken.type == TokenType::KEYWORD_NAME) {
            node->name_config = parseNameConfigNode();
        } else if (m_currentToken.type == TokenType::IDENT) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::COLON)) return nullptr;
            nextToken();
            if (m_currentToken.type == TokenType::IDENT || m_currentToken.type == TokenType::NUMBER || m_currentToken.type == TokenType::STRING) {
                node->settings[key] = m_currentToken.literal;
            } else { return nullptr; }
            if (m_peekToken.type == TokenType::SEMICOLON) nextToken();
        }
        nextToken();
    }
    return node;
}

std::unique_ptr<NameConfigNode> ConfigScanner::parseNameConfigNode()
{
    auto node = std::make_unique<NameConfigNode>();
    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();
    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
        if (m_currentToken.type == TokenType::IDENT) {
            std::string key = m_currentToken.literal;
            if (!expectPeek(TokenType::COLON)) return nullptr;
            nextToken();
            if (m_currentToken.type == TokenType::IDENT) {
                node->settings[key] = m_currentToken.literal;
            } else { return nullptr; }
            if (m_peekToken.type == TokenType::SEMICOLON) nextToken();
        }
        nextToken();
    }
    return node;
}

}