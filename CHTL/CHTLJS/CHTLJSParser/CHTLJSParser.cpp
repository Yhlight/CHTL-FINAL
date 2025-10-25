#include "CHTLJSParser.hpp"
#include "../CHTLJSNode/EnhancedSelectorNode.hpp"
#include "../CHTLJSNode/ReactiveValueNode.hpp"
#include "../CHTLJSNode/MethodCallNode.hpp"

CHTLJSParser::CHTLJSParser(CHTLJSLexer& lexer) {
    tokens_ = lexer.tokenize();
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parse() {
    return parseExpression();
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parsePrimary() {
    if (position_ >= tokens_.size()) {
        return nullptr;
    }

    const CHTLJSToken& current_token = tokens_[position_];

    if (current_token.type == CHTLJSTokenType::ENHANCED_SELECTOR) {
        position_++;
        return std::make_unique<EnhancedSelectorNode>(current_token.value);
    } else if (current_token.type == CHTLJSTokenType::REACTIVE_VALUE) {
        position_++;
        return std::make_unique<ReactiveValueNode>(current_token.value);
    }

    return nullptr;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseExpression() {
    auto left = parsePrimary();

    while (position_ < tokens_.size()) {
        const CHTLJSToken& current_token = tokens_[position_];

        if (current_token.type == CHTLJSTokenType::ARROW_OPERATOR) {
            position_++; // Consume '->'
            if (position_ < tokens_.size() && tokens_[position_].type == CHTLJSTokenType::IDENTIFIER) {
                std::string method_name = tokens_[position_].value;
                position_++; // Consume identifier
                left = std::make_unique<MethodCallNode>(std::move(left), method_name);
            } else {
                // Handle error: expected identifier after '->'
                return nullptr;
            }
        } else {
            break;
        }
    }

    return left;
}
