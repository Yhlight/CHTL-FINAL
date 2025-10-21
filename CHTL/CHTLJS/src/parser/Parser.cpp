#include "CHTLJS/include/parser/Parser.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#include "CHTLJS/include/lexer/Token.h"

namespace CHTLJS
{
    Parser::Parser(Lexer& lexer)
        : m_lexer(lexer), m_bridge(nullptr)
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
        if (m_currentToken.type == TokenType::LBRACE_BRACE) {
            return parseEnhancedSelector();
        } else if (m_currentToken.literal == "ScriptLoader") {
            return parseScriptLoader();
        } else if (m_currentToken.literal == "Listen") {
            return parseListen();
        } else if (m_currentToken.literal == "Delegate") {
            return parseDelegate();
        } else if (m_currentToken.literal == "Animate") {
            return parseAnimate();
        } else if (m_currentToken.literal == "Vir") {
            return parseVir();
        } else if (m_currentToken.type == TokenType::RAW_JS) {
            auto node = std::make_unique<RawJSNode>();
            node->content = m_currentToken.literal;
            nextToken();
            return node;
        }
        return nullptr;
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
    std::unique_ptr<AstNode> Parser::parseScriptLoader()
    {
        auto node = std::make_unique<ScriptLoaderNode>();
        nextToken(); // consume 'ScriptLoader'
        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for ScriptLoader block.");
            return nullptr;
        }
        nextToken(); // consume '{'
        while (m_currentToken.literal == "load") {
            nextToken(); // consume 'load'
            if (m_currentToken.type != TokenType::COLON) {
                 m_errors.push_back("Expected ':' after 'load' keyword.");
                 return nullptr;
            }
            nextToken(); // consume ':'
            while(m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::RBRACE) {
                node->paths.push_back(m_currentToken.literal);
                nextToken();
                if(m_currentToken.type == TokenType::COMMA) {
                    nextToken();
                }
            }
            if(m_currentToken.type == TokenType::SEMICOLON) {
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close ScriptLoader block.");
            return nullptr;
        }
        nextToken();
        return node;
    }

    std::unique_ptr<AstNode> Parser::parseListen()
    {
        auto node = std::make_unique<ListenNode>();
        nextToken(); // consume 'Listen'
        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for Listen block.");
            return nullptr;
        }
        nextToken(); // consume '{'
        while (m_currentToken.type != TokenType::RBRACE) {
            std::string event_name = m_currentToken.literal;
            nextToken();
            if (m_currentToken.type != TokenType::COLON) {
                m_errors.push_back("Expected ':' after event name.");
                return nullptr;
            }
            nextToken();
            node->events[event_name] = m_currentToken.literal;
            nextToken();
            if(m_currentToken.type == TokenType::COMMA) {
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close Listen block.");
            return nullptr;
        }
        nextToken();
        return node;
    }

    std::unique_ptr<AstNode> Parser::parseDelegate()
    {
        auto node = std::make_unique<DelegateNode>();
        nextToken(); // consume 'Delegate'
        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for Delegate block.");
            return nullptr;
        }
        nextToken(); // consume '{'

        while (m_currentToken.type != TokenType::RBRACE) {
            if (m_currentToken.literal == "target") {
                nextToken();
                if (m_currentToken.type != TokenType::COLON) {
                    m_errors.push_back("Expected ':' after 'target' keyword.");
                    return nullptr;
                }
                nextToken();
                if (m_currentToken.type == TokenType::LBRACKET) {
                    nextToken();
                    while (m_currentToken.type != TokenType::RBRACKET) {
                        node->targets.push_back(m_currentToken.literal);
                        nextToken();
                        if (m_currentToken.type == TokenType::COMMA) {
                            nextToken();
                        }
                    }
                    nextToken();
                } else {
                    node->targets.push_back(m_currentToken.literal);
                    nextToken();
                }
            } else {
                std::string event_name = m_currentToken.literal;
                nextToken();
                if (m_currentToken.type != TokenType::COLON) {
                    m_errors.push_back("Expected ':' after event name.");
                    return nullptr;
                }
                nextToken();
                node->events[event_name] = m_currentToken.literal;
                nextToken();
            }
             if(m_currentToken.type == TokenType::COMMA) {
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close Delegate block.");
            return nullptr;
        }
        nextToken();
        return node;
    }

    std::unique_ptr<AstNode> Parser::parseAnimate()
    {
        auto node = std::make_unique<AnimateNode>();
        nextToken();
        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for Animate block.");
            return nullptr;
        }
        nextToken();

        while (m_currentToken.type != TokenType::RBRACE) {
            std::string key = m_currentToken.literal;
            nextToken();
            if (m_currentToken.type != TokenType::COLON) {
                m_errors.push_back("Expected ':' after key in Animate block.");
                return nullptr;
            }
            nextToken();
            if (key == "target") node->target = m_currentToken.literal;
            else if (key == "duration") node->duration = std::stoi(m_currentToken.literal);
            else if (key == "easing") node->easing = m_currentToken.literal;
            else if (key == "loop") node->loop = std::stoi(m_currentToken.literal);
            else if (key == "direction") node->direction = m_currentToken.literal;
            else if (key == "delay") node->delay = std::stoi(m_currentToken.literal);
            else if (key == "callback") node->callback = m_currentToken.literal;
            else if (key == "begin" || key == "end") {
                if (m_currentToken.type != TokenType::LBRACE) {
                    m_errors.push_back("Expected '{' for state block.");
                    return nullptr;
                }
                nextToken();
                std::unordered_map<std::string, std::string> state;
                while (m_currentToken.type != TokenType::RBRACE) {
                    std::string prop = m_currentToken.literal;
                    nextToken();
                    if (m_currentToken.type != TokenType::COLON) {
                        m_errors.push_back("Expected ':' after property name.");
                        return nullptr;
                    }
                    nextToken();
                    state[prop] = m_currentToken.literal;
                    nextToken();
                    if (m_currentToken.type == TokenType::COMMA) nextToken();
                }
                if (key == "begin") node->begin_state = state;
                else node->end_state = state;
                nextToken();
            } else if (key == "when") {
                 if (m_currentToken.type != TokenType::LBRACKET) {
                    m_errors.push_back("Expected '[' for when block.");
                    return nullptr;
                }
                nextToken();
                while(m_currentToken.type != TokenType::RBRACKET) {
                    if (m_currentToken.type != TokenType::LBRACE) {
                         m_errors.push_back("Expected '{' for when state.");
                        return nullptr;
                    }
                    nextToken();
                    std::unordered_map<std::string, std::string> state;
                    while (m_currentToken.type != TokenType::RBRACE) {
                        std::string prop = m_currentToken.literal;
                        nextToken();
                        if (m_currentToken.type != TokenType::COLON) {
                            m_errors.push_back("Expected ':' after property name.");
                            return nullptr;
                        }
                        nextToken();
                        state[prop] = m_currentToken.literal;
                        nextToken();
                        if (m_currentToken.type == TokenType::COMMA) nextToken();
                    }
                    node->when_states.push_back(state);
                    nextToken();
                    if (m_currentToken.type == TokenType::COMMA) nextToken();
                }
                nextToken();
            }
            if (m_currentToken.type == TokenType::COMMA) {
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close Animate block.");
            return nullptr;
        }
        nextToken();
        return node;
    }

    std::unique_ptr<AstNode> Parser::parseVir()
    {
        auto node = std::make_unique<VirNode>();
        nextToken(); // consume 'Vir'
        node->name = m_currentToken.literal;
        nextToken();
        if (m_currentToken.type != TokenType::ASSIGN) {
            m_errors.push_back("Expected '=' after Vir name.");
            return nullptr;
        }
        nextToken(); // consume '='

        // The right-hand side of a Vir assignment can be a block or another CHTL JS function call.
        // For simplicity, we'll just record the tokens until the semicolon.
        std::string rhs;
        while(m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE)
        {
            rhs += m_currentToken.literal;
            if (m_peekToken.type != TokenType::SEMICOLON) {
                rhs += " ";
            }
            nextToken();
        }
        // At this point, rhs holds the string representation of the right-hand side.
        // A more advanced implementation would parse this into a proper expression tree.
        // For now, we'll just store it as a single property.
        node->properties["value"] = rhs;

        if (m_currentToken.type != TokenType::SEMICOLON) {
             m_errors.push_back("Expected ';' to end Vir declaration.");
        } else {
             nextToken(); // consume ';'
        }

        return node;
    }
}
