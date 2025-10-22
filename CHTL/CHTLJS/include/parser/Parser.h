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
    class Parser
    {
    public:
        Parser(Lexer& lexer);

        void SetBridge(std::shared_ptr<CHTL::SaltBridge> bridge) { m_bridge = bridge; }
        std::unique_ptr<ProgramNode> ParseProgram();
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


        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors;
        std::shared_ptr<CHTL::SaltBridge> m_bridge;
    };

} // namespace CHTLJS
