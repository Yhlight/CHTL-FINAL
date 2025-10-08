#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include <vector>
#include <string>
#include <memory>

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


class Parser {
public:
    explicit Parser(Lexer& lexer);

    std::unique_ptr<ProgramNode> ParseProgram();

    const std::vector<std::string>& Errors() const { return m_errors; }

private:
    void nextToken();
    bool expectPeek(TokenType t);
    void peekError(TokenType t);

    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementStatement();
    std::unique_ptr<TextNode> parseTextStatement();
    std::unique_ptr<StyleNode> parseStyleStatement();
    std::unique_ptr<StylePropertyNode> parseStylePropertyNode();
    std::unique_ptr<Attribute> parseAttribute();

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};

} // namespace CHTL

#endif // CHTL_PARSER_H