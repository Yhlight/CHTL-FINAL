#pragma once

#include "AST.h"
#include "../CHTLLexer/Token.h"
#include "../../SharedCore/Document.h"
#include "Expression.h"
#include <vector>
#include <string>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    Document parse();
    const std::vector<std::string>& getErrors() const;

private:
    void skipComments();
    std::unique_ptr<Node> parseElement(Document& doc);
    void parseAttributes(ElementNode* element);
    void parseStyleBlock(ElementNode* element, Document& doc);
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Node> parseText();
    void error(const std::string& message);

    const std::vector<Token>& tokens;
    int current = 0;
    std::vector<std::string> errors;
};
