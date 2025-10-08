#include "Parser.h"
#include <stdexcept>
#include <algorithm>
#include "../CHTLNode/NumericLiteralExprNode.h"
#include "../CHTLNode/IdentifierExprNode.h"
#include "ExpressionParser.h"
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLNode/InsertNode.h"

#include "../CHTLContext/CHTLContext.h"

Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context) : tokens(tokens), context(context) {
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
        auto statement = parseStatement();
        if (statement) { // Template declarations return nullptr
            root->children.push_back(std::move(statement));
        }
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (peek().type == TokenType::LEFT_BRACKET) {
        return parseTemplateOrCustomDeclaration();
    }
    if (peek().type == TokenType::TEXT_KEYWORD) {
        return parseText();
    }
    if (peek().type == TokenType::IDENTIFIER) {
        return parseElement();
    }
    if (peek().type == TokenType::AT) {
        return parseTemplateUsage();
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
        // Check for template usage first
        if (peek().type == TokenType::AT) {
            styleNode->templateUsages.push_back(parseTemplateUsage());
        } else if (peek().type == TokenType::DOT || peek().type == TokenType::HASH || (peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::LEFT_BRACE)) {
            // It's a rule
            styleNode->rules.push_back(parseStyleRule());
        } else {
            // Otherwise, it's an inline property
            Token key = consume(TokenType::IDENTIFIER, "Expected style property key.");
            consume(TokenType::COLON, "Expected ':' after style property key.");

            std::vector<Token> valueTokens;
            while(peek().type != TokenType::SEMICOLON && !isAtEnd()) {
                valueTokens.push_back(advance());
            }
            valueTokens.push_back(Token{TokenType::END_OF_FILE, "", 0, 0}); // Add EOF for the sub-parser

            ExpressionParser exprParser(valueTokens);
            auto value = exprParser.parse();

            consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
            styleNode->properties.push_back(std::make_unique<StylePropertyNode>(key.value, std::move(value)));
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

        std::vector<Token> valueTokens;
        while(peek().type != TokenType::SEMICOLON && !isAtEnd()) {
            valueTokens.push_back(advance());
        }
        valueTokens.push_back(Token{TokenType::END_OF_FILE, "", 0, 0});

        ExpressionParser exprParser(valueTokens);
        auto value = exprParser.parse();

        consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
        ruleNode->properties.push_back(std::make_unique<StylePropertyNode>(key.value, std::move(value)));
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

std::unique_ptr<DeleteNode> Parser::parseDeleteStatement() {
    consume(TokenType::DELETE_KEYWORD, "Expected 'delete' keyword.");

    std::vector<std::string> targets;
    do {
        // For now, assume simple identifiers as targets.
        // The spec allows for more complex selectors like div[1], but this will be handled later.
        targets.push_back(consume(TokenType::IDENTIFIER, "Expected target for delete.").value);
    } while (peek().type == TokenType::COMMA && advance().type == TokenType::COMMA);

    consume(TokenType::SEMICOLON, "Expected ';' after delete statement.");
    return std::make_unique<DeleteNode>(targets);
}

std::unique_ptr<InsertNode> Parser::parseInsertStatement() {
    consume(TokenType::INSERT_KEYWORD, "Expected 'insert' keyword.");

    InsertPosition position;
    if (peek().type == TokenType::AFTER_KEYWORD) {
        position = InsertPosition::AFTER;
        advance();
    } else if (peek().type == TokenType::BEFORE_KEYWORD) {
        position = InsertPosition::BEFORE;
        advance();
    } else if (peek().type == TokenType::REPLACE_KEYWORD) {
        position = InsertPosition::REPLACE;
        advance();
    } else {
        throw std::runtime_error("Expected position keyword (after, before, replace) after 'insert'.");
    }

    // Handle the target selector, which can include an index like div[0]
    std::string target = consume(TokenType::IDENTIFIER, "Expected target selector for insert.").value;
    if (peek().type == TokenType::LEFT_BRACKET) {
        target += advance().value; // consume '['
        target += consume(TokenType::NUMERIC_LITERAL, "Expected index in selector.").value;
        target += consume(TokenType::RIGHT_BRACKET, "Expected ']' after index.").value;
    }

    auto insertNode = std::make_unique<InsertNode>(position, target);

    consume(TokenType::LEFT_BRACE, "Expected '{' to start insert body.");
    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        insertNode->nodesToInsert.push_back(parseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to end insert body.");

    return insertNode;
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

std::unique_ptr<BaseNode> Parser::parseTemplateOrCustomDeclaration() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' at the start of a declaration.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expected 'Template' or 'Custom' keyword.");

    bool isCustom = false;
    if (keyword.value == "Template") {
        isCustom = false;
    } else if (keyword.value == "Custom") {
        isCustom = true;
    } else {
        throw std::runtime_error("Expected 'Template' or 'Custom' keyword inside brackets, got " + keyword.value);
    }
    consume(TokenType::RIGHT_BRACKET, "Expected ']' after 'Template' keyword.");

    consume(TokenType::AT, "Expected '@' before template type.");

    TemplateType type;
    if (peek().type == TokenType::STYLE_KEYWORD) {
        type = TemplateType::STYLE;
        advance();
    } else if (peek().type == TokenType::ELEMENT_KEYWORD) {
        type = TemplateType::ELEMENT;
        advance();
    } else {
        throw std::runtime_error("Unsupported template type in declaration: " + peek().value);
    }

    Token name = consume(TokenType::IDENTIFIER, "Expected template name.");

    if (isCustom) {
        auto customNode = std::make_unique<CustomDeclarationNode>(type, name.value);
        consume(TokenType::LEFT_BRACE, "Expected '{' to start custom template body.");
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            customNode->body.push_back(parseStatement());
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to end custom template body.");
        context.customTemplates[name.value] = std::move(customNode);
    } else {
        auto templateNode = std::make_unique<TemplateDeclarationNode>(type, name.value);
        consume(TokenType::LEFT_BRACE, "Expected '{' to start template body.");
        if (type == TemplateType::STYLE) {
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (peek().type == TokenType::AT) {
                    templateNode->body.push_back(parseTemplateUsage());
                } else {
                    Token key = consume(TokenType::IDENTIFIER, "Expected style property key.");
                    consume(TokenType::COLON, "Expected ':' after style property key.");
                    std::vector<Token> valueTokens;
                    while(peek().type != TokenType::SEMICOLON && !isAtEnd()) {
                        valueTokens.push_back(advance());
                    }
                    valueTokens.push_back(Token{TokenType::END_OF_FILE, "", 0, 0});
                    ExpressionParser exprParser(valueTokens);
                    auto value = exprParser.parse();
                    consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
                    templateNode->body.push_back(std::make_unique<StylePropertyNode>(key.value, std::move(value)));
                }
            }
        } else if (type == TemplateType::ELEMENT) {
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                templateNode->body.push_back(parseStatement());
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to end template body.");
        if (type == TemplateType::STYLE) {
            context.styleTemplates[name.value] = std::move(templateNode);
        } else if (type == TemplateType::ELEMENT) {
            context.elementTemplates[name.value] = std::move(templateNode);
        }
    }

    return nullptr; // Declarations are not part of the main AST
}

std::unique_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    consume(TokenType::AT, "Expected '@' for template usage.");

    TemplateType type;
    if (peek().type == TokenType::STYLE_KEYWORD) {
        type = TemplateType::STYLE;
        advance();
    } else if (peek().type == TokenType::ELEMENT_KEYWORD) {
        type = TemplateType::ELEMENT;
        advance();
    } else {
        throw std::runtime_error("Unsupported template type in usage: " + peek().value);
    }

    Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
    auto usageNode = std::make_unique<TemplateUsageNode>(type, name.value);

    // Check for an optional specialization block
    if (peek().type == TokenType::LEFT_BRACE) {
        consume(TokenType::LEFT_BRACE, "Expected '{' to start specialization block.");
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (peek().type == TokenType::DELETE_KEYWORD) {
                usageNode->specializations.push_back(std::move(parseDeleteStatement()));
            } else if (peek().type == TokenType::INSERT_KEYWORD) {
                usageNode->specializations.push_back(std::move(parseInsertStatement()));
            } else {
                throw std::runtime_error("Unexpected token in specialization block: " + peek().value);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to end specialization block.");
    } else {
        // No block, so just a semicolon
        consume(TokenType::SEMICOLON, "Expected ';' after template usage.");
    }

    return usageNode;
}