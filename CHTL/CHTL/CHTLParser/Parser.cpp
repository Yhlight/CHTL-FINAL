#include "Parser.h"
#include <memory>

namespace CHTL {

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
        switch (currentToken.type) {
        case TokenType::IDENT:
            return ParseElementStatement();
        case TokenType::TEXT:
            return ParseTextStatement();
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

        auto value = ParseExpression();

        if (peekToken.type == TokenType::SEMICOLON) {
            NextToken();
        }

        return std::make_unique<Attribute>(opToken, std::move(key), std::move(value));
    }

    std::unique_ptr<Expression> Parser::ParseExpression() {
        if (currentToken.type == TokenType::STRING) {
             return std::make_unique<StringLiteral>(currentToken, currentToken.literal);
        }
        // For now, we'll treat unquoted values as identifiers
        return std::make_unique<Identifier>(currentToken, currentToken.literal);
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

} // namespace CHTL
