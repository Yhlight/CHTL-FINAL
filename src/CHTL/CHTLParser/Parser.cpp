#include "Parser.h"
#include <sstream>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens))
    , current_(0) {}

std::vector<NodePtr> Parser::parse() {
    std::vector<NodePtr> statements;
    
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const ParseError& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return statements;
}

Token Parser::peek() const {
    return tokens_[current_];
}

Token Parser::previous() const {
    if (current_ == 0) return tokens_[0];
    return tokens_[current_ - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) const {
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

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    Token current = peek();
    throw ParseError(message, current.line, current.column);
}

NodePtr Parser::parseStatement() {
    // 检查是否是元素节点
    if (isElementStart()) {
        return parseElement();
    }
    
    // 检查是否是 text 块
    if (match(TokenType::KEYWORD_TEXT)) {
        return parseTextBlock();
    }
    
    // 跳过不认识的 token
    advance();
    return nullptr;
}

NodePtr Parser::parseElement() {
    // 获取元素名称（标签名）
    Token tagToken = consume(TokenType::IDENTIFIER, "期望元素名称");
    
    auto element = std::make_unique<ElementNode>(tagToken.value);
    element->setPosition(tagToken.line, tagToken.column);
    
    // 解析元素体 { ... }
    consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 检查属性 (name: value;)
        // 属性名可以是标识符或某些关键字（如 text, style, script）
        if (check(TokenType::IDENTIFIER) || check(TokenType::KEYWORD_TEXT) ||
            check(TokenType::KEYWORD_STYLE) || check(TokenType::KEYWORD_SCRIPT)) {
            Token nameToken = peek();
            size_t savedPos = current_;
            advance(); // 消费标识符或关键字
            
            // 检查是否是属性 (后面跟 : 或 =)
            if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
                advance(); // 消费 : 或 =
                
                // 获取属性值
                std::string value;
                if (match(TokenType::STRING_LITERAL)) {
                    value = previous().value;
                } else if (match(TokenType::IDENTIFIER)) {
                    value = previous().value;
                } else if (match(TokenType::NUMBER)) {
                    value = previous().value;
                } else {
                    throw ParseError("期望属性值", peek().line, peek().column);
                }
                
                // 消费分号（可选）
                match(TokenType::SEMICOLON);
                
                element->addAttribute(nameToken.value, value);
                continue;
            } else {
                // 不是属性，回退
                current_ = savedPos;
            }
        }
        
        // 解析子元素或特殊块
        if (match(TokenType::KEYWORD_TEXT)) {
            auto child = parseTextBlock();
            if (child) element->addChild(std::move(child));
        } else if (match(TokenType::KEYWORD_STYLE)) {
            parseStyleBlock(); // 暂时跳过，不添加到子节点
        } else if (match(TokenType::KEYWORD_SCRIPT)) {
            parseScriptBlock(); // 暂时跳过，不添加到子节点
        } else if (isElementStart()) {
            auto child = parseElement();
            if (child) element->addChild(std::move(child));
        } else {
            // 跳过未知 token
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return element;
}

NodePtr Parser::parseTextBlock() {
    // text 关键字已经被消费
    consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    std::string content;
    
    // 收集文本内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token token = advance();
        if (token.type == TokenType::STRING_LITERAL) {
            content += token.value;
        } else if (token.type == TokenType::IDENTIFIER) {
            content += token.value;
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    auto textNode = std::make_unique<TextNode>(content);
    return textNode;
}

NodePtr Parser::parseStyleBlock() {
    // style 关键字已经被消费
    consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 暂时跳过样式块内容
    int braceCount = 1;
    while (braceCount > 0 && !isAtEnd()) {
        if (match(TokenType::LEFT_BRACE)) {
            braceCount++;
        } else if (match(TokenType::RIGHT_BRACE)) {
            braceCount--;
        } else {
            advance();
        }
    }
    
    // TODO: 实现完整的样式块解析
    return nullptr;
}

NodePtr Parser::parseScriptBlock() {
    // script 关键字已经被消费
    consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 暂时跳过脚本块内容
    int braceCount = 1;
    while (braceCount > 0 && !isAtEnd()) {
        if (match(TokenType::LEFT_BRACE)) {
            braceCount++;
        } else if (match(TokenType::RIGHT_BRACE)) {
            braceCount--;
        } else {
            advance();
        }
    }
    
    // TODO: 实现完整的脚本块解析
    return nullptr;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        // 在分号或右大括号处停止
        if (previous().type == TokenType::SEMICOLON) return;
        if (previous().type == TokenType::RIGHT_BRACE) return;
        
        // 在新语句开始处停止
        if (isElementStart()) return;
        
        advance();
    }
}

void Parser::reportError(const std::string& message) {
    errors_.push_back(message);
}

bool Parser::isElementStart() const {
    // 元素以标识符开始（HTML 标签名）
    // 但不包括关键字
    if (!check(TokenType::IDENTIFIER)) return false;
    
    Token token = peek();
    // 排除一些特殊关键字
    // 这里简单判断，后续可以优化
    return true;
}

} // namespace CHTL
