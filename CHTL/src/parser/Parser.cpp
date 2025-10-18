#include "parser/Parser.h"
#include "loader/Loader.h"
#include "CHTLJS/include/lexer/Lexer.h"
#include "CHTLJS/include/parser/Parser.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <filesystem>

namespace CHTL
{
    const std::string GLOBAL_NAMESPACE = "_global";
    std::unordered_set<std::string> Parser::s_parsed_files;

    std::unordered_map<TokenType, Parser::Precedence> Parser::precedences = {
        {TokenType::PLUS,     Parser::Precedence::SUM},
        {TokenType::MINUS,    Parser::Precedence::SUM},
        {TokenType::ASTERISK, Parser::Precedence::PRODUCT},
        {TokenType::SLASH,    Parser::Precedence::PRODUCT},
        {TokenType::MODULO,   Parser::Precedence::PRODUCT},
        {TokenType::POWER,    Parser::Precedence::POWER},
        {TokenType::GT,       Parser::Precedence::COMPARE},
        {TokenType::LT,       Parser::Precedence::COMPARE},
        {TokenType::QUESTION, Parser::Precedence::CONDITIONAL},
    };

    Parser::Parser(Lexer& lexer, std::string file_path)
        : m_lexer(lexer), m_current_file_path(std::move(file_path)), m_current_namespace(GLOBAL_NAMESPACE)
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

        if (!m_current_file_path.empty()) {
            std::filesystem::path canonical_path = std::filesystem::weakly_canonical(m_current_file_path);
            s_parsed_files.insert(canonical_path.string());
        }

        while (m_currentToken.type != TokenType::END_OF_FILE)
        {
            std::unique_ptr<AstNode> stmt = nullptr;
            if (m_currentToken.type == TokenType::KEYWORD_TEMPLATE)
            {
                stmt = parseTemplateDefinition();
                if (stmt) {
                    auto* tmpl_def = static_cast<TemplateDefinitionNode*>(stmt.get());
                    program->templates[m_current_namespace][tmpl_def->name] = tmpl_def;
                }
            }
            else if (m_currentToken.type == TokenType::KEYWORD_CUSTOM)
            {
                stmt = parseCustomDefinitionNode();
                 if (stmt) {
                    auto* custom_def = static_cast<CustomDefinitionNode*>(stmt.get());
                    program->customs[m_current_namespace][custom_def->name] = custom_def;
                }
            }
            else if (m_currentToken.type == TokenType::KEYWORD_IMPORT)
            {
                stmt = parseImportNode(*program);
            }
            else if (m_currentToken.type == TokenType::KEYWORD_NAMESPACE)
            {
                stmt = parseNamespaceNode(*program);
            }
            else if (m_currentToken.type == TokenType::KEYWORD_CONFIGURATION)
            {
                stmt = parseConfigurationStatement();
            }
            else if (m_currentToken.type == TokenType::KEYWORD_USE)
            {
                stmt = parseUseStatement();
            }
            else
            {
                stmt = parseStatement();
            }

            if (stmt)
            {
                program->children.push_back(std::move(stmt));
            }
            nextToken();
        }
        return program;
    }

    bool Parser::expectPeek(TokenType t)
    {
        if (m_peekToken.type == t)
        {
            nextToken();
            return true;
        }
        else
        {
            m_errors.push_back("Expected next token to be " + TokenTypeToString(t) +
                               ", got " + TokenTypeToString(m_peekToken.type) + " instead.");
            return false;
        }
    }

    std::unique_ptr<AstNode> Parser::parseStatement()
    {
        if (m_currentToken.type == TokenType::IDENT)
        {
            if (m_peekToken.type == TokenType::COLON)
            {
                return parseStyleProperty();
            }
            return parseElementNode();
        }
        else if (m_currentToken.type == TokenType::KEYWORD_TEXT)
        {
             return parseTextNode();
        }
        else if (m_currentToken.type == TokenType::KEYWORD_STYLE)
        {
            return parseStyleNode();
        }
        else if (m_currentToken.type == TokenType::COMMENT)
        {
            return parseCommentNode();
        }
        else if (m_currentToken.type == TokenType::KEYWORD_ORIGIN)
        {
            return parseOriginNode();
        }
        else if (m_currentToken.type == TokenType::KEYWORD_SCRIPT)
        {
            return parseScriptNode();
        }
        return nullptr;
    }

    std::unique_ptr<ElementNode> Parser::parseElementNode()
    {
        auto node = std::make_unique<ElementNode>();
        node->tag_name = m_currentToken.literal;

        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }

        nextToken();

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::KEYWORD_TEXT && m_peekToken.type == TokenType::COLON)
            {
                nextToken();
                nextToken();

                if (m_currentToken.type != TokenType::STRING) {
                    m_errors.push_back("Expected string literal for text property.");
                    return nullptr;
                }

                auto text_node = std::make_unique<TextNode>();
                text_node->value = m_currentToken.literal;
                node->children.push_back(std::move(text_node));

                if (m_peekToken.type == TokenType::SEMICOLON) {
                    nextToken();
                }
            }
            else if (m_currentToken.type == TokenType::IDENT && m_peekToken.type == TokenType::COLON)
            {
                node->attributes.push_back(parseAttribute());
            }
            else if (m_currentToken.type == TokenType::AT)
            {
                node->children.push_back(parseAtUsage());
            }
            else if (m_currentToken.type == TokenType::KEYWORD_EXCEPT)
            {
                node->children.push_back(parseExceptNode());
            }
            else if (m_currentToken.type == TokenType::KEYWORD_IF)
            {
                node->children.push_back(parseIfChain());
            }
            else
            {
                auto stmt = parseStatement();
                if (stmt)
                {
                    node->children.push_back(std::move(stmt));
                }
            }

            nextToken();
        }

        if (m_currentToken.type != TokenType::RBRACE)
        {
            m_errors.push_back("Expected '}' to close element block.");
            return nullptr;
        }

        return node;
    }

    std::unique_ptr<AstNode> Parser::parseIfChain()
    {
        // Expects m_currentToken to be KEYWORD_IF
        auto if_node = std::make_unique<IfNode>();

        if (!expectPeek(TokenType::LBRACE)) { // consumes 'if', current is '{'
            m_errors.push_back("Expected '{' to start if block.");
            return nullptr;
        }
        nextToken(); // consumes '{', current is 'condition'

        if (m_currentToken.type != TokenType::IDENT || m_currentToken.literal != "condition") {
            m_errors.push_back("Expected 'condition' keyword in if block.");
            return nullptr;
        }

        if (!expectPeek(TokenType::COLON)) { // consumes 'condition', current is ':'
            m_errors.push_back("Expected ':' after 'condition'.");
            return nullptr;
        }
        nextToken(); // consumes ':', current is start of expression

        if_node->condition = parseExpression(LOWEST); // consumes expression, current is last token of it

        if (m_peekToken.type != TokenType::SEMICOLON) { // Check peek
            m_errors.push_back("Expected ';' after if condition.");
            return nullptr;
        }
        nextToken(); // consume last of expr, current is ';'
        nextToken(); // consume ';', current is start of first statement in block

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
            auto stmt = parseStatement(); // Should consume until its own end (e.g. ';')
            if (stmt) {
                if_node->consequence.push_back(std::move(stmt));
            }
            nextToken(); // Move to the next statement
        }

        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close if block.");
            return nullptr;
        }

        // At this point, m_currentToken is '}'.
        if (m_peekToken.type == TokenType::KEYWORD_ELSE) {
            nextToken(); // consume '}', current is 'else'

            if (m_peekToken.type == TokenType::KEYWORD_IF) {
                nextToken(); // consume 'else', current is 'if'
                if_node->alternative = parseIfChain(); // Recurse. The recursive call will consume until its own '}'
            } else { // plain else
                nextToken(); // consume 'else', current is '{'
                if_node->alternative = parseElseBlock(); // This will consume until its '}'
            }
        }

        return if_node;
    }

    std::unique_ptr<ElseNode> Parser::parseElseBlock()
    {
        // Expects m_currentToken to be '{'
        auto node = std::make_unique<ElseNode>();
        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for else block.");
            return nullptr;
        }
        nextToken(); // consume '{'

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE) {
            auto stmt = parseStatement();
            if (stmt) {
                node->consequence.push_back(std::move(stmt));
            }
            nextToken();
        }

        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close else block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<ExceptNode> Parser::parseExceptNode()
    {
        auto node = std::make_unique<ExceptNode>();
        nextToken();

        while (m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE)
        {
            ExceptNode::Constraint constraint;
            constraint.is_type_constraint = false;

            if (m_currentToken.type == TokenType::KEYWORD_CUSTOM ||
                m_currentToken.type == TokenType::KEYWORD_TEMPLATE ||
                m_currentToken.type == TokenType::KEYWORD_ORIGIN)
            {
                constraint.path.push_back(m_currentToken.literal);
                nextToken();
            }

            if (m_currentToken.type == TokenType::AT)
            {
                constraint.is_type_constraint = true;
                nextToken();
                if (m_currentToken.type != TokenType::IDENT) {
                    m_errors.push_back("Expected type identifier after '@' in except constraint.");
                    return nullptr;
                }
                constraint.path.push_back("@" + m_currentToken.literal);
                nextToken();
            }

            if (m_currentToken.type == TokenType::IDENT) {
                constraint.path.push_back(m_currentToken.literal);
                nextToken();
            }

            node->constraints.push_back(constraint);

            if (m_currentToken.type == TokenType::COMMA) {
                nextToken();
            } else {
                break;
            }
        }

        if (m_currentToken.type != TokenType::SEMICOLON) {
            m_errors.push_back("Expected ';' to terminate except statement.");
            return nullptr;
        }

        return node;
    }

    std::unique_ptr<InsertSpecializationNode> Parser::parseInsertSpecialization()
    {
        auto node = std::make_unique<InsertSpecializationNode>();
        nextToken(); // consume 'insert'

        if (m_currentToken.type == TokenType::KEYWORD_AFTER ||
            m_currentToken.type == TokenType::KEYWORD_BEFORE ||
            m_currentToken.type == TokenType::KEYWORD_REPLACE)
        {
            node->position = m_currentToken.literal;
            nextToken(); // consume position keyword

            std::string selector;
            if (m_currentToken.type == TokenType::IDENT) {
                selector += m_currentToken.literal;
                nextToken();
                if (m_currentToken.type == TokenType::LBRACKET) {
                    selector += m_currentToken.literal;
                    nextToken();
                    if (m_currentToken.type == TokenType::NUMBER) {
                        selector += m_currentToken.literal;
                        nextToken();
                        if (m_currentToken.type == TokenType::RBRACKET) {
                            selector += m_currentToken.literal;
                            nextToken();
                        } else {
                             m_errors.push_back("Expected ']' to close selector index.");
                        }
                    } else {
                        m_errors.push_back("Expected number for selector index.");
                    }
                }
            }

            if (selector.empty()) {
                m_errors.push_back("Expected target selector after '" + node->position + "'.");
                return nullptr;
            }
            node->target_selector = selector;
        }
        else if (m_currentToken.type == TokenType::KEYWORD_AT)
        {
             nextToken(); // consume 'at'
             if (m_currentToken.type == TokenType::KEYWORD_TOP) {
                 node->position = "at top";
             } else if (m_currentToken.type == TokenType::KEYWORD_BOTTOM) {
                 node->position = "at bottom";
             } else {
                 m_errors.push_back("Expected 'top' or 'bottom' after 'at'.");
                 return nullptr;
             }
             nextToken();
        }
        else
        {
            m_errors.push_back("Invalid keyword after 'insert'. Expected 'after', 'before', 'replace', or 'at'.");
            return nullptr;
        }

        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for insert content block.");
            return nullptr;
        }
        nextToken(); // Consume '{'

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            auto stmt = parseStatement();
            if (stmt) {
                node->content.push_back(std::move(stmt));
            }
        }

        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close insert content block.");
            return nullptr;
        }

        return node;
    }

    std::unique_ptr<NamespaceNode> Parser::parseNamespaceNode(ProgramNode& program)
    {
        auto node = std::make_unique<NamespaceNode>();
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected namespace name.");
            return nullptr;
        }
        node->name = m_currentToken.literal;
        std::string prev_namespace = m_current_namespace;
        m_current_namespace = node->name;
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for namespace block.");
            return nullptr;
        }
        nextToken();

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            std::unique_ptr<AstNode> stmt = nullptr;
            if (m_currentToken.type == TokenType::KEYWORD_TEMPLATE)
            {
                stmt = parseTemplateDefinition();
            }
            else if (m_currentToken.type == TokenType::KEYWORD_CUSTOM)
            {
                stmt = parseCustomDefinitionNode();
            }
            else
            {
                stmt = parseStatement();
            }

            if (stmt) {
                if (stmt->GetType() == NodeType::TemplateDefinition)
                {
                    auto* tmpl_def = static_cast<TemplateDefinitionNode*>(stmt.get());
                    program.templates[m_current_namespace][tmpl_def->name] = tmpl_def;
                }
                else if (stmt->GetType() == NodeType::CustomDefinition)
                {
                    auto* custom_def = static_cast<CustomDefinitionNode*>(stmt.get());
                    program.customs[m_current_namespace][custom_def->name] = custom_def;
                }
                node->children.push_back(std::move(stmt));
            }
            nextToken();
        }

        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close namespace block.");
            return nullptr;
        }
        m_current_namespace = prev_namespace;
        return node;
    }

    std::unique_ptr<ConfigurationNode> Parser::parseConfigurationStatement()
    {
        auto node = std::make_unique<ConfigurationNode>();
        if (m_peekToken.type == TokenType::AT) {
            nextToken();
            if (!expectPeek(TokenType::IDENT)) {
                m_errors.push_back("Expected configuration name after '@'.");
                return nullptr;
            }
            node->name = m_currentToken.literal;
        }
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for configuration block.");
            return nullptr;
        }
        nextToken();

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::KEYWORD_NAME)
            {
                node->name_config = parseNameConfigNode();
            }
            else if (m_currentToken.type == TokenType::IDENT)
            {
                std::string key = m_currentToken.literal;
                if (!expectPeek(TokenType::COLON)) {
                     m_errors.push_back("Expected '=' or ':' after configuration key.");
                     return nullptr;
                }
                nextToken();
                std::string value;
                if (m_currentToken.type == TokenType::IDENT || m_currentToken.type == TokenType::NUMBER || m_currentToken.type == TokenType::STRING) {
                    value = m_currentToken.literal;
                } else {
                    m_errors.push_back("Invalid value for configuration key " + key);
                    return nullptr;
                }
                node->settings[key] = value;
                if (m_peekToken.type == TokenType::SEMICOLON) {
                    nextToken();
                }
            }
            else
            {
                m_errors.push_back("Invalid token in configuration block: " + m_currentToken.ToString());
            }
            nextToken();
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close configuration block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<NameConfigNode> Parser::parseNameConfigNode()
    {
        auto node = std::make_unique<NameConfigNode>();
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for [Name] block.");
            return nullptr;
        }
        nextToken();

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::IDENT)
            {
                std::string key = m_currentToken.literal;
                if (!expectPeek(TokenType::COLON)) {
                     m_errors.push_back("Expected '=' or ':' after name config key.");
                     return nullptr;
                }
                nextToken();
                std::string value;
                if (m_currentToken.type == TokenType::IDENT) {
                    value = m_currentToken.literal;
                } else {
                    m_errors.push_back("Invalid value for name config key " + key + ". Must be an identifier.");
                    return nullptr;
                }
                node->settings[key] = value;
                if (m_peekToken.type == TokenType::SEMICOLON) {
                    nextToken();
                }
            }
            else
            {
                m_errors.push_back("Invalid token in [Name] block: " + m_currentToken.ToString());
            }
            nextToken();
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close [Name] block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<UseNode> Parser::parseUseStatement()
    {
        auto node = std::make_unique<UseNode>();
        nextToken();

        while (m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE)
        {
            std::string path_part;
            if (m_currentToken.type == TokenType::AT)
            {
                path_part = "@";
                nextToken();
                 if (m_currentToken.type != TokenType::IDENT) {
                     m_errors.push_back("Expected identifier after '@' in use statement.");
                     return nullptr;
                }
                 path_part += m_currentToken.literal;
                 node->path.push_back(path_part);
            }
            else if (m_currentToken.type == TokenType::IDENT || m_currentToken.type == TokenType::KEYWORD_HTML5)
            {
                node->path.push_back(m_currentToken.literal);
            }
            else
            {
                m_errors.push_back("Invalid token in use statement: " + m_currentToken.ToString());
                return nullptr;
            }
            nextToken();
        }
        if (m_currentToken.type != TokenType::SEMICOLON)
        {
            m_errors.push_back("Expected ';' after use statement.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<ImportNode> Parser::parseImportNode(ProgramNode& program)
    {
        auto node = std::make_unique<ImportNode>();
        if (!expectPeek(TokenType::AT)) {
            m_errors.push_back("Expected '@' for import type.");
            return nullptr;
        }
        if (!expectPeek(TokenType::IDENT)) {
             m_errors.push_back("Expected import type keyword after '@'.");
            return nullptr;
        }
        node->type = "@" + m_currentToken.literal;
        if (!expectPeek(TokenType::KEYWORD_FROM)) {
            m_errors.push_back("Expected 'from' keyword in import statement.");
            return nullptr;
        }
        if (!expectPeek(TokenType::STRING)) {
            m_errors.push_back("Expected file path string after 'from'.");
            return nullptr;
        }
        node->path = m_currentToken.literal;
        try {
            std::filesystem::path full_path = std::filesystem::weakly_canonical(std::filesystem::path(m_current_file_path).parent_path() / node->path);
            if (s_parsed_files.count(full_path.string())) {
            } else {
                std::string file_content = Loader::ReadFile(m_current_file_path, node->path);
                Lexer imported_lexer(file_content);
                Parser imported_parser(imported_lexer, full_path.string());
                auto imported_program = imported_parser.ParseProgram();
                for(auto const& [ns, def_map] : imported_program->templates) {
                    for (auto const& [name, def] : def_map) {
                        program.templates[ns][name] = def;
                    }
                }
                for(auto const& [ns, def_map] : imported_program->customs) {
                    for (auto const& [name, def] : def_map) {
                        program.customs[ns][name] = def;
                    }
                }
                for(const auto& err : imported_parser.GetErrors()) {
                    m_errors.push_back("Error in imported file '" + node->path + "': " + err);
                }
                program.imported_programs.push_back(std::move(imported_program));
            }
        } catch (const std::runtime_error& e) {
            m_errors.push_back("Could not import file '" + node->path + "'. Reason: " + e.what());
        }
        if (!expectPeek(TokenType::SEMICOLON)) {
            m_errors.push_back("Expected ';' after import statement.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<CustomDefinitionNode> Parser::parseCustomDefinitionNode()
    {
        auto node = std::make_unique<CustomDefinitionNode>();
        if (!expectPeek(TokenType::AT)) {
            m_errors.push_back("Expected '@' for custom type.");
            return nullptr;
        }
        if (!expectPeek(TokenType::IDENT)) {
             m_errors.push_back("Expected custom type keyword after '@'.");
            return nullptr;
        }
        std::string customType = m_currentToken.literal;
        if (customType != "Style" && customType != "Element" && customType != "Var") {
            m_errors.push_back("Unsupported custom type: @" + customType);
            return nullptr;
        }
        node->type = "@" + customType;
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected custom definition name.");
            return nullptr;
        }
        node->name = m_currentToken.literal;
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for custom definition block.");
            return nullptr;
        }
        nextToken();
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (customType == "Style" || customType == "Var")
            {
                if (m_currentToken.type == TokenType::IDENT)
                {
                    node->children.push_back(std::move(parseStyleProperty()));
                }
                else
                {
                    m_errors.push_back("Invalid token in " + node->type + " custom definition block: " + m_currentToken.ToString());
                }
                nextToken();
            }
            else
            {
                auto stmt = parseStatement();
                if (stmt)
                {
                    node->children.push_back(std::move(stmt));
                }
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close custom definition block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<AstNode> Parser::parseAtUsage()
    {
        nextToken();
        if (m_currentToken.type != TokenType::IDENT) {
            m_errors.push_back("Expected type keyword after '@'.");
            return nullptr;
        }
        std::string atType = m_currentToken.literal;
        if (atType != "Style" && atType != "Var" && atType != "Element") {
            m_errors.push_back("Unsupported usage: @" + atType);
            return nullptr;
        }
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected name after @" + atType);
            return nullptr;
        }
        std::string name = m_currentToken.literal;
        if (m_peekToken.type == TokenType::SEMICOLON)
        {
            nextToken();
            auto node = std::make_unique<TemplateUsageNode>();
            node->type = "@" + atType;
            node->name = name;
            return node;
        }
        else if (m_peekToken.type == TokenType::LBRACE)
        {
            nextToken();
            nextToken();
            auto node = std::make_unique<CustomUsageNode>();
            node->type = "@" + atType;
            node->name = name;
            while(m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
            {
                if (m_currentToken.type == TokenType::KEYWORD_DELETE)
                {
                    node->specializations.push_back(parseDeleteSpecialization());
                }
                else if (m_currentToken.type == TokenType::KEYWORD_INSERT)
                {
                    node->specializations.push_back(parseInsertSpecialization());
                }
                else if (m_currentToken.type == TokenType::IDENT)
                {
                    node->specializations.push_back(parseStyleProperty());
                }
                else
                {
                    m_errors.push_back("Unsupported specialization: " + m_currentToken.literal);
                }
                nextToken();
            }
            if (m_currentToken.type != TokenType::RBRACE) {
                 m_errors.push_back("Expected '}' to close specialization block.");
                 return nullptr;
            }
            return node;
        }
        else
        {
            m_errors.push_back("Expected '{' or ';' after @" + atType + " usage.");
            return nullptr;
        }
    }

    std::unique_ptr<DeleteSpecializationNode> Parser::parseDeleteSpecialization()
    {
        auto node = std::make_unique<DeleteSpecializationNode>();
        nextToken();
        if (m_currentToken.type != TokenType::IDENT)
        {
            m_errors.push_back("Expected property name after 'delete'.");
            return nullptr;
        }
        node->property_name = m_currentToken.literal;
        if (!expectPeek(TokenType::SEMICOLON)) {
             m_errors.push_back("Expected ';' after delete statement.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<TemplateDefinitionNode> Parser::parseTemplateDefinition()
    {
        auto node = std::make_unique<TemplateDefinitionNode>();
        if (!expectPeek(TokenType::AT)) {
            m_errors.push_back("Expected '@' for template type.");
            return nullptr;
        }
        if (!expectPeek(TokenType::IDENT)) {
             m_errors.push_back("Expected template type keyword after '@'.");
            return nullptr;
        }
        std::string templateType = m_currentToken.literal;
        if (templateType != "Style" && templateType != "Var" && templateType != "Element") {
            m_errors.push_back("Unsupported template type: @" + templateType);
            return nullptr;
        }
        node->type = "@" + templateType;
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected template name.");
            return nullptr;
        }
        node->name = m_currentToken.literal;
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for template block.");
            return nullptr;
        }
        nextToken();
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (templateType == "Style" || templateType == "Var")
            {
                if (m_currentToken.type == TokenType::IDENT)
                {
                    node->properties.push_back(std::move(parseStyleProperty()));
                }
                else
                {
                    m_errors.push_back("Invalid token in " + node->type + " template definition block: " + m_currentToken.ToString());
                }
                nextToken();
            }
            else
            {
                auto stmt = parseStatement();
                if (stmt)
                {
                    node->body.push_back(std::move(stmt));
                }
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close template block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<TextNode> Parser::parseTextNode()
    {
        auto node = std::make_unique<TextNode>();
        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }
        nextToken();
        std::string text_content;
        bool first_token = true;
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (!first_token) {
                if (m_currentToken.type != TokenType::DOT) {
                    text_content += " ";
                }
            }
            text_content += m_currentToken.literal;
            first_token = false;
            nextToken();
        }
        node->value = text_content;
        if (m_currentToken.type != TokenType::RBRACE)
        {
            m_errors.push_back("Expected '}' to close text block.");
            return nullptr;
        }
        return node;
    }

    Attribute Parser::parseAttribute()
    {
        Attribute attr;
        attr.name = m_currentToken.literal;
        nextToken();
        nextToken();
        if (m_currentToken.type == TokenType::STRING ||
            m_currentToken.type == TokenType::IDENT ||
            m_currentToken.type == TokenType::NUMBER)
        {
            attr.value = m_currentToken.literal;
        }
        else
        {
            m_errors.push_back("Invalid attribute value token: " + m_currentToken.ToString());
        }
        if (m_peekToken.type == TokenType::SEMICOLON)
        {
            nextToken();
        }
        return attr;
    }

    std::unique_ptr<StyleNode> Parser::parseStyleNode()
    {
        auto node = std::make_unique<StyleNode>();
        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }
        nextToken();
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::DOT || m_currentToken.type == TokenType::AMPERSAND)
            {
                node->children.push_back(parseStyleRuleNode());
                nextToken();
            }
            else if (m_currentToken.type == TokenType::AT)
            {
                node->children.push_back(parseAtUsage());
            }
            else if (m_currentToken.type == TokenType::IDENT)
            {
                node->children.push_back(parseStyleProperty());
            }
            else
            {
                m_errors.push_back("Invalid token in style block: " + m_currentToken.ToString());
                nextToken();
            }
            if (m_currentToken.type == TokenType::SEMICOLON) {
                nextToken();
            }
        }
        if (m_currentToken.type != TokenType::RBRACE)
        {
            m_errors.push_back("Expected '}' to close style block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<CommentNode> Parser::parseCommentNode()
    {
        auto node = std::make_unique<CommentNode>();
        node->value = m_currentToken.literal;
        return node;
    }

    std::unique_ptr<StyleProperty> Parser::parseStyleProperty()
    {
        auto prop = std::make_unique<StyleProperty>();
        prop->name = m_currentToken.literal;
        if (m_peekToken.type != TokenType::COLON) {
            m_errors.push_back("Expected ':' after style property name.");
            return nullptr;
        }
        nextToken();
        nextToken();
        auto first_expr = parseExpression(LOWEST);
        if (m_peekToken.type != TokenType::COMMA) {
            prop->value = std::move(first_expr);
        } else {
            auto list_node = std::make_unique<ExpressionListNode>();
            list_node->expressions.push_back(std::move(first_expr));
            while (m_peekToken.type == TokenType::COMMA) {
                nextToken();
                nextToken();
                list_node->expressions.push_back(parseExpression(LOWEST));
            }
            prop->value = std::move(list_node);
        }
        if (m_peekToken.type == TokenType::SEMICOLON)
        {
            nextToken();
        }
        return prop;
    }

    std::unique_ptr<StyleRuleNode> Parser::parseStyleRuleNode()
    {
        auto node = std::make_unique<StyleRuleNode>();
        std::string selector = "";
        while (m_currentToken.type != TokenType::LBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            selector += m_currentToken.literal;
            nextToken();
        }
        node->selector = selector;
        if (m_currentToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' to begin style rule block.");
            return nullptr;
        }
        nextToken();
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::IDENT) {
                auto prop = parseStyleProperty();
                if (prop) {
                    node->properties.push_back(std::move(prop));
                }
            } else {
                 m_errors.push_back("Invalid token in style rule: " + m_currentToken.ToString());
            }
            nextToken();
        }
        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close style rule block.");
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence)
    {
        std::unique_ptr<Expression> leftExp;
        if (m_currentToken.type == TokenType::NUMBER)
        {
            leftExp = parseNumberLiteral();
        }
        else if (m_currentToken.type == TokenType::IDENT)
        {
            if (m_peekToken.type == TokenType::LPAREN)
            {
                leftExp = parseVariableAccessExpression();
            }
            else
            {
                leftExp = parseIdentifier();
            }
        }
        else if (m_currentToken.type == TokenType::STRING)
        {
            leftExp = parseStringLiteral();
        }
        else
        {
            m_errors.push_back("No prefix parse function for " + TokenTypeToString(m_currentToken.type) + " found.");
            return nullptr;
        }

        while (m_peekToken.type != TokenType::SEMICOLON && precedence < (precedences.count(m_peekToken.type) ? precedences[m_peekToken.type] : LOWEST))
        {
            TokenType peekType = m_peekToken.type;
            if (peekType == TokenType::PLUS || peekType == TokenType::MINUS ||
                peekType == TokenType::ASTERISK || peekType == TokenType::SLASH ||
                peekType == TokenType::GT || peekType == TokenType::LT ||
                peekType == TokenType::MODULO || peekType == TokenType::POWER)
            {
                nextToken();
                leftExp = parseInfixExpression(std::move(leftExp));
            }
            else if (peekType == TokenType::QUESTION)
            {
                nextToken();
                leftExp = parseConditionalExpression(std::move(leftExp));
            }
            else
            {
                return leftExp;
            }
        }
        return leftExp;
    }

    std::unique_ptr<Expression> Parser::parseNumberLiteral()
    {
        auto literal = std::make_unique<NumberLiteral>();
        try
        {
            literal->value = std::stod(m_currentToken.literal);
        }
        catch (const std::invalid_argument& e)
        {
            m_errors.push_back("Could not parse " + m_currentToken.literal + " as double.");
            return nullptr;
        }
        if (m_peekToken.type == TokenType::IDENT)
        {
            nextToken();
            literal->unit = m_currentToken.literal;
        }
        return literal;
    }

    std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left)
    {
        auto expr = std::make_unique<InfixExpression>();
        expr->left = std::move(left);
        expr->op = m_currentToken.literal;
        Precedence curPrecedence = precedences.count(m_currentToken.type) ? precedences[m_currentToken.type] : LOWEST;
        nextToken();
        expr->right = parseExpression(curPrecedence);
        return expr;
    }

    std::unique_ptr<Expression> Parser::parseConditionalExpression(std::unique_ptr<Expression> condition)
    {
        auto expr = std::make_unique<ConditionalExpression>();
        expr->condition = std::move(condition);
        nextToken();
        expr->consequence = parseExpression(LOWEST);
        if (m_peekToken.type == TokenType::COLON)
        {
            nextToken();
            nextToken();
            expr->alternative = parseExpression(LOWEST);
        } else {
            expr->alternative = nullptr;
        }
        return expr;
    }

    std::unique_ptr<Expression> Parser::parseIdentifier()
    {
        auto ident = std::make_unique<Identifier>();
        ident->value = m_currentToken.literal;
        return ident;
    }

    std::unique_ptr<Expression> Parser::parseStringLiteral()
    {
        auto literal = std::make_unique<StringLiteral>();
        literal->value = m_currentToken.literal;
        return literal;
    }

    std::unique_ptr<Expression> Parser::parseVariableAccessExpression()
    {
        auto node = std::make_unique<VariableAccessNode>();
        node->template_name = m_currentToken.literal;
        if (!expectPeek(TokenType::LPAREN)) {
            return nullptr;
        }
        nextToken();
        if (m_currentToken.type != TokenType::IDENT) {
            m_errors.push_back("Expected variable name inside variable access.");
            return nullptr;
        }
        node->variable_name = m_currentToken.literal;
        if (!expectPeek(TokenType::RPAREN)) {
            return nullptr;
        }
        return node;
    }

    std::unique_ptr<OriginNode> Parser::parseOriginNode()
    {
        auto node = std::make_unique<OriginNode>();
        if (!expectPeek(TokenType::AT)) {
            m_errors.push_back("Expected '@' for origin type.");
            return nullptr;
        }
        if (!expectPeek(TokenType::IDENT)) {
             m_errors.push_back("Expected origin type keyword after '@'.");
            return nullptr;
        }
        node->type = "@" + m_currentToken.literal;
        if (m_peekToken.type == TokenType::IDENT) {
            nextToken();
            node->name = m_currentToken.literal;
        }
        if (m_peekToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for origin block.");
            return nullptr;
        }
        m_currentToken = m_peekToken;
        node->content = m_lexer.readRawBlockContent();
        m_currentToken = m_lexer.NextToken();
        m_peekToken = m_lexer.NextToken();
        return node;
    }

    std::unique_ptr<ScriptNode> Parser::parseScriptNode()
    {
        auto node = std::make_unique<ScriptNode>();
        if (m_peekToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for script block.");
            return nullptr;
        }

        // Manually advance currentToken to '{' without calling the lexer's NextToken().
        // This preserves the lexer's state, pointing after the '{', and avoids skipping whitespace.
        m_currentToken = m_peekToken;

        std::string script_content = m_lexer.readRawBlockContent();

        // After readRawBlockContent, the lexer's state is positioned right after
        // the closing '}'. We need to resynchronize the parser's tokens.
        m_currentToken = m_lexer.NextToken(); // Get the token after '}' which should be '}' itself from parser's view.
        if (m_currentToken.type != TokenType::RBRACE) {
             m_errors.push_back("Expected '}' to close script block after reading content.");
        }
        m_peekToken = m_lexer.NextToken();     // And the one after that

        // Now, parse the collected script content with the CHTLJS parser
        CHTLJS::Lexer js_lexer(script_content);
        CHTLJS::Parser js_parser(js_lexer);
        node->js_ast = js_parser.ParseProgram();

        // Transfer any errors from the JS parser to the main parser
        for (const auto& err : js_parser.GetErrors()) {
            m_errors.push_back("CHTL JS Parser Error: " + err);
        }

        return node;
    }
}