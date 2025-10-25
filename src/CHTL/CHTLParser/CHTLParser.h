#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "ASTNode.h"
#include <memory>

class CHTLParser {
public:
    explicit CHTLParser(CHTLLexer& lexer);
    std::unique_ptr<ASTNode> parse();

private:
    CHTLLexer& lexer;
    Token currentToken;

    void advance();
    std::unique_ptr<ASTNode> parseTextNode();
};

#endif //CHTL_PARSER_H
