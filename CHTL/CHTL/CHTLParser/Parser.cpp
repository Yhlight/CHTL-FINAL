#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

void Parser::skipComments() {
    while (current < tokens.size() && (
        tokens[current].type == TokenType::Comment ||
        tokens[current].type == TokenType::MultiLineComment ||
        tokens[current].type == TokenType::GeneratorComment)) {
        current++;
    }
}

std::unique_ptr<Node> Parser::parse() {
    auto node = parseElement();
    return node;
}

std::unique_ptr<Node> Parser::parseElement() {
    skipComments();

    if (current >= tokens.size() || tokens[current].type != TokenType::Identifier) {
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

            if (tokens[current].type == TokenType::Identifier && tokens[current].value == "style") {
                parseStyleBlock(element.get());
            } else if (current + 1 < tokens.size() && (tokens[current+1].type == TokenType::Colon || tokens[current+1].type == TokenType::Equals)) {
                if (tokens[current].type == TokenType::Identifier && tokens[current].value == "text") {
                    current++; // consume 'text'
                    current++; // consume ':' or '='
                    element->children.push_back(parseText());
                    if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) {
                        current++; // consume ';'
                    }
                } else {
                    parseAttributes(element.get());
                }
            } else {
                auto child = parseElement();
                if (child) {
                    element->children.push_back(std::move(child));
                } else {
                    break;
                }
            }
        }

        if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) {
            current++; // Consume '}'
        }
    }

    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    if (tokens[current].type != TokenType::Identifier) {
        return;
    }
    std::string key = tokens[current++].value;
    current++; // consume ':' or '='

    if (current < tokens.size() && (tokens[current].type == TokenType::String || tokens[current].type == TokenType::Identifier)) {
        std::string value = tokens[current++].value;
        element->attributes[key] = value;
    }

    if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) {
        current++; // consume ';'
    }
}

void Parser::parseStyleBlock(ElementNode* element) {
    current++; // consume 'style'
    if (tokens[current].type == TokenType::OpenBrace) {
        current++; // consume '{'
        while (tokens[current].type != TokenType::CloseBrace) {
            std::string key = tokens[current++].value;
            current++; // consume ':' or '='
            std::string value = tokens[current++].value;
            element->styles[key] = value;
            current++; // consume ';'
        }
        current++; // consume '}'
    }
}

std::unique_ptr<Node> Parser::parseText() {
    auto text = std::make_unique<TextNode>();
    if (current < tokens.size() && tokens[current].type == TokenType::String) {
        text->text = tokens[current++].value;
    }
    return text;
}
