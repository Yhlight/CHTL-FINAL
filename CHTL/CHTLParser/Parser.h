#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

namespace CHTL {

class ProgramNode : public BaseNode {
public:
    ProgramNode() = default;

    std::string GetTokenLiteral() const override { return "Program"; }

    std::string ToString(int indent = 0) const override {
        std::string out;
        for (const auto& child : m_children) {
            out += child->ToString(indent);
        }
        return out;
    }
};

enum Precedence {
    LOWEST,
    CONDITIONAL, // ?
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    CALL,        // myFunction(X)
    INDEX        // array[index]
};


class Parser {
public:
    explicit Parser(Lexer& lexer);

    std::unique_ptr<ProgramNode> ParseProgram();

    const std::vector<std::string>& Errors() const { return m_errors; }

private:
    void nextToken();
    bool expectPeek(TokenType t);
    void peekError(TokenType t);

    // Statement Parsers
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementStatement();
    std::unique_ptr<TextNode> parseTextStatement();
    std::unique_ptr<StyleNode> parseStyleStatement();
    std::unique_ptr<StylePropertyNode> parseStylePropertyNode();
    std::unique_ptr<Attribute> parseAttribute();

    // Expression Parsers
    std::unique_ptr<ExpressionNode> parseExpression(Precedence precedence);
    std::unique_ptr<ExpressionNode> parseIdentifier();
    std::unique_ptr<ExpressionNode> parseNumericLiteral();
    std::unique_ptr<ExpressionNode> parseStringLiteral();
    std::unique_ptr<ExpressionNode> parseInfixExpression(std::unique_ptr<ExpressionNode> left);
    std::unique_ptr<ExpressionNode> parseConditionalExpression(std::unique_ptr<ExpressionNode> condition);

    // Pratt parser infrastructure
    using prefixParseFn = std::unique_ptr<ExpressionNode> (Parser::*)();
    using infixParseFn = std::unique_ptr<ExpressionNode> (Parser::*)(std::unique_ptr<ExpressionNode>);

    void registerPrefix(TokenType, prefixParseFn);
    void registerInfix(TokenType, infixParseFn);

    Precedence peekPrecedence();
    Precedence curPrecedence();

    std::map<TokenType, prefixParseFn> m_prefixParseFns;
    std::map<TokenType, infixParseFn> m_infixParseFns;
    static const std::map<TokenType, Precedence> precedences;


    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};

} // namespace CHTL

#endif // CHTL_PARSER_H