#include "CHTLJSLexer.hpp"

CHTLJSLexer::CHTLJSLexer(const std::string& input) : input_(input) {}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;

    while (position_ < input_.size()) {
        if (input_.substr(position_, 2) == "{{") {
            size_t start = position_ + 2;
            size_t end = input_.find("}}", start);
            if (end != std::string::npos) {
                tokens.push_back({CHTLJSTokenType::ENHANCED_SELECTOR, input_.substr(start, end - start), position_, end + 2});
                position_ = end + 2;
            } else {
                // Handle error: unclosed selector
                tokens.push_back({CHTLJSTokenType::UNKNOWN, input_.substr(position_), position_, input_.size()});
                position_ = input_.size();
            }
        } else if (input_.substr(position_, 2) == "->") {
            tokens.push_back({CHTLJSTokenType::ARROW_OPERATOR, "->", position_, position_ + 2});
            position_ += 2;
        } else if (input_.substr(position_, 3) == "&->") {
            tokens.push_back({CHTLJSTokenType::BIND_OPERATOR, "&->", position_, position_ + 3});
            position_ += 3;
        } else if (input_[position_] == '$') {
            size_t start = position_ + 1;
            size_t end = input_.find('$', start);
            if (end != std::string::npos) {
                tokens.push_back({CHTLJSTokenType::REACTIVE_VALUE, input_.substr(start, end - start), position_, end + 1});
                position_ = end + 1;
            } else {
                // Handle error: unclosed reactive value
                tokens.push_back({CHTLJSTokenType::UNKNOWN, input_.substr(position_), position_, input_.size()});
                position_ = input_.size();
            }
        } else if (isalpha(input_[position_])) {
            size_t start = position_;
            while (position_ < input_.size() && isalnum(input_[position_])) {
                position_++;
            }
            std::string value = input_.substr(start, position_ - start);
            if (value == "Listen") {
                tokens.push_back({CHTLJSTokenType::KEYWORD_LISTEN, value, start, position_});
            } else if (value == "Delegate") {
                tokens.push_back({CHTLJSTokenType::KEYWORD_DELEGATE, value, start, position_});
            } else if (value == "Animate") {
                tokens.push_back({CHTLJSTokenType::KEYWORD_ANIMATE, value, start, position_});
            } else if (value == "Vir") {
                tokens.push_back({CHTLJSTokenType::KEYWORD_VIR, value, start, position_});
            } else if (value == "Router") {
                tokens.push_back({CHTLJSTokenType::KEYWORD_ROUTER, value, start, position_});
            } else if (value == "ScriptLoader") {
                tokens.push_back({CHTLJSTokenType::KEYWORD_SCRIPTLOADER, value, start, position_});
            } else {
                tokens.push_back({CHTLJSTokenType::UNKNOWN, value, start, position_});
            }
        } else {
            position_++;
        }
    }

    return tokens;
}
