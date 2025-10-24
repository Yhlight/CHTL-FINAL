#include "Parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

void Parser::error(const std::string& message) {
    errors.push_back(message);
    // In a real implementation, you might want to synchronize the parser
    // to a known state to continue parsing and find more errors.
    // For now, we'll just stop parsing by throwing an exception.
    throw std::runtime_error(message);
}

void Parser::skipComments() {
    while (current < tokens.size() && (
        tokens[current].type == TokenType::Comment ||
        tokens[current].type == TokenType::MultiLineComment ||
        tokens[current].type == TokenType::GeneratorComment)) {
        current++;
    }
}

Document Parser::parse() {
    Document doc;
    try {
        doc.root = parseElement(doc);
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
    }
    return doc;
}

std::unique_ptr<Node> Parser::parseElement(Document& doc) {
    skipComments();
    if (current >= tokens.size() || tokens[current].type != TokenType::Identifier) {
        error("Expected an identifier for an element name.");
        return nullptr;
    }

    auto element = std::make_unique<ElementNode>();
    element->tagName = tokens[current++].value;

    skipComments();
    if (current < tokens.size() && tokens[current].type == TokenType::OpenBrace) {
        current++; // Consume '{'
        while (current < tokens.size() && tokens[current].type != TokenType::CloseBrace) {
            skipComments();
            if (current >= tokens.size() || tokens[current].type == TokenType::CloseBrace) break;

            if (tokens[current].value == "style") {
                parseStyleBlock(element.get(), doc);
            } else if (tokens[current].value == "text") {
                current++; // consume 'text'
                if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
                element->children.push_back(parseText());
                if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
            } else if (current + 1 < tokens.size() && (tokens[current+1].type == TokenType::Colon || tokens[current+1].type == TokenType::Equals)) {
                parseAttributes(element.get());
            } else {
                auto child = parseElement(doc); // Pass doc recursively
                if (child) {
                    element->children.push_back(std::move(child));
                } else {
                    break;
                }
            }
        }
        if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) current++;
        else error("Expected '}'.");
    }
    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    std::string key = tokens[current++].value;
    if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
    else error("Expected ':' or '=' after attribute name.");
    if (current < tokens.size() && (tokens[current].type == TokenType::String || tokens[current].type == TokenType::Identifier)) {
        element->attributes[key] = tokens[current++].value;
    } else {
        error("Expected attribute value.");
    }
    if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
    else error("Expected ';' after attribute.");
}

void Parser::parseStyleBlock(ElementNode* element, Document& doc) {
    current++; // consume 'style'
    skipComments();
    if (current < tokens.size() && tokens[current].type == TokenType::OpenBrace) {
        current++; // consume '{'
        while (current < tokens.size() && tokens[current].type != TokenType::CloseBrace) {
            skipComments();
            if (current >= tokens.size() || tokens[current].type == TokenType::CloseBrace) break;

            if (tokens[current].type == TokenType::Ampersand) {
                current++; // consume '&'
                StyleRule rule;
                std::string pseudo_selector;
                while(current < tokens.size() && tokens[current].type != TokenType::OpenBrace) {
                    pseudo_selector += tokens[current++].value;
                }
                std::string base_selector = (element->attributes.count("class") ? "." + element->attributes["class"] : "#" + element->attributes["id"]);
                rule.selector = base_selector + pseudo_selector;

                skipComments();
                if (current < tokens.size() && tokens[current].type == TokenType::OpenBrace) {
                    current++; // consume '{'
                    while (current < tokens.size() && tokens[current].type != TokenType::CloseBrace) {
                        skipComments();
                        if (current >= tokens.size() || tokens[current].type == TokenType::CloseBrace) break;

                        std::string style_key = tokens[current++].value;
                        if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
                        else error("Expected ':' or '=' after style property name.");
                        rule.properties[style_key] = parseExpression();
                        if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
                        else error("Expected ';' after style property.");
                    }
                    if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) current++;
                    else error("Expected '}' to close style rule block.");
                }
                doc.globalStyles.push_back(std::move(rule));

            } else if (tokens[current].value[0] == '.' || tokens[current].value[0] == '#') {
                StyleRule rule;
                rule.selector = tokens[current].value;
                if (tokens[current].value[0] == '.') element->attributes["class"] = tokens[current].value.substr(1);
                else element->attributes["id"] = tokens[current].value.substr(1);
                current++; // consume selector

                skipComments();
                if (current < tokens.size() && tokens[current].type == TokenType::OpenBrace) {
                    current++; // consume '{'
                    while (current < tokens.size() && tokens[current].type != TokenType::CloseBrace) {
                        skipComments();
                        if (current >= tokens.size() || tokens[current].type == TokenType::CloseBrace) break;

                        std::string style_key = tokens[current++].value;
                        if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
                        else error("Expected ':' or '=' after style property name.");
                        rule.properties[style_key] = parseExpression();
                        if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
                        else error("Expected ';' after style property.");
                    }
                    if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) current++;
                     else error("Expected '}' to close style rule block.");
                }
                doc.globalStyles.push_back(std::move(rule));
            } else { // Inline styles
                std::string style_key = tokens[current++].value;
                if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
                else error("Expected ':' or '=' after style property name.");
                element->styles[style_key] = parseExpression();
                if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
                else error("Expected ';' after style property.");
            }
        }
        if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) current++;
        else error("Expected '}' to close style block.");
    }
}

std::unique_ptr<Expression> Parser::parseExpression() {
    if (tokens[current].type == TokenType::String || tokens[current].type == TokenType::Identifier) {
        auto string_literal = std::make_unique<StringLiteral>();
        string_literal->value = tokens[current++].value;
        return string_literal;
    }

    if (tokens[current].type != TokenType::Number) {
        error("Expected a number or string for an expression.");
        return nullptr;
    }

    auto left = std::make_unique<NumberLiteral>();
    left->value = std::stod(tokens[current++].value);
    if (tokens[current].type == TokenType::Identifier) {
        left->unit = tokens[current++].value;
    }

    if (tokens[current].type == TokenType::Plus || tokens[current].type == TokenType::Minus || tokens[current].type == TokenType::Star || tokens[current].type == TokenType::Slash) {
        auto binaryExpr = std::make_unique<BinaryExpr>();
        binaryExpr->left = std::move(left);
        switch(tokens[current].type) {
            case TokenType::Plus: binaryExpr->op = Operator::Add; break;
            case TokenType::Minus: binaryExpr->op = Operator::Subtract; break;
            case TokenType::Star: binaryExpr->op = Operator::Multiply; break;
            case TokenType::Slash: binaryExpr->op = Operator::Divide; break;
            default: break;
        }
        current++;
        binaryExpr->right = parseExpression();
        return binaryExpr;
    }

    return left;
}

std::unique_ptr<Node> Parser::parseText() {
    auto text = std::make_unique<TextNode>();
    if (current < tokens.size() && tokens[current].type == TokenType::String) {
        text->text = tokens[current++].value;
    }
    return text;
}
