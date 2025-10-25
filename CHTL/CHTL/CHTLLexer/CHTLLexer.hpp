#ifndef CHTL_LEXER_HPP
#define CHTL_LEXER_HPP

#include <string>
#include <vector>

enum class TokenType {
    COMMENT,
    // Add other token types here
};

struct Token {
    TokenType type;
    std::string value;
};

class CHTLLexer {
public:
    CHTLLexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string input_;
    size_t position_ = 0;
};

#endif // CHTL_LEXER_HPP
