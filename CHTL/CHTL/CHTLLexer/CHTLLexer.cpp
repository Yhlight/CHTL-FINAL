#include "CHTLLexer.hpp"
#include <cctype>

CHTLLexer::CHTLLexer(const std::string& input) : input_(input) {}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;

    while (position_ < input_.size()) {
        skipWhitespace();
        if (position_ >= input_.size()) break;

        if (input_.substr(position_, 2) == "//") {
            tokens.push_back(tokenizeComment());
        } else if (input_.substr(position_, 2) == "/*") {
            tokens.push_back(tokenizeMultiLineComment());
        } else if (input_.substr(position_, 2) == "# ") {
            tokens.push_back(tokenizeGeneratorComment());
        } else if (std::isalpha(input_[position_]) || input_[position_] == '_') {
            tokens.push_back(tokenizeIdentifier());
        } else if (input_[position_] == '"' || input_[position_] == '\'') {
            tokens.push_back(tokenizeStringLiteral());
        } else if (std::isdigit(input_[position_])) {
            tokens.push_back(tokenizeNumber());
        } else if (input_.substr(position_, 2) == "**") {
            size_t start = position_;
            position_ += 2;
            tokens.push_back({TokenType::POWER, "**", "**", start, position_});
        } else {
            tokens.push_back(tokenizePunctuation());
        }
    }

    return tokens;
}

void CHTLLexer::skipWhitespace() {
    while (position_ < input_.size() && std::isspace(input_[position_])) {
        position_++;
    }
}

Token CHTLLexer::tokenizeComment() {
    size_t start = position_;
    position_ += 2;
    size_t end = input_.find('\n', position_);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(position_, end - position_);
    position_ = end;
    return {TokenType::COMMENT, value, input_.substr(start, end - start), start, end};
}

Token CHTLLexer::tokenizeMultiLineComment() {
    size_t start_pos = position_;
    position_ += 2; // Skip "/*"
    size_t value_end_pos = input_.find("*/", position_);
    std::string value;
    if (value_end_pos == std::string::npos) {
        value = input_.substr(position_);
        position_ = input_.size();
    } else {
        value = input_.substr(position_, value_end_pos - position_);
        position_ = value_end_pos + 2;
    }
    return {TokenType::MULTI_LINE_COMMENT, value, input_.substr(start_pos, position_ - start_pos), start_pos, position_};
}

Token CHTLLexer::tokenizeGeneratorComment() {
    size_t start = position_;
    position_ += 2;
    size_t end = input_.find('\n', position_);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(position_, end - position_);
    position_ = end;
    return {TokenType::GENERATOR_COMMENT, value, input_.substr(start, end - start), start, end};
}

Token CHTLLexer::tokenizeIdentifier() {
    size_t start = position_;
    while (position_ < input_.size() && (std::isalnum(input_[position_]) || input_[position_] == '_')) {
        position_++;
    }
    return {TokenType::IDENTIFIER, input_.substr(start, position_ - start), input_.substr(start, position_ - start), start, position_};
}

Token CHTLLexer::tokenizeStringLiteral() {
    size_t start = position_;
    char quote_type = input_[position_];
    position_++;
    size_t end = input_.find(quote_type, position_);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(position_, end - position_);
    position_ = end + 1;
    return {TokenType::STRING_LITERAL, value, input_.substr(start, position_ - start), start, position_};
}

Token CHTLLexer::tokenizePunctuation() {
    size_t start = position_;
    char p = input_[position_];
    position_++;
    std::string raw(1, p);
    switch (p) {
        case '{': return {TokenType::LEFT_BRACE, "{", raw, start, position_};
        case '}': return {TokenType::RIGHT_BRACE, "}", raw, start, position_};
        case ':': return {TokenType::COLON, ":", raw, start, position_};
        case ';': return {TokenType::SEMICOLON, ";", raw, start, position_};
        case '=': return {TokenType::EQUALS, "=", raw, start, position_};
        case '.': return {TokenType::DOT, ".", raw, start, position_};
        case '#': return {TokenType::HASH, "#", raw, start, position_};
        case '&': return {TokenType::AMPERSAND, "&", raw, start, position_};
        case '+': return {TokenType::PLUS, "+", raw, start, position_};
        case '-': return {TokenType::MINUS, "-", raw, start, position_};
        case '*': return {TokenType::STAR, "*", raw, start, position_};
        case '/': return {TokenType::SLASH, "/", raw, start, position_};
        case '%': return {TokenType::PERCENT, "%", raw, start, position_};
        default: return {TokenType::UNKNOWN, raw, raw, start, position_};
    }
}

Token CHTLLexer::tokenizeNumber() {
    size_t start = position_;
    while (position_ < input_.size() && std::isdigit(input_[position_])) {
        position_++;
    }
    return {TokenType::NUMBER, input_.substr(start, position_ - start), input_.substr(start, position_ - start), start, position_};
}
