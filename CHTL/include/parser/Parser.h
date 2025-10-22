#pragma once

#include "lexer/Lexer.h"
#include "AstNode.h"
#include "lexer/Token.h"
#include "loader/Loader.h"
#include "bridge/SaltBridge.h"
#include "bridge/ConcreteSaltBridge.h"
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

namespace CHTL
{
    /**
     * @class Parser
     * @brief The Parser class is responsible for transforming a stream of tokens from the Lexer
     *        into an Abstract Syntax Tree (AST).
     *
     * It uses a Pratt (top-down operator precedence) parser for expressions,
     * which makes it easy to handle operator precedence and associativity.
     */
    class Parser
    {
    public:
        /**
         * @brief Constructs a Parser.
         * @param lexer A reference to the Lexer that provides the tokens.
         * @param file_path The path to the file being parsed, used for error reporting.
         */
        Parser(Lexer& lexer, std::string file_path = "");

        /**
         * @brief Sets the SaltBridge for communication with the CHTL JS compiler.
         * @param bridge A shared pointer to the SaltBridge instance.
         */
        void SetBridge(std::shared_ptr<SaltBridge> bridge) { m_bridge = bridge; }

        /**
         * @brief Parses the entire input from the lexer and returns the root of the AST.
         * @return A unique pointer to the ProgramNode, which is the root of the AST.
         */
        std::unique_ptr<ProgramNode> ParseProgram();

        /**
         * @brief Returns a list of parsing errors.
         * @return A const reference to a vector of error message strings.
         */
        const std::vector<std::string>& GetErrors() const { return m_errors; }

        /**
         * @brief Resets the set of parsed files. This is primarily for testing purposes
         *        to ensure a clean state between test runs.
         */
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

        // Statement parsing methods
        std::unique_ptr<AstNode> parseStatement();
        std::unique_ptr<ElementNode> parseElementNode();
        std::unique_ptr<TextNode> parseTextNode();
        std::unique_ptr<StyleNode> parseStyleNode();
        std::unique_ptr<CommentNode> parseCommentNode();
        std::unique_ptr<ScriptNode> parseScriptNode();
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
        std::unique_ptr<ExceptNode> parseExceptNode();
        std::unique_ptr<AstNode> parseIfChain();
        std::unique_ptr<ElseNode> parseElseBlock();
        Attribute parseAttribute();

        // Expression parsing methods
        std::unique_ptr<Expression> parseExpression(Precedence precedence);
        std::unique_ptr<Expression> parseIdentifier();
        std::unique_ptr<Expression> parseNumberLiteral();
        std::unique_ptr<Expression> parseStringLiteral();
        std::unique_ptr<Expression> parseVariableAccessExpression();
        std::unique_ptr<Expression> parseAttributeAccessExpression();
        std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
        std::unique_ptr<Expression> parseConditionalExpression(std::unique_ptr<Expression> condition);

        Lexer& m_lexer;
        Token m_currentToken;
        Token m_peekToken;
        std::vector<std::string> m_errors;
        std::string m_current_file_path;
        std::string m_current_namespace;
        std::shared_ptr<SaltBridge> m_bridge;

        // Static member to track parsed files across all parser instances
        // to prevent circular dependencies.
        static std::unordered_set<std::string> s_parsed_files;
    };

    extern const std::string GLOBAL_NAMESPACE;

} // namespace CHTL
