#include "CHTLParser.h"
#include <iostream>

CHTLParser::CHTLParser(CHTLLexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

void CHTLParser::advance() {
    currentToken = lexer.getNextToken();
}

void CHTLParser::parseAttributes(ElementNode& element) {
    while (currentToken.type == TokenType::IDENTIFIER) {
        // Lookahead to see if this is an attribute or a child element.
        Token peeked = lexer.peekToken();
        if (peeked.type == TokenType::COLON || peeked.type == TokenType::EQUALS) {
            std::string key = currentToken.value;
            advance(); // consume key
            advance(); // consume ':' or '='
            if (currentToken.type == TokenType::STRING || currentToken.type == TokenType::IDENTIFIER) {
                element.addAttribute(key, currentToken.value);
                advance(); // consume value
                if (currentToken.type == TokenType::SEMICOLON) {
                    advance(); // consume ';'
                } else {
                    // This is an error, but for now we'll just continue.
                }
            }
        } else {
            // Not an attribute, so break out of the loop.
            break;
        }
    }
}

std::unique_ptr<ASTNode> CHTLParser::parseTextNode() {
    if (currentToken.value == "text") {
        advance(); // consume 'text'
        if (currentToken.type == TokenType::LBRACE) {
            advance(); // consume '{'
            if (currentToken.type == TokenType::STRING) {
                std::string text = currentToken.value;
                advance(); // consume string
                if (currentToken.type == TokenType::RBRACE) {
                    advance(); // consume '}'
                    return std::make_unique<TextNode>(text);
                }
            }
        }
    }
    return nullptr;
}

std::unique_ptr<ASTNode> CHTLParser::parseStyleNode() {
    if (currentToken.value == "style") {
        advance(); // consume 'style'
        if (currentToken.type == TokenType::LBRACE) {
            advance(); // consume '{'
            std::string content = "";
            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                content += currentToken.value;
                advance();
            }
            if (currentToken.type == TokenType::RBRACE) {
                advance(); // consume '}'
                return std::make_unique<StyleNode>(content);
            }
        }
    }
    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string tag = currentToken.value;
        advance(); // consume tag
        auto elementNode = std::make_unique<ElementNode>(tag);
        if (currentToken.type == TokenType::LBRACE) {
            advance(); // consume '{'
            parseAttributes(*elementNode);
            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                elementNode->addChild(parseStatement());
            }
            if (currentToken.type == TokenType::RBRACE) {
                advance(); // consume '}'
                return elementNode;
            }
        }
    }
    return nullptr;
}

std::unique_ptr<ASTNode> CHTLParser::parseStatement() {
    if (currentToken.value == "text") {
        return parseTextNode();
    } else if (currentToken.value == "style") {
        return parseStyleNode();
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        return parseElementNode();
    }

    if (currentToken.type != TokenType::END_OF_FILE) {
        advance(); // Move past unexpected tokens for now
    }
    return nullptr;
}

std::unique_ptr<ProgramNode> CHTLParser::parse() {
    auto programNode = std::make_unique<ProgramNode>();
    while (currentToken.type != TokenType::END_OF_FILE) {
        programNode->addStatement(parseStatement());
    }
    return programNode;
}
