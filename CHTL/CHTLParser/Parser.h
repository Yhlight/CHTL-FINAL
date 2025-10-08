#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/StatementNodes.h"
#include "../CHTLNode/ExpressionNodes.h"
#include <vector>
#include <string>
#include <memory>

class Parser {
public:
    Parser(Lexer& l);

    std::unique_ptr<Program> ParseProgram();
    const std::vector<std::string>& Errors() const { return errors; }

private:
    void nextToken();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ElementStatement> parseElementStatement();
    std::unique_ptr<TextStatement> parseTextStatement();
    std::unique_ptr<AttributeStatement> parseAttributeStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<Expression> parseExpression();

    Lexer& l;
    Token curToken;
    Token peekToken;
    std::vector<std::string> errors;
};