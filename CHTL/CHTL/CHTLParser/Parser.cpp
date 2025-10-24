#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::unique_ptr<ASTNode> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> nodes;
    while (!isAtEnd()) {
        if (check(TokenType::Text)) {
            nodes.push_back(textNode());
        } else {
            // In the future, we will handle other node types here.
            // For now, we'll just advance to avoid an infinite loop.
            advance();
        }
    }
    return std::make_unique<ElementNode>("root", std::move(nodes));
}

std::unique_ptr<ASTNode> Parser::textNode() {
    consume(TokenType::Text, "Expect 'text' keyword.");
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message);
}

} // namespace CHTL
