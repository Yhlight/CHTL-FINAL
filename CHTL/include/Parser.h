#pragma once

#include "Lexer.h"
#include "AstNode.h"
#include "Token.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL
{
    class Parser
    {
    public:
        Parser(Lexer& lexer);

        std::unique_ptr<ProgramNode> ParseProgram();
        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        void nextToken();
        bool expectPeek(TokenType t);

        std::unique_ptr<AstNode> parseStatement();
        std::unique_ptr<ElementNode> parseElementNode();
        std::unique_ptr<TextNode> parseTextNode();
        Attribute parseAttribute();

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors; // 用于收集解析错误
    };

} // namespace CHTL
