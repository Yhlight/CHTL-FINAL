#pragma once

#include "AST.h"
#include "../CHTLLexer/Token.h"
#include "../../SharedCore/Document.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    Document parse();

private:
    void skipComments();
    std::unique_ptr<Node> parseElement(Document& doc);
    void parseAttributes(ElementNode* element);
    void parseStyleBlock(ElementNode* element, Document& doc);
    std::unique_ptr<Node> parseText();

    const std::vector<Token>& tokens;
    int current = 0;
};
