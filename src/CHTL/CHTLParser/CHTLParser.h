#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "ASTNode.h"
#include <memory>

class CHTLParser {
public:
    explicit CHTLParser(CHTLLexer& lexer);
    std::unique_ptr<ProgramNode> parse();

private:
    CHTLLexer& lexer;
    Token currentToken;

    void advance();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseTopLevelStatement();
    std::unique_ptr<ASTNode> parseTextNode();
    std::unique_ptr<StyleNode> parseStyleNode();
    std::unique_ptr<ASTNode> parseStyleTemplateUsage();
    std::unique_ptr<ElementTemplateUsageNode> parseElementTemplateUsage();
    std::unique_ptr<ElementNode> parseElementNode();
    void parseAttributes(ElementNode& element);
    std::unique_ptr<ValueNode> parseValue();
};

#endif //CHTL_PARSER_H
