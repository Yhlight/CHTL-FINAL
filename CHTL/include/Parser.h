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
        // Pratt Parser related types
        using prefixParseFn = std::unique_ptr<Expression> (Parser::*)();
        using infixParseFn = std::unique_ptr<Expression> (Parser::*)(std::unique_ptr<Expression>);

        enum Precedence
        {
            LOWEST = 0,
            SUM,     // + -
            PRODUCT, // * /
            PREFIX,  // -X or !X
        };

        static std::unordered_map<TokenType, Precedence> precedences;

        void nextToken();
        bool expectPeek(TokenType t);

        // Statement parsing
        std::unique_ptr<AstNode> parseStatement();
        std::unique_ptr<ElementNode> parseElementNode();
        std::unique_ptr<TextNode> parseTextNode();
        std::unique_ptr<StyleNode> parseStyleNode();
        Attribute parseAttribute();

        // Expression parsing
        std::unique_ptr<Expression> parseExpression(Precedence precedence);
        std::unique_ptr<Expression> parseIdentifier();
        std::unique_ptr<Expression> parseNumberLiteral();
        std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors; // 用于收集解析错误
    };

} // namespace CHTL
