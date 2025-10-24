#pragma once

#include "CHTLJS/include/lexer/Lexer.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "bridge/SaltBridge.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTLJS
{
    /**
     * @class Parser
     * @brief Parses CHTL JS tokens and constructs an Abstract Syntax Tree (AST).
     */
    class Parser
    {
    public:
        /**
         * @brief Constructs a Parser with a given lexer.
         * @param lexer The lexer that provides the tokens.
         */
        Parser(Lexer& lexer);

        /**
         * @brief Sets the SaltBridge for communication with the CHTL compiler.
         * @param bridge The SaltBridge instance.
         */
        void SetBridge(std::shared_ptr<CHTL::SaltBridge> bridge) { m_bridge = bridge; }

        /**
         * @brief Parses the entire program and returns the root of the AST.
         * @return A unique pointer to the ProgramNode.
         */
        std::unique_ptr<ProgramNode> ParseProgram();

        /**
         * @brief Gets the list of parsing errors.
         * @return A constant reference to the vector of error strings.
         */
        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        void nextToken();

        std::unique_ptr<AstNode> parseStatement();
        std::unique_ptr<EnhancedSelectorNode> parseEnhancedSelector();
        std::unique_ptr<AstNode> parseScriptLoader();
        std::unique_ptr<AstNode> parseListen();
        std::unique_ptr<AstNode> parseDelegate();
        std::unique_ptr<AstNode> parseAnimate();
        std::unique_ptr<AstNode> parseVir();
        std::unique_ptr<AstNode> parseEventBind();
        std::unique_ptr<AstNode> parseRouter();
        std::unique_ptr<AstNode> parseReactiveValue();

        // Helper functions
        bool expectToken(TokenType type);
        bool peekTokenIs(TokenType type) const;
        std::string parseStringOrIdent();

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors;
        std::shared_ptr<CHTL::SaltBridge> m_bridge;
    };

} // namespace CHTLJS
