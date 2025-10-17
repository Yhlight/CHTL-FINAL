#include "Parser.h"
#include "Loader.h"
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

        // Add the initial file to the parsed list to prevent self-import
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

    // 期待下一个Token是指定类型
    bool Parser::expectPeek(TokenType t)
    {
        if (m_peekToken.type == t)
        {
            nextToken();
            return true;
        }
        else
        {
            // 记录错误
            m_errors.push_back("Expected next token to be " + TokenTypeToString(t) +
                               ", got " + TokenTypeToString(m_peekToken.type) + " instead.");
            return false;
        }
    }

    // 解析一个语句（目前仅支持元素节点）
    std::unique_ptr<AstNode> Parser::parseStatement()
    {
        if (m_currentToken.type == TokenType::IDENT)
        {
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
        return nullptr;
    }

    // 解析元素节点: e.g., div { id: box; ... }
    std::unique_ptr<ElementNode> Parser::parseElementNode()
    {
        auto node = std::make_unique<ElementNode>();
        node->tag_name = m_currentToken.literal;

        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }

        nextToken();

        // 解析属性和子节点
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            // 判断是属性还是子元素
            if (m_currentToken.type == TokenType::KEYWORD_TEXT && m_peekToken.type == TokenType::COLON)
            {
                // 这是 text: "value"; 语法
                nextToken(); // consume 'text'
                nextToken(); // consume ':' or '='

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
            else // 否则就是子元素
            {
                auto stmt = parseStatement();
                if (stmt)
                {
                    node->children.push_back(std::move(stmt));
                }
            }

            if (m_currentToken.type == TokenType::SEMICOLON) {
                nextToken();
            } else {
                nextToken();
            }
        }

        if (m_currentToken.type != TokenType::RBRACE)
        {
            m_errors.push_back("Expected '}' to close element block.");
            return nullptr;
        }

        return node;
    }

    // 解析命名空间 e.g., [Namespace] MySpace { ... }
    std::unique_ptr<NamespaceNode> Parser::parseNamespaceNode(ProgramNode& program)
    {
        auto node = std::make_unique<NamespaceNode>();

        // Current token is [Namespace]
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected namespace name.");
            return nullptr;
        }
        node->name = m_currentToken.literal;

        // Set current namespace
        std::string prev_namespace = m_current_namespace;
        m_current_namespace = node->name;

        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for namespace block.");
            return nullptr;
        }
        nextToken(); // Consume '{'

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
            // Note: Imports and nested namespaces could be supported here in the future.
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

        // Restore previous namespace
        m_current_namespace = prev_namespace;

        return node;
    }

    // 解析配置组 e.g., [Configuration] { KEY = VALUE; }
    std::unique_ptr<ConfigurationNode> Parser::parseConfigurationStatement()
    {
        auto node = std::make_unique<ConfigurationNode>();
        // Current token is [Configuration]

        // Check for an optional name
        if (m_peekToken.type == TokenType::AT) {
            nextToken(); // consume '@'
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
        nextToken(); // Consume '{'

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::KEYWORD_NAME)
            {
                node->name_config = parseNameConfigNode();
            }
            else if (m_currentToken.type == TokenType::IDENT)
            {
                std::string key = m_currentToken.literal;

                if (!expectPeek(TokenType::COLON)) { // COLON is ':' or '='
                     m_errors.push_back("Expected '=' or ':' after configuration key.");
                     return nullptr;
                }

                nextToken(); // Move to the value token

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

    // 解析Name配置块 e.g., [Name] { KEY = VALUE; }
    std::unique_ptr<NameConfigNode> Parser::parseNameConfigNode()
    {
        auto node = std::make_unique<NameConfigNode>();
        // Current token is [Name]

        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for [Name] block.");
            return nullptr;
        }
        nextToken(); // Consume '{'

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::IDENT)
            {
                std::string key = m_currentToken.literal;

                if (!expectPeek(TokenType::COLON)) { // COLON is ':' or '='
                     m_errors.push_back("Expected '=' or ':' after name config key.");
                     return nullptr;
                }

                nextToken(); // Move to the value token

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
        // The calling function (parseConfigurationStatement) will advance the token past '}'
        return node;
    }

    // 解析 use 语句 e.g., use html5; or use @Config Basic;
    std::unique_ptr<UseNode> Parser::parseUseStatement()
    {
        auto node = std::make_unique<UseNode>();
        // Current token is 'use'
        nextToken(); // consume 'use'

        // The path can be a simple identifier like 'html5' or a more complex path
        while (m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::END_OF_FILE)
        {
            std::string path_part;
            if (m_currentToken.type == TokenType::AT)
            {
                path_part = "@";
                nextToken(); // consume '@'
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
        // The main loop in ParseProgram will advance past the semicolon

        return node;
    }

    // 解析导入语句 e.g., [Import] @Chtl from "./file.chtl";
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
                // Already parsed, skip to avoid circular dependency.
                // We still return the node to represent the import statement, but do not process it.
            } else {
                std::string file_content = Loader::ReadFile(m_current_file_path, node->path);
                Lexer imported_lexer(file_content);
                Parser imported_parser(imported_lexer, full_path.string());
                auto imported_program = imported_parser.ParseProgram();

                // Merge templates
                for(auto const& [ns, def_map] : imported_program->templates) {
                    for (auto const& [name, def] : def_map) {
                        program.templates[ns][name] = def;
                    }
                }
                // Merge customs
                for(auto const& [ns, def_map] : imported_program->customs) {
                    for (auto const& [name, def] : def_map) {
                        program.customs[ns][name] = def;
                    }
                }
                // Also merge errors
                for(const auto& err : imported_parser.GetErrors()) {
                    m_errors.push_back("Error in imported file '" + node->path + "': " + err);
                }

                // Transfer ownership to prevent memory errors
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

    // 解析自定义定义 e.g., [Custom] @Style MyStyle { ... }
    std::unique_ptr<CustomDefinitionNode> Parser::parseCustomDefinitionNode()
    {
        auto node = std::make_unique<CustomDefinitionNode>();

        // Current token is KEYWORD_CUSTOM

        // Expect @Style or @Element
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

        // Expect custom name
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected custom definition name.");
            return nullptr;
        }
        node->name = m_currentToken.literal;

        // Expect { ... } block
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for custom definition block.");
            return nullptr;
        }
        nextToken(); // Consume '{'

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            // TODO: Implement parsing for specializations like 'delete'
            if (customType == "Style" || customType == "Var")
            {
                if (m_currentToken.type == TokenType::IDENT)
                {
                    node->children.push_back(std::move(parseStyleProperty()));
                }
                else
                {
                    m_errors.push_back("Invalid token in " + node->type + " custom definition block: " + m_currentToken.ToString());
                    nextToken();
                }
            }
            else // Element
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

    // 解析 @ 关键字用法, e.g., @Style DefaultText; or @Element MyCustom { delete ...; }
    std::unique_ptr<AstNode> Parser::parseAtUsage()
    {
        // Current token is '@'
        nextToken(); // Consume '@'

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

        // Now, decide if it's a TemplateUsage or CustomUsage based on the *peek* token.
        if (m_peekToken.type == TokenType::SEMICOLON)
        {
            nextToken(); // consume name
            // It's a simple TemplateUsage
            auto node = std::make_unique<TemplateUsageNode>();
            node->type = "@" + atType;
            node->name = name;
            return node;
        }
        else if (m_peekToken.type == TokenType::LBRACE)
        {
            nextToken(); // consume name
            nextToken(); // consume '{'
            // It's a CustomUsage with a specialization block
            auto node = std::make_unique<CustomUsageNode>();
            node->type = "@" + atType;
            node->name = name;

            while(m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
            {
                if (m_currentToken.type == TokenType::KEYWORD_DELETE)
                {
                    node->specializations.push_back(parseDeleteSpecialization());
                }
                else
                {
                    m_errors.push_back("Unsupported specialization: " + m_currentToken.literal);
                    nextToken(); // Skip unknown token
                }
            }

            if (m_currentToken.type != TokenType::RBRACE) {
                 m_errors.push_back("Expected '}' to close specialization block.");
                 return nullptr;
            }
            nextToken(); // Consume '}'

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
        // Current token is 'delete'
        auto node = std::make_unique<DeleteSpecializationNode>();
        nextToken(); // consume 'delete'

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
        // After expectPeek, current token is the semicolon. The calling loop needs us to advance past it.
        nextToken();

        return node;
    }

    // 解析模板定义 e.g., [Template] @Style DefaultText { ... }
    std::unique_ptr<TemplateDefinitionNode> Parser::parseTemplateDefinition()
    {
        auto node = std::make_unique<TemplateDefinitionNode>();

        // The current token is KEYWORD_TEMPLATE.

        // Expect @Style or @Var
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

        // Expect template name
        if (!expectPeek(TokenType::IDENT)) {
            m_errors.push_back("Expected template name.");
            return nullptr;
        }
        node->name = m_currentToken.literal;

        // Expect { ... } block
        if (!expectPeek(TokenType::LBRACE)) {
            m_errors.push_back("Expected '{' for template block.");
            return nullptr;
        }
        nextToken(); // Consume '{'

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
                    nextToken();
                }
            }
            else // Element template
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

    // 解析文本节点: e.g., text { "content" } or text { unquoted content }
    std::unique_ptr<TextNode> Parser::parseTextNode()
    {
        auto node = std::make_unique<TextNode>();

        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }
        nextToken(); // Consume '{'

        std::string text_content;
        bool first_token = true;
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (!first_token) {
                // Add a space, unless the current token is punctuation like a dot.
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

    // 解析属性: e.g., id: "box"; or class = container;
    Attribute Parser::parseAttribute()
    {
        Attribute attr;
        attr.name = m_currentToken.literal;

        // 跳过 ':' or '='
        nextToken();

        nextToken(); // 前进到值Token

        // 值的类型可以是 STRING, IDENT (无引号), or NUMBER
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

        // 处理可选的分号
        if (m_peekToken.type == TokenType::SEMICOLON)
        {
            nextToken();
        }

        return attr;
    }

    // 解析样式块: e.g., style { width: 100px; .box { ... } }
    std::unique_ptr<StyleNode> Parser::parseStyleNode()
    {
        auto node = std::make_unique<StyleNode>();

        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }
        nextToken(); // 消费'{'

        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::DOT || m_currentToken.type == TokenType::AMPERSAND)
            {
                // 解析CSS规则
                node->children.push_back(parseStyleRuleNode());
                // After a rule is parsed, current token is '}'
                nextToken(); // Consume '}'
            }
            else if (m_currentToken.type == TokenType::AT)
            {
                node->children.push_back(parseAtUsage());
            }
            else if (m_currentToken.type == TokenType::IDENT)
            {
                // 解析内联样式属性
                node->children.push_back(parseStyleProperty());
            }
            else
            {
                m_errors.push_back("Invalid token in style block: " + m_currentToken.ToString());
                nextToken(); // 跳过无法识别的Token
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

    // 解析注释节点: e.g., # this is a comment
    std::unique_ptr<CommentNode> Parser::parseCommentNode()
    {
        auto node = std::make_unique<CommentNode>();
        node->value = m_currentToken.literal;
        return node;
    }

    // 解析样式属性 e.g. width: 100px, 200px;
    std::unique_ptr<StyleProperty> Parser::parseStyleProperty()
    {
        auto prop = std::make_unique<StyleProperty>();
        prop->name = m_currentToken.literal;

        if (m_peekToken.type != TokenType::COLON) {
            m_errors.push_back("Expected ':' after style property name.");
            return nullptr;
        }
        nextToken(); // 消费属性名
        nextToken(); // 消费':'

        auto first_expr = parseExpression(LOWEST);

        if (m_peekToken.type != TokenType::COMMA) {
            // Single expression
            prop->value = std::move(first_expr);
        } else {
            // Expression list
            auto list_node = std::make_unique<ExpressionListNode>();
            list_node->expressions.push_back(std::move(first_expr));

            while (m_peekToken.type == TokenType::COMMA) {
                nextToken(); // consume ','
                nextToken(); // move to start of next expression
                list_node->expressions.push_back(parseExpression(LOWEST));
            }
            prop->value = std::move(list_node);
        }


        if (m_peekToken.type == TokenType::SEMICOLON)
        {
            nextToken();
        }
        nextToken();

        return prop;
    }

    // 解析CSS规则 e.g. .box { width: 100px; } or &:hover { ... }
    std::unique_ptr<StyleRuleNode> Parser::parseStyleRuleNode()
    {
        auto node = std::make_unique<StyleRuleNode>();
        std::string selector = "";

        // The tokens that make up the selector are IDENT, DOT, AMPERSAND, COLON, etc.
        // We consume them until we hit the opening brace of the rule block.
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
        nextToken(); // Consume '{'

        // Now parse the properties inside the rule block
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            auto prop = parseStyleProperty();
            if (prop) {
                node->properties.push_back(std::move(prop));
            } else {
                // If parseStyleProperty failed, we need to advance to avoid an infinite loop.
                nextToken();
            }
        }

        if (m_currentToken.type != TokenType::RBRACE) {
            m_errors.push_back("Expected '}' to close style rule block.");
            return nullptr;
        }
        // The calling function (parseStyleNode) is responsible for consuming the closing brace.
        // We leave m_currentToken on '}' so it knows where we are.

        return node;
    }


    // --- Expression Parsing Methods ---

    std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence)
    {
        // Prefix position
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
            // No prefix parse function for the token, record an error
            m_errors.push_back("No prefix parse function for " + TokenTypeToString(m_currentToken.type) + " found.");
            return nullptr;
        }

        // Infix position
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

        // Check for a unit identifier (e.g., "px", "em")
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

    // 解析条件表达式: e.g., <condition> ? <consequence> [: <alternative>]
    std::unique_ptr<Expression> Parser::parseConditionalExpression(std::unique_ptr<Expression> condition)
    {
        auto expr = std::make_unique<ConditionalExpression>();
        expr->condition = std::move(condition);

        nextToken(); // 消费 '?'
        expr->consequence = parseExpression(LOWEST); // Parse consequence with lowest precedence until ':' or ',' or ';'

        // Optional alternative
        if (m_peekToken.type == TokenType::COLON)
        {
            nextToken(); // 消费 ':'
            nextToken(); // Move to the start of the alternative expression
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

    // 解析变量访问表达式 e.g., ThemeColor(tableColor)
    std::unique_ptr<Expression> Parser::parseVariableAccessExpression()
    {
        auto node = std::make_unique<VariableAccessNode>();

        // Current token is the template name (IDENT)
        node->template_name = m_currentToken.literal;

        if (!expectPeek(TokenType::LPAREN)) {
            return nullptr;
        }
        nextToken(); // Consume '('

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

    // 解析原始嵌入节点 e.g., [Origin] @Html myDiv { <div>...</div> }
    std::unique_ptr<OriginNode> Parser::parseOriginNode()
    {
        auto node = std::make_unique<OriginNode>();
        // Current token is [Origin]

        if (!expectPeek(TokenType::AT)) {
            m_errors.push_back("Expected '@' for origin type.");
            return nullptr;
        }
        if (!expectPeek(TokenType::IDENT)) {
             m_errors.push_back("Expected origin type keyword after '@'.");
            return nullptr;
        }
        node->type = "@" + m_currentToken.literal;

        // Check for an optional name
        if (m_peekToken.type == TokenType::IDENT) {
            nextToken();
            node->name = m_currentToken.literal;
        }

        if (m_peekToken.type != TokenType::LBRACE) {
            m_errors.push_back("Expected '{' for origin block.");
            return nullptr;
        }
        // Manually advance past the LBRACE *without* triggering the lexer's NextToken()
        // which would skip whitespace.
        m_currentToken = m_peekToken;

        // At this point, the lexer's read position is exactly after the '{',
        // which is what readRawBlockContent expects.
        node->content = m_lexer.readRawBlockContent();

        // After readRawBlockContent, the lexer's state is at the closing '}'.
        // We need to fully re-sync the parser's token buffer.
        m_peekToken = m_lexer.NextToken(); // This is the token *after* the '}'
        m_currentToken = m_peekToken;      // Set current token to the one after '}'
        m_peekToken = m_lexer.NextToken(); // Set peek token to the one after that

        return node;
    }

} // namespace CHTL
