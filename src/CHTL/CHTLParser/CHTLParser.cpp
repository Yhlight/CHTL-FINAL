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
            element.addAttribute(key, parseValue());
            if (currentToken.type == TokenType::SEMICOLON) {
                advance(); // consume ';'
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

std::unique_ptr<ASTNode> CHTLParser::parseScriptNode() {
    advance(); // consume 'script'
    if (currentToken.type == TokenType::LBRACE) {
        lexer.setMode(LexerMode::SCRIPT_CONTENT);
        advance(); // consume '{' to get the script content
        std::string content = currentToken.value;
        advance(); // this should now be '}'
        if (currentToken.type == TokenType::RBRACE) {
            advance(); // consume '}'
        }
        return std::make_unique<ScriptNode>(content);
    }
    return nullptr;
}

std::unique_ptr<ASTNode> CHTLParser::parseNamespaceStatement() {
    advance(); // consume '['
    advance(); // consume 'Namespace'
    advance(); // consume ']'

    if (currentToken.type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    std::string name = currentToken.value;
    advance();

    if (currentToken.type != TokenType::LBRACE) {
        return nullptr;
    }
    advance(); // consume '{'

    std::vector<std::unique_ptr<ASTNode>> body;
    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        body.push_back(parseStatement());
    }

    if (currentToken.type == TokenType::RBRACE) {
        advance();
    }

    return std::make_unique<NamespaceNode>(name, std::move(body));
}

std::unique_ptr<ASTNode> CHTLParser::parseStyleTemplateUsage() {
    advance(); // consume '@'
    if (currentToken.value == "Style") {
        advance(); // consume 'Style'
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string name = currentToken.value;
            advance();
            std::string from;
            if (currentToken.type == TokenType::FROM) {
                advance();
                if (currentToken.type == TokenType::IDENTIFIER) {
                    from = currentToken.value;
                    advance();
                }
            }
            if (currentToken.type == TokenType::LBRACE) {
                advance();
                auto styleNode = parseStyleNode();
                if (currentToken.type == TokenType::RBRACE) {
                    advance();
                }
                return std::make_unique<CustomStyleUsageNode>(name, std::move(styleNode), from);
            } else {
                if (currentToken.type == TokenType::SEMICOLON) {
                    advance();
                }
                return std::make_unique<StyleTemplateUsageNode>(name, from);
            }
        }
    }
    return nullptr;
}

std::unique_ptr<ASTNode> CHTLParser::parseElementUsage() {
    advance(); // consume '@'
    if (currentToken.value == "Element") {
        advance(); // consume 'Element'
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string name = currentToken.value;
            advance();
            std::string from;
            if (currentToken.type == TokenType::FROM) {
                advance();
                if (currentToken.type == TokenType::IDENTIFIER) {
                    from = currentToken.value;
                    advance();
                }
            }
            if (currentToken.type == TokenType::LBRACE) {
                advance();
                std::vector<std::unique_ptr<ASTNode>> body;
                while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                    body.push_back(parseStatement());
                }
                if (currentToken.type == TokenType::RBRACE) {
                    advance();
                }
                return std::make_unique<CustomElementUsageNode>(name, std::move(body), from);
            } else {
                if (currentToken.type == TokenType::SEMICOLON) {
                    advance();
                }
                return std::make_unique<ElementTemplateUsageNode>(name, from);
            }
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
                styleNode->addItem(std::make_unique<StylePropertyNode>(key, parseValue()));
                if (currentToken.type == TokenType::SEMICOLON) {
                    advance();
                }
            } else {
                styleNode->addItem(std::make_unique<StylePropertyNode>(key, nullptr));
                 if (currentToken.type == TokenType::SEMICOLON) {
                    advance();
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

std::unique_ptr<ASTNode> CHTLParser::parseTopLevelStatement() {
    advance(); // consume '['
    std::string blockType = currentToken.value;
    advance();
    if (currentToken.type == TokenType::RBRACKET) {
        advance();
        if (currentToken.type == TokenType::AT) {
            advance();
            if (blockType == "Template") {
                 if (currentToken.value == "Style") {
                    advance();
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
                } else if (currentToken.value == "Element") {
                    advance();
                    if (currentToken.type == TokenType::IDENTIFIER) {
                        std::string name = currentToken.value;
                        advance();
                        if (currentToken.type == TokenType::LBRACE) {
                            advance();
                            std::vector<std::unique_ptr<ASTNode>> body;
                            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                                body.push_back(parseStatement());
                            }
                            if (currentToken.type == TokenType::RBRACE) {
                                advance();
                                return std::make_unique<ElementTemplateNode>(name, std::move(body));
                            }
                        }
                    }
                } else if (currentToken.type == TokenType::VAR) {
                    return parseVarTemplate();
                }
            } else if (blockType == "Custom") {
                if (currentToken.value == "Style") {
                    advance();
                    if (currentToken.type == TokenType::IDENTIFIER) {
                        std::string name = currentToken.value;
                        advance();
                        if (currentToken.type == TokenType::LBRACE) {
                            advance();
                            auto styleNode = parseStyleNode();
                            if (currentToken.type == TokenType::RBRACE) {
                                advance();
                                return std::make_unique<CustomStyleTemplateNode>(name, std::move(styleNode));
                            }
                        }
                    }
                } else if (currentToken.value == "Element") {
                    advance();
                    if (currentToken.type == TokenType::IDENTIFIER) {
                        std::string name = currentToken.value;
                        advance();
                        if (currentToken.type == TokenType::LBRACE) {
                            advance();
                            std::vector<std::unique_ptr<ASTNode>> body;
                            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                                body.push_back(parseStatement());
                            }
                            if (currentToken.type == TokenType::RBRACE) {
                                advance();
                                return std::make_unique<CustomElementNode>(name, std::move(body));
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
        int index = -1;
        if (currentToken.type == TokenType::LBRACKET) {
            advance();
            if (currentToken.type == TokenType::NUMBER) {
                index = std::stoi(currentToken.value);
                advance();
            }
            if (currentToken.type == TokenType::RBRACKET) {
                advance();
            }
        }
        auto elementNode = std::make_unique<ElementNode>(tag, index);
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
        Token peeked = lexer.peekToken();
        if (peeked.value == "Import") {
            return parseImportStatement();
        } else if (peeked.type == TokenType::NAMESPACE) {
            return parseNamespaceStatement();
        }
        return parseTopLevelStatement();
    } else if (currentToken.value == "text") {
        return parseTextNode();
    } else if (currentToken.type == TokenType::SCRIPT) {
        return parseScriptNode();
    } else if (currentToken.value == "style") {
        advance();
        if (currentToken.type == TokenType::LBRACE) {
            advance();
            auto styleNode = parseStyleNode();
            if (currentToken.type == TokenType::RBRACE) {
                advance();
            }
            return styleNode;
        }
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        return parseElementNode();
    } else if (currentToken.type == TokenType::AT) {
        Token peeked = lexer.peekToken();
        if (peeked.value == "Element") {
            return parseElementUsage();
        } else if (peeked.value == "Style") {
            return parseStyleTemplateUsage();
        }
    }

    if (currentToken.type != TokenType::END_OF_FILE) {
        advance();
    }
    return nullptr;
}

std::unique_ptr<ASTNode> CHTLParser::parseImportStatement() {
    advance(); // consume '['
    if (currentToken.value != "Import") {
        return nullptr;
    }
    advance(); // consume 'Import'
    if (currentToken.type != TokenType::RBRACKET) {
        return nullptr;
    }
    advance(); // consume ']'

    if (currentToken.type != TokenType::AT) {
        return nullptr;
    }
    advance(); // consume '@'

    std::string type = currentToken.value;
    advance();

    if (currentToken.type != TokenType::FROM) {
        return nullptr;
    }
    advance(); // consume 'from'

    if (currentToken.type != TokenType::STRING) {
        return nullptr;
    }
    std::string path = currentToken.value;
    advance();

    std::string alias;
    if (currentToken.type == TokenType::AS) {
        advance();
        if (currentToken.type == TokenType::IDENTIFIER) {
            alias = currentToken.value;
            advance();
        }
    }

    if (currentToken.type == TokenType::SEMICOLON) {
        advance();
    }

    return std::make_unique<ImportNode>(type, path, alias);
}

std::unique_ptr<ValueNode> CHTLParser::parseValue() {
    // Handle template var usage first, as it starts with an identifier
    if (currentToken.type == TokenType::IDENTIFIER) {
        Token peeked = lexer.peekToken();
        if (peeked.type == TokenType::LPAREN) {
            std::string name = currentToken.value;
            advance(); // consume name
            advance(); // consume '('
            if (currentToken.type == TokenType::IDENTIFIER) {
                std::string varName = currentToken.value;
                advance();
                if (currentToken.type == TokenType::RPAREN) {
                    advance();
                    return std::make_unique<TemplateVarUsageNode>(name, varName);
                }
            }
        }
    }

    // Handle single string literal
    if (currentToken.type == TokenType::STRING) {
        std::string value = currentToken.value;
        advance();
        return std::make_unique<LiteralValueNode>(value, true);
    }

    // Handle other values (concatenated identifiers, numbers, etc.)
    std::string value;
    while (currentToken.type != TokenType::SEMICOLON && currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        value += currentToken.value; // Concatenate without space
        advance();
    }

    if (!value.empty()) {
        return std::make_unique<LiteralValueNode>(value);
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

std::unique_ptr<VarTemplateNode> CHTLParser::parseVarTemplate() {
    advance(); // consume 'Var'
    if (currentToken.type == TokenType::IDENTIFIER) {
        auto varTemplate = std::make_unique<VarTemplateNode>(currentToken.value);
        advance();
        if (currentToken.type == TokenType::LBRACE) {
            advance();
            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                if (currentToken.type == TokenType::IDENTIFIER) {
                    std::string key = currentToken.value;
                    advance();
                    if (currentToken.type == TokenType::COLON) {
                        advance();
                        varTemplate->addVariable(key, parseValue());
                        if (currentToken.type == TokenType::SEMICOLON) {
                            advance();
                        }
                    }
                } else {
                    advance();
                }
            }
            if (currentToken.type == TokenType::RBRACE) {
                advance();
                return varTemplate;
            }
        }
    }
    return nullptr;
}
