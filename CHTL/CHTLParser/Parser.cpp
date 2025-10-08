#include "Parser.h"
#include <memory>

Parser::Parser(Lexer& l) : l(l) {
    // Read two tokens, so curToken and peekToken are both set
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l.NextToken();
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    while (curToken.type != TokenType::TOKEN_EOF) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        } else {
            nextToken(); // Should not happen in a valid program, but prevents infinite loops
        }
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (curToken.type) {
        case TokenType::TOKEN_IDENTIFIER:
            if (peekToken.type == TokenType::TOKEN_LBRACE) {
                return parseElementStatement();
            } else if (peekToken.type == TokenType::TOKEN_COLON) {
                return parseAttributeStatement();
            }
            break;
        case TokenType::TOKEN_TEXT:
            if (peekToken.type == TokenType::TOKEN_LBRACE) {
                return parseTextStatement();
            } else if (peekToken.type == TokenType::TOKEN_COLON) {
                // `text: "..."` is an attribute
                return parseAttributeStatement();
            }
            break;
        case TokenType::TOKEN_STRING:
             return parseExpressionStatement();
        default:
            return nullptr;
    }
    return nullptr;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    if (curToken.type == TokenType::TOKEN_STRING) {
        auto str = std::make_unique<StringLiteral>();
        str->token = curToken;
        str->value = curToken.literal;
        return str;
    }
    return nullptr;
}


std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->token = curToken;
    stmt->expression = parseExpression();

    nextToken(); // Consume the expression token

    if (curToken.type == TokenType::TOKEN_SEMICOLON) {
        nextToken(); // Consume the semicolon
    }
    return stmt;
}


std::unique_ptr<AttributeStatement> Parser::parseAttributeStatement() {
    auto stmt = std::make_unique<AttributeStatement>();
    stmt->token = curToken;
    stmt->name = std::make_unique<Identifier>(curToken, curToken.literal);

    nextToken(); // consume name
    nextToken(); // consume ':'

    stmt->value = parseExpression();
    nextToken(); // consume value token

    if (curToken.type == TokenType::TOKEN_SEMICOLON) {
        nextToken(); // consume ';'
    }
    return stmt;
}


std::unique_ptr<ElementStatement> Parser::parseElementStatement() {
    auto stmt = std::make_unique<ElementStatement>();
    stmt->token = curToken;
    stmt->tagName = std::make_unique<Identifier>(curToken, curToken.literal);
    nextToken(); // consume identifier
    stmt->body = parseBlockStatement();
    nextToken(); // consume '}'
    return stmt;
}

std::unique_ptr<TextStatement> Parser::parseTextStatement() {
    auto stmt = std::make_unique<TextStatement>();
    stmt->token = curToken;
    nextToken(); // consume 'text'
    stmt->body = parseBlockStatement();
    nextToken(); // consume '}'
    return stmt;
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>();
    block->token = curToken; // {
    nextToken(); // consume {

    while (curToken.type != TokenType::TOKEN_RBRACE && curToken.type != TokenType::TOKEN_EOF) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        } else {
             nextToken(); // Skip token if it doesn't start a statement
        }
    }
    // after loop, curToken is '}'
    return block;
}