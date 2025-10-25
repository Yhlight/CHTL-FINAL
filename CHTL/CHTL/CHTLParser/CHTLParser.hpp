#ifndef CHTL_PARSER_HPP
#define CHTL_PARSER_HPP

#include "CHTLLexer/CHTLLexer.hpp"
#include "CHTLNode/AstNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/ExpressionNode.hpp"
#include <vector>
#include <memory>
#include <string>

class CHTLParser {
public:
    CHTLParser(const std::string& input);
    std::unique_ptr<AstNode> parse();

private:
    std::unique_ptr<AstNode> parseElement();
    std::string parseTextNode();
    std::unique_ptr<AstNode> parseStyleNode();
    std::unique_ptr<AstNode> parseScriptNode();
    std::unique_ptr<ExpressionNode> parseExpression();
    void parseAttributes(ElementNode* element);

    std::string input_;
    std::vector<Token> tokens_;
    size_t position_ = 0;
    CHTLLexer lexer_;
};

#endif // CHTL_PARSER_HPP
