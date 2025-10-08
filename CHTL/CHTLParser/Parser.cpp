#include "Parser.h"
#include <memory>
#include <sstream>

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
    } else if (m_curToken.type == TokenType::KEYWORD_STYLE) {
        return parseStyleStatement();
    } else if (m_curToken.type == TokenType::IDENTIFIER) {
        if (m_peekToken.type == TokenType::L_BRACE) {
            return parseElementStatement();
        }
    }
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement() {
    auto elementNode = std::make_unique<ElementNode>(m_curToken);

    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{', move to the first token inside the block

    while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        if (m_curToken.type == TokenType::KEYWORD_STYLE) {
            auto styleNode = parseStyleStatement();
            if (styleNode) {
                elementNode->AddChild(std::move(styleNode));
            }
        } else if (m_curToken.type == TokenType::IDENTIFIER && (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUAL)) {
            auto attr = parseAttribute();
            if (attr) {
                elementNode->AddAttribute(*attr);
            }
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                elementNode->AddChild(std::move(stmt));
            }
        }
        nextToken(); // Move to the next statement/attribute
    }

    if (m_curToken.type != TokenType::R_BRACE) {
        peekError(TokenType::R_BRACE);
        return nullptr;
    }

    return elementNode;
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    Token textToken = m_curToken; // "text" keyword token

    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{', move to the first token of the text content

    std::string textContent;

    if (m_curToken.type == TokenType::STRING_LITERAL) {
        // Case 1: Quoted string literal
        textContent = m_curToken.literal;
        nextToken(); // Consume the literal
    } else {
        // Case 2: Unquoted literal (sequence of identifiers, numbers, etc.)
        std::stringstream ss;
        while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
            ss << m_curToken.literal;
            nextToken();
            if (m_curToken.type != TokenType::R_BRACE) {
                ss << " ";
            }
        }
        textContent = ss.str();
    }

    if (m_curToken.type != TokenType::R_BRACE) {
        peekError(TokenType::R_BRACE);
        return nullptr;
    }

    return std::make_unique<TextNode>(textToken, textContent);
}

std::unique_ptr<StyleNode> Parser::parseStyleStatement() {
    auto styleNode = std::make_unique<StyleNode>(m_curToken);

    if (!expectPeek(TokenType::L_BRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{'

    while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        if (m_curToken.type == TokenType::IDENTIFIER) {
            auto prop = parseStylePropertyNode();
            if (prop) {
                styleNode->AddChild(std::move(prop));
            }
        }
        nextToken();
    }

    if (m_curToken.type != TokenType::R_BRACE) {
        peekError(TokenType::R_BRACE);
        return nullptr;
    }

    return styleNode;
}

std::unique_ptr<StylePropertyNode> Parser::parseStylePropertyNode() {
    Token propToken = m_curToken;
    std::string key = m_curToken.literal;

    if (m_peekToken.type != TokenType::COLON) {
        peekError(TokenType::COLON);
        return nullptr;
    }
    nextToken(); // Consume key
    nextToken(); // Consume ':'

    std::stringstream value_ss;
    while (m_curToken.type != TokenType::SEMICOLON) {
        if (m_curToken.type == TokenType::END_OF_FILE || m_curToken.type == TokenType::R_BRACE) {
            m_errors.push_back("Unterminated style property for key: " + key);
            return nullptr;
        }
        value_ss << m_curToken.literal;
        if (m_peekToken.type != TokenType::SEMICOLON) {
            value_ss << " ";
        }
        nextToken();
    }

    std::string value = value_ss.str();
    if (!value.empty() && value.back() == ' ') {
        value.pop_back();
    }
    return std::make_unique<StylePropertyNode>(propToken, key, value);
}

std::unique_ptr<Attribute> Parser::parseAttribute() {
    std::string key = m_curToken.literal;

    if (m_peekToken.type != TokenType::COLON && m_peekToken.type != TokenType::EQUAL) {
        peekError(TokenType::COLON);
        return nullptr;
    }
    nextToken(); // Consume key
    nextToken(); // Consume separator

    std::stringstream value_ss;
    while (m_curToken.type != TokenType::SEMICOLON) {
        if (m_curToken.type == TokenType::END_OF_FILE || m_curToken.type == TokenType::R_BRACE) {
            m_errors.push_back("Unterminated attribute for key: " + key);
            return nullptr;
        }
        value_ss << m_curToken.literal;
        if (m_peekToken.type != TokenType::SEMICOLON) {
            value_ss << " ";
        }
        nextToken();
    }

    std::string value = value_ss.str();
    if (!value.empty() && value.back() == ' ') {
        value.pop_back();
    }
    return std::make_unique<Attribute>(key, value);
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