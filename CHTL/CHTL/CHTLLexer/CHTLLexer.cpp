#include "CHTLLexer.hpp"

CHTLLexer::CHTLLexer(const std::string& input) : input_(input) {}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;

    while (position_ < input_.size()) {
        if (input_.substr(position_, 2) == "//") {
            size_t start = position_ + 2;
            size_t end = input_.find('\n', start);
            if (end == std::string::npos) {
                end = input_.size();
            }
            tokens.push_back({TokenType::COMMENT, input_.substr(start, end - start)});
            position_ = end;
        } else {
            position_++;
        }
    }

    return tokens;
}
