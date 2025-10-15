#include "Parser.h"
#include "CHTL/CHTLCommon/CompilerError.h"
#include "CHTL/CHTLCommon/CompilerConfig.h"
#include <sstream>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens))
    , current_(0)
    , currentDepth_(0)
    , currentElement_(nullptr) {
    // 清空全局样式收集器（每次编译开始）
    GlobalStyleCollector::getInstance().clear();
}

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
    // 增加嵌套深度
    currentDepth_++;
    if (currentDepth_ > CompilerConfig::MAX_NESTING_DEPTH) {
        throw ResourceLimitError(
            "元素嵌套深度超限",
            "当前深度: " + std::to_string(currentDepth_) + 
            ", 最大深度: " + std::to_string(CompilerConfig::MAX_NESTING_DEPTH)
        );
    }
    
    // 获取元素名称（标签名）
    Token tagToken = consume(TokenType::IDENTIFIER, "期望元素名称");
    
    auto element = std::make_unique<ElementNode>(tagToken.value);
    element->setPosition(tagToken.line, tagToken.column);
    
    // 保存之前的 currentElement_（支持嵌套）
    ElementNode* previousElement = currentElement_;
    currentElement_ = element.get();
    
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
            // 解析 style 块并添加为子节点
            auto child = parseStyleBlock();
            if (child) element->addChild(std::move(child));
        } else if (match(TokenType::KEYWORD_SCRIPT)) {
            // 解析 script 块并添加为子节点
            auto child = parseScriptBlock();
            if (child) element->addChild(std::move(child));
        } else if (isElementStart()) {
            auto child = parseElement();
            if (child) element->addChild(std::move(child));
        } else {
            // 跳过未知 token
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 检查子节点数量
    if (element->getChildCount() > CompilerConfig::MAX_CHILDREN) {
        throw ResourceLimitError(
            "子节点数量超限",
            "节点 " + element->getTagName() + " 有 " + std::to_string(element->getChildCount()) + " 个子节点"
        );
    }
    
    // 恢复之前的 currentElement_
    currentElement_ = previousElement;
    
    // 减少嵌套深度
    currentDepth_--;
    
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
    
    auto styleNode = std::make_unique<StyleNode>();
    
    // 重置样式上下文
    styleContext_.currentSelector = "";
    styleContext_.hasSelector = false;
    
    // 解析 style 块内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 检查是否是选择器（.box {} 或 #id {} 或 & {}）
        if (isSelector()) {
            // 解析 CSS 规则
            CSSRule rule = parseCSSRule();
            
            // 添加到全局样式收集器
            GlobalStyleCollector::getInstance().addRule(rule.selector, rule.properties);
            
            continue;
        }
        
        // 否则是内联样式，调用 parseInlineStyles()
        // 注意：这里不能调用 parseInlineStyles()，因为它期望在 while 循环内
        // 所以保持原有的内联样式解析逻辑
        
        // 检查是否是 CSS 属性名（标识符）
        if (check(TokenType::IDENTIFIER)) {
            std::string propertyName = advance().value;
            
            // 检查冒号或等号
            if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
                advance(); // 消费 : 或 =
                
                // 检查是否是表达式（包含运算符）
                if (CSSExpression::isExpression(tokens_, current_)) {
                    // 解析并求值表达式
                    try {
                        CSSValue result = CSSExpression::evaluate(tokens_, current_);
                        
                        // 消费分号（如果存在）
                        match(TokenType::SEMICOLON);
                        
                        // 添加求值后的属性
                        styleNode->addProperty(propertyName, result.toString());
                    } catch (const std::exception& e) {
                        // 表达式求值失败，抛出错误
                        throw ParseError(std::string("CSS 表达式求值失败: ") + e.what(), 
                                         tokens_[current_].line, tokens_[current_].column);
                    }
                } else {
                    // 不是表达式，按原来的方式收集属性值
                    std::string propertyValue;
                    TokenType lastTokenType = TokenType::UNKNOWN;
                    std::string lastTokenValue;
                    bool inColorCode = false;  // 跟踪是否在颜色代码中
                    
                    // CSS 单位列表
                    static const std::vector<std::string> cssUnits = {
                        "px", "em", "rem", "pt", "pc", "in", "cm", "mm", "ex", "ch",
                        "vw", "vh", "vmin", "vmax", "deg", "rad", "grad", "turn",
                        "s", "ms", "fr", "dpi", "dpcm", "dppx"
                    };
                    
                    while (!check(TokenType::SEMICOLON) && 
                           !check(TokenType::RIGHT_BRACE) && 
                           !isAtEnd()) {
                        Token valueToken = advance();
                        
                        // 判断是否需要添加空格
                        bool needSpace = false;
                        if (!propertyValue.empty()) {
                            // 在颜色代码中（但不是 # 本身） → 不加空格
                            if (inColorCode && !(valueToken.type == TokenType::HASH)) {
                                needSpace = false;
                                // 颜色代码通常是 #fff 或 #aabbcc，读取完整个颜色后结束
                                // 简单判断：如果当前token不是NUMBER也不是IDENTIFIER，颜色代码结束
                                if (valueToken.type != TokenType::NUMBER && 
                                    valueToken.type != TokenType::IDENTIFIER &&
                                    valueToken.type != TokenType::HASH) {
                                    inColorCode = false;
                                }
                            }
                            // 数字 + 百分号 → 不加空格
                            else if (lastTokenType == TokenType::NUMBER && 
                                     valueToken.type == TokenType::PERCENT) {
                                needSpace = false;
                            }
                            // 数字 + CSS 单位 → 不加空格
                            else if (lastTokenType == TokenType::NUMBER && 
                                     valueToken.type == TokenType::IDENTIFIER) {
                                // 检查是否是 CSS 单位
                                bool isUnit = std::find(cssUnits.begin(), cssUnits.end(), 
                                                       valueToken.value) != cssUnits.end();
                                needSpace = !isUnit;  // 是单位则不加空格，否则加空格
                            }
                            // 其他情况需要空格
                            else {
                                needSpace = true;
                            }
                        }
                        
                        if (needSpace) {
                            propertyValue += " ";
                        }
                        
                        propertyValue += valueToken.value;
                        lastTokenType = valueToken.type;
                        lastTokenValue = valueToken.value;
                        
                        // 检测颜色代码开始（# 符号）
                        if (valueToken.type == TokenType::HASH) {
                            inColorCode = true;
                        }
                        
                        // 颜色代码读取完毕后重置标志
                        // 如果刚才添加的是NUMBER或IDENTIFIER，且我们在颜色代码中，继续
                        // 否则，如果下一个token不是NUMBER/IDENTIFIER，结束颜色代码
                        if (inColorCode && valueToken.type != TokenType::HASH) {
                            // 先peek下一个token
                            if (current_ < tokens_.size() && 
                                tokens_[current_].type != TokenType::NUMBER &&
                                tokens_[current_].type != TokenType::IDENTIFIER) {
                                inColorCode = false;
                            }
                        }
                    }
                    
                    // 消费分号（如果存在）
                    match(TokenType::SEMICOLON);
                    
                    // 添加属性
                    if (!propertyValue.empty()) {
                        styleNode->addProperty(propertyName, propertyValue);
                    }
                }
            } else {
                // 不是有效的属性语法，跳过
                advance();
            }
        } else {
            // 跳过非标识符 token
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return styleNode;
}

NodePtr Parser::parseScriptBlock() {
    // script 关键字已经被消费
    consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto scriptNode = std::make_unique<ScriptNode>();
    
    // 收集 JavaScript 代码
    // 我们需要保留所有内容，包括花括号（对于 JavaScript 函数等）
    std::string scriptContent;
    int braceCount = 1;  // 已经消费了左花括号
    
    while (braceCount > 0 && !isAtEnd()) {
        Token token = peek();
        
        if (token.type == TokenType::LEFT_BRACE) {
            scriptContent += "{";
            braceCount++;
            advance();
        } else if (token.type == TokenType::RIGHT_BRACE) {
            if (braceCount == 1) {
                // 这是 script 块的结束花括号
                break;
            }
            scriptContent += "}";
            braceCount--;
            advance();
        } else {
            // 添加其他 token
            if (!scriptContent.empty() && 
                token.type != TokenType::SEMICOLON &&
                token.type != TokenType::LEFT_PAREN &&
                token.type != TokenType::RIGHT_PAREN &&
                token.type != TokenType::LEFT_BRACKET &&
                token.type != TokenType::RIGHT_BRACKET) {
                // 大多数情况需要空格分隔
                if (scriptContent.back() != ' ' && 
                    scriptContent.back() != '{' &&
                    scriptContent.back() != '(' &&
                    scriptContent.back() != '[') {
                    scriptContent += " ";
                }
            }
            
            scriptContent += token.value;
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    scriptNode->setContent(scriptContent);
    return scriptNode;
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

// 检查当前 token 是否是选择器开始（. 或 # 或 &）
bool Parser::isSelector() const {
    return check(TokenType::DOT) || check(TokenType::HASH) || check(TokenType::AMPERSAND);
}

// 解析 CSS 选择器
CSSSelector Parser::parseSelector() {
    if (match(TokenType::DOT)) {
        // 类选择器：.box
        Token nameToken = consume(TokenType::IDENTIFIER, "期望类名");
        return CSSSelector(SelectorType::CLASS, nameToken.value);
    }
    
    if (match(TokenType::HASH)) {
        // id 选择器：#header
        Token nameToken = consume(TokenType::IDENTIFIER, "期望 id 名");
        return CSSSelector(SelectorType::ID, nameToken.value);
    }
    
    if (match(TokenType::AMPERSAND)) {
        // & 上下文引用
        // 检查是否跟着伪类或伪元素
        if (check(TokenType::COLON)) {
            advance(); // 消费 :
            
            // 检查是否是伪元素（::）
            if (check(TokenType::COLON)) {
                advance(); // 消费第二个 :
                // 伪元素名可以是标识符或某些关键字（如 before, after）
                Token nameToken = advance();
                if (nameToken.type != TokenType::IDENTIFIER && 
                    nameToken.type != TokenType::KEYWORD_BEFORE &&
                    nameToken.type != TokenType::KEYWORD_AFTER) {
                    throw ParseError("期望伪元素名", nameToken.line, nameToken.column);
                }
                return CSSSelector(SelectorType::PSEUDO_ELEMENT, nameToken.value);
            } else {
                // 伪类（:）
                Token nameToken = advance();
                if (nameToken.type != TokenType::IDENTIFIER) {
                    throw ParseError("期望伪类名", nameToken.line, nameToken.column);
                }
                return CSSSelector(SelectorType::PSEUDO_CLASS, nameToken.value);
            }
        } else if (check(TokenType::DOUBLE_COLON)) {
            // :: 伪元素
            advance(); // 消费 ::
            // 伪元素名可以是标识符或某些关键字（如 before, after）
            Token nameToken = advance();
            if (nameToken.type != TokenType::IDENTIFIER && 
                nameToken.type != TokenType::KEYWORD_BEFORE &&
                nameToken.type != TokenType::KEYWORD_AFTER) {
                throw ParseError("期望伪元素名", nameToken.line, nameToken.column);
            }
            return CSSSelector(SelectorType::PSEUDO_ELEMENT, nameToken.value);
        } else {
            // 单独的 &，引用当前上下文
            // 返回一个特殊的选择器，稍后处理
            return CSSSelector(SelectorType::CLASS, "&");  // 临时标记
        }
    }
    
    throw ParseError("无效的选择器", peek().line, peek().column);
}

// 解析 CSS 规则块
CSSRule Parser::parseCSSRule() {
    // 解析选择器
    CSSSelector selector = parseSelector();
    
    // 构建选择器字符串
    std::string selectorStr;
    bool updateContext = false;
    
    if (selector.type == SelectorType::CLASS) {
        if (selector.name == "&") {
            // 单独的 &，使用当前上下文
            if (!styleContext_.hasSelector) {
                throw ParseError("& 必须在类或 id 选择器之后使用", peek().line, peek().column);
            }
            selectorStr = styleContext_.currentSelector;
        } else {
            // 普通类选择器
            selectorStr = "." + selector.name;
            updateContext = true;
            
            // 自动为当前元素添加类名
            if (currentElement_) {
                std::string className = selector.name;
                if (!currentElement_->hasAttribute("class")) {
                    currentElement_->addAttribute("class", className);
                } else {
                    // 追加到现有类名
                    std::string existingClass = currentElement_->getAttribute("class");
                    currentElement_->addAttribute("class", existingClass + " " + className);
                }
            }
        }
    } else if (selector.type == SelectorType::ID) {
        selectorStr = "#" + selector.name;
        updateContext = true;
        
        // 自动为当前元素添加 id
        if (currentElement_) {
            std::string idName = selector.name;
            if (!currentElement_->hasAttribute("id")) {
                currentElement_->addAttribute("id", idName);
            } else {
                // id 应该是唯一的，如果已存在则报告警告（但仍然覆盖）
                // 这里简单处理：覆盖现有 id
                currentElement_->addAttribute("id", idName);
            }
        }
    } else if (selector.type == SelectorType::PSEUDO_CLASS) {
        // 伪类：&:hover → .box:hover
        if (!styleContext_.hasSelector) {
            throw ParseError("伪类必须在类或 id 选择器之后使用", peek().line, peek().column);
        }
        selectorStr = styleContext_.currentSelector + ":" + selector.name;
    } else if (selector.type == SelectorType::PSEUDO_ELEMENT) {
        // 伪元素：&::before → .box::before
        if (!styleContext_.hasSelector) {
            throw ParseError("伪元素必须在类或 id 选择器之后使用", peek().line, peek().column);
        }
        selectorStr = styleContext_.currentSelector + "::" + selector.name;
    }
    
    // 更新上下文（如果是新的类/id选择器）
    if (updateContext) {
        styleContext_.currentSelector = selectorStr;
        styleContext_.hasSelector = true;
    }
    
    CSSRule rule(selectorStr);
    
    // 解析规则块 { ... }
    consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析 CSS 属性
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER)) {
            std::string propertyName = advance().value;
            
            if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
                advance(); // 消费 : 或 =
                
                // 检查是否是表达式（包含运算符）
                if (CSSExpression::isExpression(tokens_, current_)) {
                    // 解析并求值表达式
                    try {
                        CSSValue result = CSSExpression::evaluate(tokens_, current_);
                        
                        // 消费分号（如果存在）
                        match(TokenType::SEMICOLON);
                        
                        // 添加求值后的属性
                        rule.properties[propertyName] = result.toString();
                    } catch (const std::exception& e) {
                        // 表达式求值失败，跳过
                        while (!check(TokenType::SEMICOLON) && 
                               !check(TokenType::RIGHT_BRACE) && 
                               !isAtEnd()) {
                            advance();
                        }
                        match(TokenType::SEMICOLON);
                    }
                } else {
                    // 不是表达式，收集属性值（使用与内联样式相同的逻辑）
                    std::string propertyValue;
                    TokenType lastTokenType = TokenType::UNKNOWN;
                    bool inColorCode = false;
                    
                    // CSS 单位列表
                    static const std::vector<std::string> cssUnits = {
                        "px", "em", "rem", "pt", "pc", "in", "cm", "mm", "ex", "ch",
                        "vw", "vh", "vmin", "vmax", "deg", "rad", "grad", "turn",
                        "s", "ms", "fr", "dpi", "dpcm", "dppx"
                    };
                    
                    while (!check(TokenType::SEMICOLON) && 
                           !check(TokenType::RIGHT_BRACE) && 
                           !isAtEnd()) {
                        Token valueToken = advance();
                        
                        // 判断是否需要添加空格
                        bool needSpace = false;
                        if (!propertyValue.empty()) {
                            // 在颜色代码中 → 不加空格
                            if (inColorCode && !(valueToken.type == TokenType::HASH)) {
                                needSpace = false;
                                if (valueToken.type != TokenType::NUMBER && 
                                    valueToken.type != TokenType::IDENTIFIER &&
                                    valueToken.type != TokenType::HASH) {
                                    inColorCode = false;
                                }
                            }
                            // 数字 + 百分号 → 不加空格
                            else if (lastTokenType == TokenType::NUMBER && 
                                     valueToken.type == TokenType::PERCENT) {
                                needSpace = false;
                            }
                            // 数字 + CSS 单位 → 不加空格
                            else if (lastTokenType == TokenType::NUMBER && 
                                     valueToken.type == TokenType::IDENTIFIER) {
                                bool isUnit = std::find(cssUnits.begin(), cssUnits.end(), 
                                                       valueToken.value) != cssUnits.end();
                                needSpace = !isUnit;
                            }
                            // 其他情况需要空格
                            else {
                                needSpace = true;
                            }
                        }
                        
                        if (needSpace) {
                            propertyValue += " ";
                        }
                        
                        propertyValue += valueToken.value;
                        lastTokenType = valueToken.type;
                        
                        // 检测颜色代码开始（# 符号）
                        if (valueToken.type == TokenType::HASH) {
                            inColorCode = true;
                        }
                        
                        // 颜色代码读取完毕后重置标志
                        if (inColorCode && valueToken.type != TokenType::HASH) {
                            if (current_ < tokens_.size() && 
                                tokens_[current_].type != TokenType::NUMBER &&
                                tokens_[current_].type != TokenType::IDENTIFIER) {
                                inColorCode = false;
                            }
                        }
                    }
                    
                    // 消费分号（如果存在）
                    match(TokenType::SEMICOLON);
                    
                    // 添加属性到规则
                    if (!propertyValue.empty()) {
                        rule.properties[propertyName] = propertyValue;
                    }
                }
            }
        } else {
            advance(); // 跳过非标识符
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return rule;
}

// 解析内联样式（原有的内联样式逻辑）
void Parser::parseInlineStyles(StyleNode* styleNode) {
    // 解析 CSS 属性：property-name: value;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 检查是否是 CSS 属性名（标识符）
        if (check(TokenType::IDENTIFIER)) {
            std::string propertyName = advance().value;
            
            // 检查冒号或等号
            if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
                advance(); // 消费 : 或 =
                
                // 检查是否是表达式（包含运算符）
                if (CSSExpression::isExpression(tokens_, current_)) {
                    // 解析并求值表达式
                    try {
                        CSSValue result = CSSExpression::evaluate(tokens_, current_);
                        
                        // 消费分号（如果存在）
                        match(TokenType::SEMICOLON);
                        
                        // 添加求值后的属性
                        styleNode->addProperty(propertyName, result.toString());
                    } catch (const std::exception& e) {
                        // 表达式求值失败，抛出错误
                        throw ParseError(std::string("CSS 表达式求值失败: ") + e.what(), 
                                         tokens_[current_].line, tokens_[current_].column);
                    }
                } else {
                    // 不是表达式，按原来的方式收集属性值
                    std::string propertyValue;
                    TokenType lastTokenType = TokenType::UNKNOWN;
                    std::string lastTokenValue;
                    bool inColorCode = false;  // 跟踪是否在颜色代码中
                    
                    // CSS 单位列表
                    static const std::vector<std::string> cssUnits = {
                        "px", "em", "rem", "pt", "pc", "in", "cm", "mm", "ex", "ch",
                        "vw", "vh", "vmin", "vmax", "deg", "rad", "grad", "turn",
                        "s", "ms", "fr", "dpi", "dpcm", "dppx"
                    };
                    
                    while (!check(TokenType::SEMICOLON) && 
                           !check(TokenType::RIGHT_BRACE) && 
                           !isAtEnd()) {
                        Token valueToken = advance();
                        
                        // 判断是否需要添加空格
                        bool needSpace = false;
                        if (!propertyValue.empty()) {
                            // 在颜色代码中（但不是 # 本身） → 不加空格
                            if (inColorCode && !(valueToken.type == TokenType::HASH)) {
                                needSpace = false;
                                if (valueToken.type != TokenType::NUMBER && 
                                    valueToken.type != TokenType::IDENTIFIER &&
                                    valueToken.type != TokenType::HASH) {
                                    inColorCode = false;
                                }
                            }
                            // 数字 + 百分号 → 不加空格
                            else if (lastTokenType == TokenType::NUMBER && 
                                     valueToken.type == TokenType::PERCENT) {
                                needSpace = false;
                            }
                            // 数字 + CSS 单位 → 不加空格
                            else if (lastTokenType == TokenType::NUMBER && 
                                     valueToken.type == TokenType::IDENTIFIER) {
                                // 检查是否是 CSS 单位
                                bool isUnit = std::find(cssUnits.begin(), cssUnits.end(), 
                                                       valueToken.value) != cssUnits.end();
                                needSpace = !isUnit;  // 是单位则不加空格，否则加空格
                            }
                            // 其他情况需要空格
                            else {
                                needSpace = true;
                            }
                        }
                        
                        if (needSpace) {
                            propertyValue += " ";
                        }
                        
                        propertyValue += valueToken.value;
                        lastTokenType = valueToken.type;
                        lastTokenValue = valueToken.value;
                        
                        // 检测颜色代码开始（在添加token之后）
                        if (valueToken.type == TokenType::UNKNOWN && valueToken.value == "#") {
                            inColorCode = true;
                        }
                        
                        // 颜色代码读取完毕后重置标志
                        if (inColorCode && valueToken.type != TokenType::UNKNOWN) {
                            // 先peek下一个token
                            if (current_ < tokens_.size() && 
                                tokens_[current_].type != TokenType::NUMBER &&
                                tokens_[current_].type != TokenType::IDENTIFIER) {
                                inColorCode = false;
                            }
                        }
                    }
                    
                    // 消费分号（如果存在）
                    match(TokenType::SEMICOLON);
                    
                    // 添加属性
                    if (!propertyValue.empty()) {
                        styleNode->addProperty(propertyName, propertyValue);
                    }
                }
            } else {
                // 不是有效的属性语法，跳过
                advance();
            }
        } else {
            // 跳过非标识符 token
            advance();
        }
    }
}

} // namespace CHTL
