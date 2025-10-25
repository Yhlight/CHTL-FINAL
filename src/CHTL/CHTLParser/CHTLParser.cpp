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

std::unique_ptr<ASTNode> CHTLParser::parse() {
    return parseTextNode();
}
