#include "CHTLParser.h"
#include <iostream>

CHTLParser::CHTLParser(CHTLLexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

void CHTLParser::advance() {
    currentToken = lexer.getNextToken();
}

void CHTLParser::parseAttributes(ElementNode& element) {
    while (currentToken.type == TokenType::IDENTIFIER) {
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
                }
            }
        } else {
            break;
        }
    }
}

std::unique_ptr<ASTNode> CHTLParser::parseTextNode() {
    if (currentToken.value == "text") {
        advance();
        if (currentToken.type == TokenType::LBRACE) {
            advance();
            if (currentToken.type == TokenType::STRING) {
                std::string text = currentToken.value;
                advance();
                if (currentToken.type == TokenType::RBRACE) {
                    advance();
                    return std::make_unique<TextNode>(text);
                }
            }
        }
    }
    return nullptr;
}

std::unique_ptr<StyleTemplateUsageNode> CHTLParser::parseStyleTemplateUsage() {
    advance(); // consume '@'
    if (currentToken.value == "Style") {
        advance(); // consume 'Style'
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string name = currentToken.value;
            advance();
            if (currentToken.type == TokenType::SEMICOLON) {
                advance();
            }
            return std::make_unique<StyleTemplateUsageNode>(name);
        }
    }
    return nullptr;
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleNode() {
    auto styleNode = std::make_unique<StyleNode>();
    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string key = currentToken.value;
            advance();
            if (currentToken.type == TokenType::COLON) {
                advance();
                bool isString = currentToken.type == TokenType::STRING;
                if (currentToken.type == TokenType::IDENTIFIER || currentToken.type == TokenType::STRING) {
                    std::string value = currentToken.value;
                    advance();
                    styleNode->addItem(std::make_unique<StylePropertyNode>(key, value, isString));
                    if (currentToken.type == TokenType::SEMICOLON) {
                        advance();
                    }
                }
            }
        } else if (currentToken.type == TokenType::AT) {
            styleNode->addItem(parseStyleTemplateUsage());
        } else {
            advance();
        }
    }
    return styleNode;
}

std::unique_ptr<ASTNode> CHTLParser::parseTemplate() {
    advance(); // consume '['
    if (currentToken.value == "Template") {
        advance(); // consume 'Template'
        if (currentToken.type == TokenType::RBRACKET) {
            advance(); // consume ']'
            if (currentToken.type == TokenType::AT) {
                advance(); // consume '@'
                if (currentToken.value == "Style") {
                    advance(); // consume 'Style'
                    if (currentToken.type == TokenType::IDENTIFIER) {
                        std::string name = currentToken.value;
                        advance();
                        if (currentToken.type == TokenType::LBRACE) {
                            advance();
                            auto styleNode = parseStyleNode();
                            if (currentToken.type == TokenType::RBRACE) {
                                advance();
                                return std::make_unique<StyleTemplateNode>(name, std::move(styleNode));
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string tag = currentToken.value;
        advance();
        auto elementNode = std::make_unique<ElementNode>(tag);
        if (currentToken.type == TokenType::LBRACE) {
            advance();
            parseAttributes(*elementNode);
            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                elementNode->addChild(parseStatement());
            }
            if (currentToken.type == TokenType::RBRACE) {
                advance();
                return elementNode;
            }
        }
    }
    return nullptr;
}

std::unique_ptr<ASTNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::LBRACKET) {
        return parseTemplate();
    } else if (currentToken.value == "text") {
        return parseTextNode();
    } else if (currentToken.value == "style") {
        advance(); // consume 'style'
        if (currentToken.type == TokenType::LBRACE) {
            advance(); // consume '{'
            auto styleNode = parseStyleNode();
            if (currentToken.type == TokenType::RBRACE) {
                advance(); // consume '}'
            }
            return styleNode;
        }
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        return parseElementNode();
    }

    if (currentToken.type != TokenType::END_OF_FILE) {
        advance();
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
