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
    size_t start = position_ + 2;
    size_t end = input_.find('\n', start);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(start, end - start);
    position_ = end;
    return {TokenType::COMMENT, value};
}

Token CHTLLexer::tokenizeMultiLineComment() {
    size_t start = position_ + 2;
    size_t end = input_.find("*/", start);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(start, end - start);
    position_ = end + 2;
    return {TokenType::MULTI_LINE_COMMENT, value};
}

Token CHTLLexer::tokenizeGeneratorComment() {
    size_t start = position_ + 2;
    size_t end = input_.find('\n', start);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(start, end - start);
    position_ = end;
    return {TokenType::GENERATOR_COMMENT, value};
}

Token CHTLLexer::tokenizeIdentifier() {
    size_t start = position_;
    while (position_ < input_.size() && (std::isalnum(input_[position_]) || input_[position_] == '_')) {
        position_++;
    }
    return {TokenType::IDENTIFIER, input_.substr(start, position_ - start)};
}

Token CHTLLexer::tokenizeStringLiteral() {
    char quote_type = input_[position_];
    size_t start = position_ + 1;
    size_t end = input_.find(quote_type, start);
    if (end == std::string::npos) {
        end = input_.size();
    }
    std::string value = input_.substr(start, end - start);
    position_ = end + 1;
    return {TokenType::STRING_LITERAL, value};
}

Token CHTLLexer::tokenizePunctuation() {
    char p = input_[position_];
    position_++;
    switch (p) {
        case '{': return {TokenType::LEFT_BRACE, "{"};
        case '}': return {TokenType::RIGHT_BRACE, "}"};
        case ':': return {TokenType::COLON, ":"};
        case ';': return {TokenType::SEMICOLON, ";"};
        case '=': return {TokenType::EQUALS, "="};
        default: return {TokenType::UNKNOWN, std::string(1, p)};
    }
}

Token CHTLLexer::tokenizeNumber() {
    size_t start = position_;
    while (position_ < input_.size() && std::isdigit(input_[position_])) {
        position_++;
    }
    return {TokenType::NUMBER, input_.substr(start, position_ - start)};
}
