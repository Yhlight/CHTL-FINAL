#include "ExpressionParser.h"
#include "../CHTLNode/NumericLiteralExprNode.h"
#include "../CHTLNode/BinaryOpExprNode.h"
#include <stdexcept>

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ExprNode> ExpressionParser::parse() {
    return expression();
}

std::unique_ptr<ExprNode> ExpressionParser::expression() {
    return term();
}

std::unique_ptr<ExprNode> ExpressionParser::term() {
    auto expr = factor();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = advance();
        auto right = factor();
        expr = std::make_unique<BinaryOpExprNode>(std::move(expr), op.type, std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> ExpressionParser::factor() {
    auto expr = primary();

    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        Token op = advance();
        auto right = primary();
        expr = std::make_unique<BinaryOpExprNode>(std::move(expr), op.type, std::move(right));
    }

    return expr;
}

#include "../CHTLNode/IdentifierExprNode.h"

std::unique_ptr<ExprNode> ExpressionParser::primary() {
    if (peek().type == TokenType::NUMERIC_LITERAL) {
        return std::make_unique<NumericLiteralExprNode>(std::stod(advance().value));
    }
    if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::TEXT_KEYWORD) {
        return std::make_unique<IdentifierExprNode>(advance().value);
    }

    throw std::runtime_error("Expected expression.");
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

Token ExpressionParser::peek() {
    return tokens[current];
}

bool ExpressionParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token ExpressionParser::previous() {
    return tokens[current - 1];
}