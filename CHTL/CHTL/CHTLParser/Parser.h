#pragma once

#include "AST.h"
#include "../CHTLLexer/Token.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Node> parse();

private:
    void skipComments();
    std::unique_ptr<Node> parseElement();
    void parseAttributes(ElementNode* element);
    void parseStyleBlock(ElementNode* element);
    std::unique_ptr<Node> parseText();

    const std::vector<Token>& tokens;
    int current = 0;
};
