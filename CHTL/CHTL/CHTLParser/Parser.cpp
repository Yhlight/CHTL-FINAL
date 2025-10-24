#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<Node> Parser::parse() {
    return parseElement();
}

std::unique_ptr<Node> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = tokens[current++].value;

    if (tokens[current].type == TokenType::OpenBrace) {
        current++;
        while (tokens[current].type != TokenType::CloseBrace) {
            if (tokens[current].value == "text") {
                current++; // consume 'text'
                current++; // consume ':'
                element->children.push_back(parseText());
                current++; // consume ';'
            } else {
                element->children.push_back(parseElement());
            }
        }
        current++;
    }

    return element;
}

std::unique_ptr<Node> Parser::parseText() {
    auto text = std::make_unique<TextNode>();
    text->text = tokens[current++].value;
    return text;
}
