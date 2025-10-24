#include "Parser.h"
#include <stdexcept>
#include <iostream> // For cerr
#include <algorithm> // For std::remove

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::unique_ptr<ASTNode> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> nodes;
    while (!isAtEnd()) {
        auto node = declaration();
        if (node) {
            nodes.push_back(std::move(node));
        }
    }
    return std::make_unique<ElementNode>("root", std::vector<Attribute>{}, std::move(nodes));
}

std::unique_ptr<ASTNode> Parser::declaration() {
    while(match(TokenType::GeneratorComment)) {
        // Ignore generator comments
    }

    if (isAtEnd()) return nullptr;

    if (check(TokenType::Text)) {
        advance();
        return textNode();
    }
    if (check(TokenType::Style)) {
        advance();
        return styleNode();
    }
    if (check(TokenType::Identifier)) {
        return element();
    }

    // If we have an unhandled token, we'll advance past it to avoid infinite loops.
    std::cerr << "Warning: Unexpected token '" << peek().lexeme << "' at line "
              << peek().line << ", column " << peek().column << ". Skipping." << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::element() {
    Token identifier = consume(TokenType::Identifier, "Expect element name.");
    consume(TokenType::OpenBrace, "Expect '{' after element name.");

    std::vector<Attribute> attrs;
    std::vector<std::unique_ptr<ASTNode>> children;

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        // Peek ahead to see if the next token is an attribute or a child node
        if (check(TokenType::Identifier) && m_tokens[m_current + 1].type == TokenType::Colon || m_tokens[m_current+1].type == TokenType::Equals) {
            auto parsed_attrs = attributes();
            attrs.insert(attrs.end(), std::make_move_iterator(parsed_attrs.begin()), std::make_move_iterator(parsed_attrs.end()));
        } else {
            auto child = declaration();
            if(child) children.push_back(std::move(child));
        }
    }

    consume(TokenType::CloseBrace, "Expect '}' after element block.");

    return std::make_unique<ElementNode>(identifier.lexeme, std::move(attrs), std::move(children));
}

std::vector<Attribute> Parser::attributes() {
    std::vector<Attribute> attrs;
    while (check(TokenType::Identifier)) {
        Token key = advance();
        if (match(TokenType::Colon) || match(TokenType::Equals)) {
            std::string value_str;
            while (!check(TokenType::Semicolon) && !isAtEnd()) {
                value_str += advance().lexeme + " ";
            }
            // trim trailing space
            if (!value_str.empty()) {
                value_str.pop_back();
            }
            consume(TokenType::Semicolon, "Expect ';' after attribute value.");
            attrs.push_back({key.lexeme, value_str});
        } else {
            m_current--;
            break;
        }
    }
    return attrs;
}

std::unique_ptr<ASTNode> Parser::styleNode() {
    consume(TokenType::OpenBrace, "Expect '{' after 'style' keyword.");

    std::vector<Attribute> inline_properties;
    std::vector<Selector> selectors;

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (check(TokenType::Dot) || check(TokenType::Hash) || check(TokenType::Ampersand)) {
            Token type_token = advance();
            Selector::Type type;
            std::string name_str;

            if (type_token.type == TokenType::Ampersand) {
                type = Selector::Type::Context;
                // The "name" is the rest of the selector (e.g., ":hover")
                // We'll consume tokens until we hit the opening brace.
                while(!check(TokenType::OpenBrace) && !isAtEnd()) {
                    name_str += advance().lexeme;
                }
            } else {
                type = type_token.type == TokenType::Dot ? Selector::Type::Class : Selector::Type::Id;
                name_str = consume(TokenType::Identifier, "Expect selector name after '.' or '#'.").lexeme;
            }

            if (match(TokenType::OpenBrace)) {
                std::vector<Attribute> properties = attributes();
                consume(TokenType::CloseBrace, "Expect '}' after selector block.");
                selectors.push_back({type, name_str, std::move(properties)});
            } else {
                // This could be a syntax error, or a more complex selector not yet supported
                throw std::runtime_error("Expect '{' after selector.");
            }
        } else if (check(TokenType::Identifier)) {
            auto parsed_props = attributes();
            inline_properties.insert(inline_properties.end(), std::make_move_iterator(parsed_props.begin()), std::make_move_iterator(parsed_props.end()));
        } else {
            throw std::runtime_error("Unexpected token '" + peek().lexeme + "' in style block.");
        }
    }

    consume(TokenType::CloseBrace, "Expect '}' after style block.");
    return std::make_unique<StyleNode>(std::move(inline_properties), std::move(selectors));
}


std::unique_ptr<ASTNode> Parser::textNode() {
    consume(TokenType::OpenBrace, "Expect '{' after 'text' keyword.");
    Token stringToken = consume(TokenType::String, "Expect string literal inside text block.");
    consume(TokenType::CloseBrace, "Expect '}' after text block.");
    return std::make_unique<TextNode>(stringToken.lexeme);
}

Token Parser::advance() {
    if (!isAtEnd()) {
        m_current++;
    }
    return m_tokens[m_current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek() {
    return m_tokens[m_current];
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message + " Got " + peek().lexeme);
}

} // namespace CHTL
