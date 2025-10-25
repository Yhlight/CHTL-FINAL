#ifndef CHTL_PARSER_HPP
#define CHTL_PARSER_HPP

#include "CHTLLexer/CHTLLexer.hpp"
#include "CHTLNode/AstNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include <vector>
#include <memory>

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<AstNode> parse();

private:
    std::unique_ptr<AstNode> parseElement();
    std::unique_ptr<AstNode> parseTextNode();
    void parseAttributes(ElementNode* element);

    std::vector<Token> tokens_;
    size_t position_ = 0;
};

#endif // CHTL_PARSER_HPP
