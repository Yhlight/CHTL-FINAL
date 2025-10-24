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

    if (match(TokenType::Text)) {
        return textNode();
    }
    if (match(TokenType::Style)) {
        return styleNode();
    }
    if (check(TokenType::Identifier)) {
        return element();
    }

    // If we have an unhandled token, we'll advance past it to avoid infinite loops.
    // This is a simple error recovery strategy. A more robust parser would
    // synchronize to the next valid declaration.
    std::cerr << "Warning: Unexpected token '" << peek().lexeme << "' at line "
              << peek().line << ", column " << peek().column << ". Skipping." << std::endl;
    advance();
    return nullptr; // Will be filtered out later
}

std::unique_ptr<ASTNode> Parser::element() {
    Token identifier = consume(TokenType::Identifier, "Expect element name.");
    consume(TokenType::OpenBrace, "Expect '{' after element name.");

    std::vector<Attribute> attrs = attributes();
    std::vector<std::unique_ptr<ASTNode>> children;

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        children.push_back(declaration());
    }

    consume(TokenType::CloseBrace, "Expect '}' after element block.");

    // Filter out nullptrs from skipped tokens
    children.erase(std::remove(children.begin(), children.end(), nullptr), children.end());

    return std::make_unique<ElementNode>(identifier.lexeme, std::move(attrs), std::move(children));
}

std::vector<Attribute> Parser::attributes() {
    std::vector<Attribute> attrs;
    while (check(TokenType::Identifier)) {
        Token key = advance();
        if (match(TokenType::Colon) || match(TokenType::Equals)) {
            Token value;
            if (check(TokenType::String) || check(TokenType::UnquotedLiteral) || check(TokenType::Identifier)) {
                value = advance();
            } else {
                 throw std::runtime_error("Expect string, unquoted literal, or identifier as attribute value.");
            }
            consume(TokenType::Semicolon, "Expect ';' after attribute value.");
            attrs.push_back({key.lexeme, value.lexeme});
        } else {
            // If there's no ':' or '=', it's not an attribute, so we backtrack.
            m_current--;
            break;
        }
    }
    return attrs;
}

std::unique_ptr<ASTNode> Parser::styleNode() {
    consume(TokenType::OpenBrace, "Expect '{' after 'style' keyword.");
    std::vector<Attribute> properties = attributes();
    consume(TokenType::CloseBrace, "Expect '}' after style block.");
    return std::make_unique<StyleNode>(std::move(properties));
}

std::unique_ptr<ASTNode> Parser::textNode() {
    // The 'text' keyword is already consumed by declaration()
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
