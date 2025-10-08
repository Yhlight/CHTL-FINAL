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
        if (peek().type == TokenType::STYLE_KEYWORD) {
            element->style = parseStyle();
        } else if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::TEXT_KEYWORD) {
            // Look ahead to differentiate between a child element and an attribute.
            if (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS) {
                parseAttribute(element.get());
            } else {
                element->children.push_back(parseStatement());
            }
        } else {
            // It's not an attribute or a style block, so it must be a statement (e.g., a text {} block)
            element->children.push_back(parseStatement());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after element body.");

    return element;
}

void Parser::parseAttribute(ElementNode* element) {
    Token key;
    if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::TEXT_KEYWORD) {
        key = advance();
    } else {
        throw std::runtime_error("Expected attribute key.");
    }

    // Consume colon or equals
    if (!match({TokenType::COLON, TokenType::EQUALS})) {
        throw std::runtime_error("Expected ':' or '=' after attribute key.");
    }

    Token valueToken;
    // An attribute value can be a string literal or any identifier-like token
    if (peek().type == TokenType::STRING_LITERAL) {
        valueToken = consume(TokenType::STRING_LITERAL, "Expected attribute value.");
    } else if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::TEXT_KEYWORD || peek().type == TokenType::NUMERIC_LITERAL) {
        valueToken = advance();
    } else {
        throw std::runtime_error("Expected attribute value to be a string or an identifier.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");

    std::string value = valueToken.value;
    // Strip quotes from string literals
    if (valueToken.type == TokenType::STRING_LITERAL) {
        if (value.length() >= 2 && (value.front() == '"' || value.front() == '\'') && value.front() == value.back()) {
            value = value.substr(1, value.length() - 2);
        }
    }

    if (key.value == "text") {
        element->children.push_back(std::make_unique<TextNode>(value));
    } else {
        element->attributes[key.value] = value;
    }
}

std::unique_ptr<StyleNode> Parser::parseStyle() {
    consume(TokenType::STYLE_KEYWORD, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'.");

    auto styleNode = std::make_unique<StyleNode>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // If it starts with a selector-like token, or is an identifier followed by a brace, it's a rule.
        if (peek().type == TokenType::DOT || peek().type == TokenType::HASH || (peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::LEFT_BRACE)) {
            styleNode->rules.push_back(parseStyleRule());
        } else {
            // Otherwise, it's an inline property.
            Token key = consume(TokenType::IDENTIFIER, "Expected style property key.");
            consume(TokenType::COLON, "Expected ':' after style property key.");
            Token value = advance();
            consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
            styleNode->properties.push_back(std::make_unique<StylePropertyNode>(key.value, value.value));
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block.");

    return styleNode;
}

std::unique_ptr<StyleRuleNode> Parser::parseStyleRule() {
    std::string selector;
    // A selector is a sequence of tokens up to the '{'
    while (peek().type != TokenType::LEFT_BRACE && !isAtEnd()) {
        selector += advance().value;
    }

    auto ruleNode = std::make_unique<StyleRuleNode>(selector);

    consume(TokenType::LEFT_BRACE, "Expected '{' after style rule selector.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token key = consume(TokenType::IDENTIFIER, "Expected style property key.");
        consume(TokenType::COLON, "Expected ':' after style property key.");
        Token value = advance();
        consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
        ruleNode->properties.push_back(std::make_unique<StylePropertyNode>(key.value, value.value));
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after style rule block.");

    return ruleNode;
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