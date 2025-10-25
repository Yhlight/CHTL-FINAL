#include "CHTLParser.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TextNode.hpp"
#include "CHTLNode/StyleNode.hpp"
#include "CHTLNode/PropertyNode.hpp"

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::unique_ptr<AstNode> CHTLParser::parse() {
    if (position_ >= tokens_.size()) {
        return nullptr;
    }

    if (tokens_[position_].type == TokenType::IDENTIFIER) {
        if (tokens_[position_].value == "text") {
            return std::make_unique<TextNode>(parseTextNode());
        } else if (tokens_[position_].value == "style") {
            return parseStyleNode();
        } else {
            return parseElement();
        }
    } else if (tokens_[position_].type == TokenType::STRING_LITERAL) {
        return std::make_unique<TextNode>(tokens_[position_++].value);
    }

    return nullptr;
}

std::unique_ptr<AstNode> CHTLParser::parseElement() {
    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::IDENTIFIER) {
        return nullptr;
    }

    std::string tag_name = tokens_[position_].value;
    position_++;

    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::LEFT_BRACE) {
        return nullptr;
    }
    position_++;

    auto element = std::make_unique<ElementNode>(tag_name);

    while (position_ < tokens_.size() && tokens_[position_].type != TokenType::RIGHT_BRACE) {
        if (tokens_[position_ + 1].type == TokenType::COLON) {
            parseAttributes(element.get());
        } else {
            element->children.push_back(parse());
        }
    }

    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::RIGHT_BRACE) {
        return nullptr;
    }
    position_++;

    return element;
}

std::string CHTLParser::parseTextNode() {
    position_++; // Consume 'text' identifier

    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::LEFT_BRACE) {
        return "";
    }
    position_++;

    std::string text = "";
    if (position_ < tokens_.size() && tokens_[position_].type == TokenType::STRING_LITERAL) {
        text = tokens_[position_].value;
        position_++;
    }

    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::RIGHT_BRACE) {
        return "";
    }
    position_++;

    return text;
}

void CHTLParser::parseAttributes(ElementNode* element) {
    std::string key = tokens_[position_].value;
    position_++; // consume identifier
    position_++; // consume colon

    if (position_ < tokens_.size() && (tokens_[position_].type == TokenType::STRING_LITERAL || tokens_[position_].type == TokenType::IDENTIFIER)) {
        std::string value = tokens_[position_].value;
        position_++; // consume value

        if (position_ < tokens_.size() && tokens_[position_].type == TokenType::SEMICOLON) {
            position_++; // consume semicolon
            element->attributes[key] = value;
        }
    }
}

std::unique_ptr<AstNode> CHTLParser::parseStyleNode() {
    position_++; // Consume 'style' identifier

    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::LEFT_BRACE) {
        return nullptr;
    }
    position_++;

    auto styleNode = std::make_unique<StyleNode>();

    while (position_ < tokens_.size() && tokens_[position_].type != TokenType::RIGHT_BRACE) {
        if (tokens_[position_].type == TokenType::DOT || tokens_[position_].type == TokenType::HASH) {
            std::string selector = tokens_[position_].value;
            position_++; // consume dot or hash
            selector += tokens_[position_].value;
            position_++; // consume identifier
            position_++; // consume left brace

            auto ruleNode = std::make_unique<RuleNode>(selector);

            while (position_ < tokens_.size() && tokens_[position_].type != TokenType::RIGHT_BRACE) {
                std::string key = tokens_[position_].value;
                position_++; // consume identifier
                position_++; // consume colon

                std::string value = "";
                while (position_ < tokens_.size() && tokens_[position_].type != TokenType::SEMICOLON) {
                    value += tokens_[position_].value;
                    position_++;
                }
                position_++; // consume semicolon

                ruleNode->properties.push_back(std::make_unique<PropertyNode>(key, value));
            }
            position_++; // consume right brace
            styleNode->rules.push_back(std::move(ruleNode));
        } else {
            std::string key = tokens_[position_].value;
            position_++; // consume identifier
            position_++; // consume colon

            std::string value = "";
            while (position_ < tokens_.size() && tokens_[position_].type != TokenType::SEMICOLON) {
                value += tokens_[position_].value;
                position_++;
            }
            position_++; // consume semicolon

            styleNode->properties.push_back(std::make_unique<PropertyNode>(key, value));
        }
    }

    if (position_ >= tokens_.size() || tokens_[position_].type != TokenType::RIGHT_BRACE) {
        return nullptr;
    }
    position_++;

    return styleNode;
}
