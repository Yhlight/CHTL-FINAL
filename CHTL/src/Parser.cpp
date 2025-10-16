#include "Parser.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <unordered_set>

namespace CHTL
{

// Helper function to read a file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        // In a real implementation, you'd want better error handling
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
    std::unordered_map<TokenType, Parser::Precedence> Parser::precedences = {
        {TokenType::PLUS,     Parser::Precedence::SUM},
        {TokenType::MINUS,    Parser::Precedence::SUM},
        {TokenType::ASTERISK, Parser::Precedence::PRODUCT},
        {TokenType::SLASH,    Parser::Precedence::PRODUCT},
        {TokenType::GT,       Parser::Precedence::COMPARE},
        {TokenType::LT,       Parser::Precedence::COMPARE},
        {TokenType::QUESTION, Parser::Precedence::CONDITIONAL},
    };

    Parser::Parser(Lexer& lexer)
        : m_lexer(lexer)
    {
        // 读取两个Token，以设置好currentToken和peekToken
        nextToken();
        nextToken();
    }

    void Parser::nextToken()
    {
        m_currentToken = m_peekToken;
        m_peekToken = m_lexer.NextToken();
    }

    // 主解析函数
    std::unique_ptr<ProgramNode> Parser::ParseProgram()
    {
        auto program = std::make_unique<ProgramNode>();

        while (m_currentToken.type != TokenType::END_OF_FILE)
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
            else if (m_currentToken.type == TokenType::KEYWORD_IMPORT)
            {
                stmt = parseImportNode();
            }
            else
            {
                stmt = parseStatement();
            }

            if (stmt)
            {
                if (stmt->GetType() == NodeType::Import)
                {
                    auto* import_node = static_cast<ImportNode*>(stmt.get());
                    if (m_imported_files.find(import_node->path) == m_imported_files.end())
                    {
                        m_imported_files.insert(import_node->path);
                        std::string file_content = readFile(import_node->path);
                        if (!file_content.empty())
                        {
                            Lexer imported_lexer(file_content);
                            Parser imported_parser(imported_lexer);
                            auto imported_program = imported_parser.ParseProgram();

                            // Merge templates
                            for(auto const& [key, val] : imported_program->templates)
                            {
                                program->templates[key] = val;
                            }
                            // Merge customs
                             for(auto const& [key, val] : imported_program->customs)
                            {
                                program->customs[key] = val;
                            }
                             // Also merge errors
                            for(const auto& err : imported_parser.GetErrors())
                            {
                                m_errors.push_back("Error in imported file '" + import_node->path + "': " + err);
                            }

                            // Transfer ownership to prevent memory errors
                            program->imported_programs.push_back(std::move(imported_program));
                        }
                        else
                        {
                            m_errors.push_back("Could not read imported file: " + import_node->path);
                        }
                    }
                }
                else
                {
                    if (stmt->GetType() == NodeType::TemplateDefinition)
                    {
                        auto* tmpl_def = static_cast<TemplateDefinitionNode*>(stmt.get());
                        program->templates[tmpl_def->name] = tmpl_def;
                    }
                    else if (stmt->GetType() == NodeType::CustomDefinition)
                    {
                        auto* custom_def = static_cast<CustomDefinitionNode*>(stmt.get());
                        program->customs[custom_def->name] = custom_def;
                    }
                    program->children.push_back(std::move(stmt));
                }
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
            if (m_currentToken.type == TokenType::IDENT && (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::ASSIGN))
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

    // 解析导入语句 e.g., [Import] @Chtl from "./file.chtl";
    std::unique_ptr<ImportNode> Parser::parseImportNode()
    {
        auto node = std::make_unique<ImportNode>();

        // Current token is [Import]

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

    // 解析文本节点: e.g., text { "content" }
    std::unique_ptr<TextNode> Parser::parseTextNode()
    {
        auto node = std::make_unique<TextNode>();

        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }

        nextToken();

        if (m_currentToken.type != TokenType::STRING)
        {
            m_errors.push_back("Expected a string literal inside text block.");
            return nullptr;
        }

        node->value = m_currentToken.literal;

        if (!expectPeek(TokenType::RBRACE))
        {
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

    // 解析样式属性 e.g. width: 100px;
    std::unique_ptr<StyleProperty> Parser::parseStyleProperty()
    {
        auto prop = std::make_unique<StyleProperty>();
        prop->name = m_currentToken.literal;

        if (m_peekToken.type != TokenType::COLON && m_peekToken.type != TokenType::ASSIGN) {
            m_errors.push_back("Expected ':' or '=' after style property name.");
            return nullptr;
        }
        nextToken(); // 消费属性名
        nextToken(); // 消费':'或'='

        prop->value = parseExpression(LOWEST);

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
                peekType == TokenType::GT || peekType == TokenType::LT)
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

    // 解析条件表达式: e.g., <condition> ? <consequence> : <alternative>
    std::unique_ptr<Expression> Parser::parseConditionalExpression(std::unique_ptr<Expression> condition)
    {
        auto expr = std::make_unique<ConditionalExpression>();
        expr->condition = std::move(condition);

        nextToken(); // 消费 '?'
        expr->consequence = parseExpression(CONDITIONAL);

        if (!expectPeek(TokenType::COLON))
        {
            m_errors.push_back("Expected ':' in conditional expression.");
            return nullptr;
        }

        nextToken(); // 消费 ':'
        expr->alternative = parseExpression(CONDITIONAL);

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

} // namespace CHTL
