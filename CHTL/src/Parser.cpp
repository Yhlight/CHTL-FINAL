#include "Parser.h"
#include <memory>

namespace CHTL
{
    std::unordered_map<TokenType, Parser::Precedence> Parser::precedences = {
        {TokenType::PLUS,     Parser::Precedence::SUM},
        {TokenType::MINUS,    Parser::Precedence::SUM},
        {TokenType::ASTERISK, Parser::Precedence::PRODUCT},
        {TokenType::SLASH,    Parser::Precedence::PRODUCT},
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
            auto stmt = parseStatement();
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
            else // 否则就是子元素
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

    // 解析样式块: e.g., style { width: 100px; }
    std::unique_ptr<StyleNode> Parser::parseStyleNode()
    {
        auto node = std::make_unique<StyleNode>();

        // 当前Token是'style'，期望下一个是'{'
        if (!expectPeek(TokenType::LBRACE))
        {
            return nullptr;
        }
        nextToken(); // 消费'{'

        // 循环解析样式属性
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::END_OF_FILE)
        {
            if (m_currentToken.type == TokenType::IDENT)
            {
                StyleProperty prop;
                prop.name = m_currentToken.literal;

                // 期望':'或'='
                if (m_peekToken.type != TokenType::COLON && m_peekToken.type != TokenType::ASSIGN) {
                     m_errors.push_back("Expected ':' or '=' after style property name.");
                     return nullptr;
                }
                nextToken(); // 消费属性名
                nextToken(); // 消费':'或'='

                prop.value = parseExpression(LOWEST);
                node->properties.push_back(std::move(prop));

                // 在解析完一个表达式后，当前Token是表达式的最后一个Token。
                // 我们需要前进到下一个Token，以准备解析下一个属性或'}'。
                // 如果下一个是分号，我们也消费它。
                if (m_peekToken.type == TokenType::SEMICOLON)
                {
                    nextToken();
                }
                nextToken();

            } else {
                 m_errors.push_back("Invalid token in style block: " + m_currentToken.ToString());
                 nextToken(); // 跳过无法识别的Token，避免无限循环
            }
        }

        if (m_currentToken.type != TokenType::RBRACE)
        {
             m_errors.push_back("Expected '}' to close style block.");
             return nullptr;
        }

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
        else
        {
            // No prefix parse function for the token, record an error
            m_errors.push_back("No prefix parse function for " + TokenTypeToString(m_currentToken.type) + " found.");
            return nullptr;
        }

        // Infix position
        while (m_peekToken.type != TokenType::SEMICOLON && precedence < (precedences.count(m_peekToken.type) ? precedences[m_peekToken.type] : LOWEST))
        {
            nextToken();
            leftExp = parseInfixExpression(std::move(leftExp));
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

} // namespace CHTL
