#include "CHTLParser.h"
#include <iostream>

CHTLParser::CHTLParser(CHTLLexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

void CHTLParser::advance() {
    currentToken = lexer.getNextToken();
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

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string tag = currentToken.value;
        advance(); // consume tag
        auto elementNode = std::make_unique<ElementNode>(tag);
        if (currentToken.type == TokenType::LBRACE) {
            advance(); // consume '{'
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
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        return parseElementNode();
    }
    // In the future, we will handle other statement types here.
    advance(); // Move past unexpected tokens for now
    return nullptr;
}

std::unique_ptr<ProgramNode> CHTLParser::parse() {
    auto programNode = std::make_unique<ProgramNode>();
    while (currentToken.type != TokenType::END_OF_FILE) {
        programNode->addStatement(parseStatement());
    }
    return programNode;
}
