#include "Parser.h"
#include <stdexcept>
#include <algorithm>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {
    // Filter out comments
    this->tokens.erase(
        std::remove_if(this->tokens.begin(), this->tokens.end(), [](const Token& t) {
            return t.type == TokenType::SINGLE_LINE_COMMENT ||
                   t.type == TokenType::MULTI_LINE_COMMENT ||
                   t.type == TokenType::GENERATOR_COMMENT;
        }),
        this->tokens.end()
    );
}

std::unique_ptr<RootNode> Parser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        root->children.push_back(parseStatement());
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (peek().type == TokenType::TEXT_KEYWORD) {
        return parseText();
    }
    if (peek().type == TokenType::IDENTIFIER) {
        return parseElement();
    }
    // Should not happen with valid CHTL
    throw std::runtime_error("Unexpected token in parseStatement: " + peek().value);
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expected element name.");
    auto element = std::make_unique<ElementNode>(identifier.value);

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        element->children.push_back(parseStatement());
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after element body.");

    return element;
}

std::unique_ptr<TextNode> Parser::parseText() {
    consume(TokenType::TEXT_KEYWORD, "Expected 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'.");
    Token contentToken = consume(TokenType::STRING_LITERAL, "Expected string literal for text content.");
    consume(TokenType::RIGHT_BRACE, "Expected '}' after text content.");

    // Strip quotes from the string literal
    std::string content = contentToken.value;
    if (content.length() >= 2 && content.front() == '"' && content.back() == '"') {
        content = content.substr(1, content.length() - 2);
    }

    return std::make_unique<TextNode>(content);
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

Token Parser::peek() {
    return tokens[current];
}

bool Parser::isAtEnd() {
    return current >= tokens.size() || peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message + " Got " + peek().value + " instead.");
}