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

Document Parser::parse() {
    Document doc;
    doc.root = parseElement(doc);
    return doc;
}

std::unique_ptr<Node> Parser::parseElement(Document& doc) {
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
    }
    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    std::string key = tokens[current++].value;
    if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
    if (current < tokens.size() && (tokens[current].type == TokenType::String || tokens[current].type == TokenType::Identifier)) {
        element->attributes[key] = tokens[current++].value;
    }
    if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
}

void Parser::parseStyleBlock(ElementNode* element, Document& doc) {
    current++; // consume 'style'
    skipComments();
    if (current < tokens.size() && tokens[current].type == TokenType::OpenBrace) {
        current++; // consume '{'
        while (current < tokens.size() && tokens[current].type != TokenType::CloseBrace) {
            skipComments();
            if (current >= tokens.size() || tokens[current].type == TokenType::CloseBrace) break;

            if (tokens[current].value[0] == '.' || tokens[current].value[0] == '#') {
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
                        if (current < tokens.size() && (tokens[current].type == TokenType::String || tokens[current].type == TokenType::Identifier)) {
                            rule.properties[style_key] = tokens[current++].value;
                        }
                        if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
                    }
                    if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) current++;
                }
                doc.globalStyles.push_back(rule);
            } else { // Inline styles
                std::string style_key = tokens[current++].value;
                if (current < tokens.size() && (tokens[current].type == TokenType::Colon || tokens[current].type == TokenType::Equals)) current++;
                if (current < tokens.size() && (tokens[current].type == TokenType::String || tokens[current].type == TokenType::Identifier)) {
                     element->styles[style_key] = tokens[current++].value;
                }
                if (current < tokens.size() && tokens[current].type == TokenType::Semicolon) current++;
            }
        }
        if (current < tokens.size() && tokens[current].type == TokenType::CloseBrace) current++;
    }
}

std::unique_ptr<Node> Parser::parseText() {
    auto text = std::make_unique<TextNode>();
    if (current < tokens.size() && tokens[current].type == TokenType::String) {
        text->text = tokens[current++].value;
    }
    return text;
}
