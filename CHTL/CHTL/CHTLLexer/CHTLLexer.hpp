#ifndef CHTL_LEXER_HPP
#define CHTL_LEXER_HPP

#include <string>
#include <vector>

enum class TokenType {
    COMMENT,
    MULTI_LINE_COMMENT,
    GENERATOR_COMMENT,
    IDENTIFIER,
    STRING_LITERAL,
    LEFT_BRACE,
    RIGHT_BRACE,
    COLON,
    SEMICOLON,
    EQUALS,
    NUMBER,
    DOT,
    HASH,
    AMPERSAND,
    UNKNOWN,
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
    void skipWhitespace();
    Token tokenizeComment();
    Token tokenizeMultiLineComment();
    Token tokenizeGeneratorComment();
    Token tokenizeIdentifier();
    Token tokenizeStringLiteral();
    Token tokenizePunctuation();
    Token tokenizeNumber();

    std::string input_;
    size_t position_ = 0;
};

#endif // CHTL_LEXER_HPP
