#include "CHTLJS/include/parser/Parser.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/lexer/Token.h"

namespace CHTLJS
{
    Parser::Parser(Lexer& lexer)
        : m_lexer(lexer)
    {
        nextToken();
        nextToken();
    }

    void Parser::nextToken()
    {
        m_currentToken = m_peekToken;
        m_peekToken = m_lexer.NextToken();
    }

    std::unique_ptr<ProgramNode> Parser::ParseProgram()
    {
        auto program = std::make_unique<ProgramNode>();

        while (m_currentToken.type != TokenType::END_OF_FILE)
        {
            auto stmt = parseStatement();
            if (stmt)
            {
                program->children.push_back(std::move(stmt));
            }
        }
        return program;
    }

    std::unique_ptr<AstNode> Parser::parseStatement()
    {
        switch (m_currentToken.type)
        {
            case TokenType::LBRACE_BRACE:
                return parseEnhancedSelector();
            case TokenType::RAW_JS:
            {
                auto node = std::make_unique<RawJSNode>();
                node->content = m_currentToken.literal;
                nextToken(); // Consume the RAW_JS token
                return node;
            }
            default:
                // If we are at EOF or see something unexpected, we stop.
                return nullptr;
        }
    }

    std::unique_ptr<EnhancedSelectorNode> Parser::parseEnhancedSelector()
    {
        auto node = std::make_unique<EnhancedSelectorNode>();

        // m_currentToken is LBRACE_BRACE, consume it
        nextToken();

        std::string selector_content;
        while (m_currentToken.type != TokenType::RBRACE_BRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            selector_content += m_currentToken.literal;
            nextToken();
        }

        // Trim leading/trailing whitespace from the collected content
        size_t first = selector_content.find_first_not_of(" \t\n\r");
        if (std::string::npos == first) {
            node->selector = "";
        } else {
            size_t last = selector_content.find_last_not_of(" \t\n\r");
            node->selector = selector_content.substr(first, (last - first + 1));
        }

        if (m_currentToken.type != TokenType::RBRACE_BRACE) {
            m_errors.push_back("Expected '}}' to close enhanced selector, but got EOF.");
            return nullptr;
        }

        // Consume RBRACE_BRACE
        nextToken();

        return node;
    }

}
