#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/ExpressionNode.h"
#include <vector>
#include <memory>

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    std::unique_ptr<ExprNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    Token advance();
    Token peek();
    bool isAtEnd();
    Token previous();

    std::unique_ptr<ExprNode> expression();
    std::unique_ptr<ExprNode> term();
    std::unique_ptr<ExprNode> factor();
    std::unique_ptr<ExprNode> primary();
};