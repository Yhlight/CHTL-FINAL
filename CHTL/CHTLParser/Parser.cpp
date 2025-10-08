#include "Parser.h"
#include <memory>

namespace CHTL {

Parser::Parser(Lexer& lexer) : m_lexer(lexer), m_curToken({TokenType::ILLEGAL, "", 0, 0}), m_peekToken({TokenType::ILLEGAL, "", 0, 0}) {
    // Read two tokens, so m_curToken and m_peekToken are both set
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

std::unique_ptr<ProgramNode> Parser::ParseProgram() {
    auto program = std::make_unique<ProgramNode>();

    while (m_curToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            program->AddChild(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (m_curToken.type == TokenType::KEYWORD_TEXT) {
        return parseTextStatement();
    } else if (m_curToken.type == TokenType::IDENTIFIER) {
        // This could be an element or an attribute. For now, assume element.
        return parseElementStatement();
    }
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement() {
    auto elementNode = std::make_unique<ElementNode>(m_curToken);

    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{'

    while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            elementNode->AddChild(std::move(stmt));
        }
        nextToken();
    }

    if (m_curToken.type != TokenType::R_BRACE) {
        // Missing closing brace error
        return nullptr;
    }

    return elementNode;
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    Token textToken = m_curToken; // "text" keyword token

    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{'

    if (m_curToken.type != TokenType::STRING_LITERAL && m_curToken.type != TokenType::IDENTIFIER) {
         // For now, we'll support a single identifier as unquoted text
        peekError(TokenType::STRING_LITERAL);
        return nullptr;
    }

    // The actual text content
    std::string textContent = m_curToken.literal;

    auto textNode = std::make_unique<TextNode>(textToken, textContent);

    if (!expectPeek(TokenType::R_BRACE)) {
        return nullptr;
    }

    return textNode;
}


bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

void Parser::peekError(TokenType t) {
    std::string error = "Expected next token to be " + tokenTypeToString(t) +
                        ", got " + tokenTypeToString(m_peekToken.type) + " instead.";
    m_errors.push_back(error);
}

} // namespace CHTL