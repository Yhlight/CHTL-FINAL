#pragma once

#include "CHTLJS/include/lexer/Lexer.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTLJS
{
    class Parser
    {
    public:
        Parser(Lexer& lexer);

        std::unique_ptr<ProgramNode> ParseProgram();
        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        void nextToken();

        std::unique_ptr<AstNode> parseStatement();
        std::unique_ptr<EnhancedSelectorNode> parseEnhancedSelector();

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors;
    };

} // namespace CHTLJS
