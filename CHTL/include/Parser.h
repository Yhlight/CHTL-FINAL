#pragma once

#include "Lexer.h"
#include "AstNode.h"
#include "Token.h"
#include "Loader.h"
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

namespace CHTL
{
    class Parser
    {
    public:
        Parser(Lexer& lexer, std::string file_path = "");

        std::unique_ptr<ProgramNode> ParseProgram();
        const std::vector<std::string>& GetErrors() const { return m_errors; }

        // For testing purposes
        static void ResetParsedFiles() { s_parsed_files.clear(); }

    private:
        // Pratt Parser related types
        using prefixParseFn = std::unique_ptr<Expression> (Parser::*)();
        using infixParseFn = std::unique_ptr<Expression> (Parser::*)(std::unique_ptr<Expression>);

        enum Precedence
        {
            LOWEST = 0,
            CONDITIONAL, // ? :
            COMPARE, // > or <
            SUM,     // + -
            PRODUCT, // * /
            POWER,   // **
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
        std::unique_ptr<CommentNode> parseCommentNode();
        std::unique_ptr<StyleRuleNode> parseStyleRuleNode();
        std::unique_ptr<StyleProperty> parseStyleProperty();
        std::unique_ptr<TemplateDefinitionNode> parseTemplateDefinition();
        std::unique_ptr<CustomDefinitionNode> parseCustomDefinitionNode();
        std::unique_ptr<ImportNode> parseImportNode(ProgramNode& program);
        std::unique_ptr<OriginNode> parseOriginNode();
        std::unique_ptr<NamespaceNode> parseNamespaceNode(ProgramNode& program);
        std::unique_ptr<ConfigurationNode> parseConfigurationStatement();
        std::unique_ptr<NameConfigNode> parseNameConfigNode();
        std::unique_ptr<UseNode> parseUseStatement();
        std::unique_ptr<AstNode> parseAtUsage();
        std::unique_ptr<DeleteSpecializationNode> parseDeleteSpecialization();
        std::unique_ptr<InsertSpecializationNode> parseInsertSpecialization();
        Attribute parseAttribute();

        // Expression parsing
        std::unique_ptr<Expression> parseExpression(Precedence precedence);
        std::unique_ptr<Expression> parseIdentifier();
        std::unique_ptr<Expression> parseNumberLiteral();
        std::unique_ptr<Expression> parseStringLiteral();
        std::unique_ptr<Expression> parseVariableAccessExpression();
        std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
        std::unique_ptr<Expression> parseConditionalExpression(std::unique_ptr<Expression> condition);

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors;
        std::string m_current_file_path;
        std::string m_current_namespace;

        // Static member to track parsed files across all parser instances
        // to prevent circular dependencies.
        static std::unordered_set<std::string> s_parsed_files;
    };

    extern const std::string GLOBAL_NAMESPACE;

} // namespace CHTL
