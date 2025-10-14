#include "Parser.h"
#include "../CHTLNode/Style.h"
#include <memory>
#include <map>

namespace CHTL {

    // Pratt parser precedence table
    const std::map<TokenType, Precedence> precedences = {
        {TokenType::EQ,       Precedence::SUM},
        {TokenType::NOT_EQ,   Precedence::SUM},
        {TokenType::LT,       Precedence::SUM},
        {TokenType::GT,       Precedence::SUM},
        {TokenType::PLUS,     Precedence::SUM},
        {TokenType::MINUS,    Precedence::SUM},
        {TokenType::SLASH,    Precedence::PRODUCT},
        {TokenType::ASTERISK, Precedence::PRODUCT},
        {TokenType::MODULO,   Precedence::PRODUCT},
        {TokenType::POWER,    Precedence::POWER},
        {TokenType::AND,      Precedence::LOGICAL},
        {TokenType::OR,       Precedence::LOGICAL},
        {TokenType::QUESTION, Precedence::CONDITIONAL},
        {TokenType::DOT,      Precedence::PROPERTY},
    };

    // Forward declarations for parse functions
    std::unique_ptr<Expression> ParseInfixExpression(Parser* parser, std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> ParseConditionalExpression(Parser* parser, std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> ParsePropertyAccessExpression(Parser* parser, std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> ParseIdentifier(Parser* parser);
    std::unique_ptr<Expression> ParseNumberLiteral(Parser* parser);
    std::unique_ptr<Expression> ParseStringLiteral(Parser* parser);

    const std::map<TokenType, PrefixParseFn> prefixParseFns = {
        {TokenType::IDENT,  ParseIdentifier},
        {TokenType::HASH,   ParseIdentifier},
        {TokenType::DOT,    ParseIdentifier},
        {TokenType::NUMBER, ParseNumberLiteral},
        {TokenType::STRING, ParseStringLiteral},
    };

    const std::map<TokenType, InfixParseFn> infixParseFns = {
        {TokenType::PLUS,     ParseInfixExpression},
        {TokenType::MINUS,    ParseInfixExpression},
        {TokenType::SLASH,    ParseInfixExpression},
        {TokenType::ASTERISK, ParseInfixExpression},
        {TokenType::MODULO,   ParseInfixExpression},
        {TokenType::POWER,    ParseInfixExpression},
        {TokenType::EQ,       ParseInfixExpression},
        {TokenType::NOT_EQ,   ParseInfixExpression},
        {TokenType::LT,       ParseInfixExpression},
        {TokenType::GT,       ParseInfixExpression},
        {TokenType::AND,      ParseInfixExpression},
        {TokenType::OR,       ParseInfixExpression},
        {TokenType::QUESTION, ParseConditionalExpression},
        {TokenType::DOT,      ParsePropertyAccessExpression},
    };

    std::unique_ptr<Expression> ParsePropertyAccessExpression(Parser* parser, std::unique_ptr<Expression> left) {
        Token token = parser->currentToken;
        parser->NextToken();
        auto property = std::make_unique<Identifier>(parser->currentToken, parser->currentToken.literal);
        return std::make_unique<PropertyAccessExpression>(token, std::move(left), std::move(property));
    }

    std::unique_ptr<Expression> ParseInfixExpression(Parser* parser, std::unique_ptr<Expression> left) {
        Token token = parser->currentToken;
        std::string op = parser->currentToken.literal;
        Precedence precedence = parser->CurPrecedence();
        parser->NextToken();
        auto right = parser->ParseExpression(precedence);
        return std::make_unique<InfixExpression>(token, std::move(left), op, std::move(right));
    }

    std::unique_ptr<Expression> ParseConditionalExpression(Parser* parser, std::unique_ptr<Expression> left) {
        Token token = parser->currentToken;
        parser->NextToken();
        auto consequence = parser->ParseExpression(Precedence::LOWEST);
        parser->NextToken(); // consume ':'
        auto alternative = parser->ParseExpression(Precedence::LOWEST);
        return std::make_unique<ConditionalExpression>(token, std::move(left), std::move(consequence), std::move(alternative));
    }

    std::unique_ptr<Expression> ParseIdentifier(Parser* parser) {
        return std::make_unique<Identifier>(parser->currentToken, parser->currentToken.literal);
    }

    std::unique_ptr<Expression> ParseNumberLiteral(Parser* parser) {
        std::string literal = parser->currentToken.literal;
        size_t first_alpha = std::string::npos;
        for (size_t i = 0; i < literal.length(); ++i) {
            if (isalpha(literal[i]) || literal[i] == '%') {
                first_alpha = i;
                break;
            }
        }

        std::string num_part = literal.substr(0, first_alpha);
        std::string unit_part = (first_alpha != std::string::npos) ? literal.substr(first_alpha) : "";

        double value = std::stod(num_part);
        return std::make_unique<NumberLiteral>(parser->currentToken, value, unit_part);
    }

    std::unique_ptr<Expression> ParseStringLiteral(Parser* parser) {
        return std::make_unique<StringLiteral>(parser->currentToken, parser->currentToken.literal);
    }


    Parser::Parser(Lexer& l) : lexer(l) {
        NextToken();
        NextToken();
    }

    void Parser::NextToken() {
        currentToken = peekToken;
        peekToken = lexer.NextToken();
    }

    std::unique_ptr<Program> Parser::ParseProgram() {
        auto program = std::make_unique<Program>();
        while (currentToken.type != TokenType::END_OF_FILE) {
            auto stmt = ParseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
            NextToken();
        }
        return program;
    }

    std::unique_ptr<Statement> Parser::ParseStatement() {
        if (currentToken.type == TokenType::IDENT && peekToken.type == TokenType::COLON) {
            return ParseAttribute();
        }

        switch (currentToken.type) {
        case TokenType::IDENT:
            return ParseElementStatement();
        case TokenType::TEXT:
            return ParseTextStatement();
        case TokenType::STYLE:
            return ParseStyleStatement();
        default:
            return nullptr;
        }
    }

    void Parser::PeekError(TokenType t) {
        std::string msg = "expected next token to be " + std::to_string(static_cast<int>(t)) +
                          ", got " + std::to_string(static_cast<int>(peekToken.type)) + " instead";
        errors.push_back(msg);
    }

    std::unique_ptr<Attribute> Parser::ParseAttribute() {
        auto key = std::make_unique<Identifier>(currentToken, currentToken.literal);

        if (peekToken.type != TokenType::COLON && peekToken.type != TokenType::ASSIGN) {
            PeekError(TokenType::COLON); // or ASSIGN
            return nullptr;
        }
        NextToken(); // Consume identifier, currentToken is now ':' or '='
        Token opToken = currentToken;

        NextToken(); // Consume ':' or '=', currentToken is now the value expression

        auto value = ParseExpression(Precedence::LOWEST);

        if (peekToken.type == TokenType::SEMICOLON) {
            NextToken();
        }

        return std::make_unique<Attribute>(opToken, std::move(key), std::move(value));
    }

    std::unique_ptr<Expression> Parser::ParseExpression(Precedence precedence) {
        auto prefixIt = prefixParseFns.find(currentToken.type);
        if (prefixIt == prefixParseFns.end()) {
            return nullptr;
        }
        auto leftExp = prefixIt->second(this);

        while (peekToken.type != TokenType::SEMICOLON && precedence < PeekPrecedence()) {
            auto infixIt = infixParseFns.find(peekToken.type);
            if (infixIt == infixParseFns.end()) {
                return leftExp;
            }
            NextToken();
            leftExp = infixIt->second(this, std::move(leftExp));
        }

        return leftExp;
    }

    std::unique_ptr<StyleStatement> Parser::ParseStyleStatement() {
        auto styleStmt = std::make_unique<StyleStatement>(currentToken);
        if (peekToken.type != TokenType::LBRACE) {
            PeekError(TokenType::LBRACE);
            return nullptr;
        }
        NextToken(); // consume 'style'
        NextToken(); // consume '{'

        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
            if (currentToken.type == TokenType::DOT || currentToken.type == TokenType::HASH) {
                auto rule = ParseStyleRule();
                if (rule) {
                    styleStmt->Rules.push_back(std::move(rule));
                }
            } else {
                auto prop = ParseStyleProperty();
                if (prop) {
                    styleStmt->Properties.push_back(std::move(prop));
                }
            }
            NextToken();
        }
        return styleStmt;
    }

    std::unique_ptr<StyleRule> Parser::ParseStyleRule() {
        Token selectorToken = currentToken;
        std::string selectorText;

        if (currentToken.type == TokenType::DOT || currentToken.type == TokenType::HASH) {
            selectorText += currentToken.literal;
            NextToken(); // consume '.' or '#'
            selectorText += currentToken.literal; // consume identifier
            NextToken();
        } else if (currentToken.type == TokenType::AMPERSAND) {
            // Greedily consume tokens for complex selectors like &:hover
            while (currentToken.type != TokenType::LBRACE && currentToken.type != TokenType::END_OF_FILE) {
                selectorText += currentToken.literal;
                NextToken();
            }
        } else {
             return nullptr; // Should not happen if called correctly
        }

        auto selector = std::make_unique<Identifier>(selectorToken, selectorText);
        auto rule = std::make_unique<StyleRule>(selectorToken, std::move(selector));

        if (currentToken.type != TokenType::LBRACE) {
            PeekError(TokenType::LBRACE);
            return nullptr;
        }
        NextToken(); // consume '{'

        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
             auto prop = ParseStyleProperty();
            if (prop) {
                rule->Properties.push_back(std::move(prop));
            }
            NextToken();
        }
        return rule;
    }

    std::unique_ptr<StyleProperty> Parser::ParseStyleProperty() {
        auto key = std::make_unique<Identifier>(currentToken, currentToken.literal);

        if (peekToken.type != TokenType::COLON) {
            PeekError(TokenType::COLON);
            return nullptr;
        }

        NextToken(); // consume key
        Token opToken = currentToken;
        NextToken(); // consume ':'

        auto value = ParseExpression(Precedence::LOWEST);

        if (peekToken.type == TokenType::SEMICOLON) {
            NextToken();
        }

        return std::make_unique<StyleProperty>(opToken, std::move(key), std::move(value));
    }


    std::unique_ptr<TextStatement> Parser::ParseTextStatement() {
        Token textToken = currentToken; // The 'text' token

        if (peekToken.type != TokenType::LBRACE) {
            PeekError(TokenType::LBRACE);
            return nullptr;
        }
        NextToken(); // Consume 'text', currentToken is now '{'
        NextToken(); // Consume '{', currentToken is now the first token of the literal

        std::unique_ptr<Expression> literal;
        if (currentToken.type == TokenType::STRING) {
            literal = std::make_unique<StringLiteral>(currentToken, currentToken.literal);
            NextToken(); // Consume the string token
        } else {
            literal = ParseUnquotedLiteral(); // This will consume tokens until '}'
        }

        if (currentToken.type != TokenType::RBRACE) {
            PeekError(TokenType::RBRACE);
            return nullptr;
        }

        return std::make_unique<TextStatement>(textToken, std::move(literal));
    }

    std::unique_ptr<Expression> Parser::ParseUnquotedLiteral() {
        Token startToken = currentToken;
        std::string literalValue;

        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
            literalValue += currentToken.literal;
            if (peekToken.type != TokenType::RBRACE && peekToken.type != TokenType::END_OF_FILE) {
                 literalValue += " ";
            }
            NextToken();
        }

        return std::make_unique<UnquotedLiteral>(startToken, literalValue);
    }

    std::unique_ptr<ElementStatement> Parser::ParseElementStatement() {
        auto tag = std::make_unique<Identifier>(currentToken, currentToken.literal);

        if (peekToken.type != TokenType::LBRACE) {
            PeekError(TokenType::LBRACE);
            return nullptr;
        }
        NextToken(); // Consume identifier

        auto body = std::make_unique<BlockStatement>();
        auto element = std::make_unique<ElementStatement>(std::move(tag), std::move(body));

        NextToken(); // Consume '{'

        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
            if (currentToken.type == TokenType::IDENT && peekToken.type == TokenType::COLON) {
                auto attr = ParseAttribute();
                if (attr) {
                    element->Attributes.push_back(std::move(attr));
                }
            } else {
                auto stmt = ParseStatement();
                if (stmt) {
                    element->Body->statements.push_back(std::move(stmt));
                }
            }
            NextToken();
        }

        return element;
    }

    std::unique_ptr<BlockStatement> Parser::ParseBlockStatement() {
        auto block = std::make_unique<BlockStatement>();
        NextToken(); // Consume '{'

        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
            auto stmt = ParseStatement();
            if (stmt) {
                block->statements.push_back(std::move(stmt));
            }
            NextToken();
        }

        return block;
    }

    Precedence Parser::PeekPrecedence() {
        if (precedences.count(peekToken.type)) {
            return precedences.at(peekToken.type);
        }
        return Precedence::LOWEST;
    }

    Precedence Parser::CurPrecedence() {
        if (precedences.count(currentToken.type)) {
            return precedences.at(currentToken.type);
        }
        return Precedence::LOWEST;
    }

} // namespace CHTL
