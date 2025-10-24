#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/ASTNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();

private:
    std::unique_ptr<ASTNode> declaration();
    std::unique_ptr<ASTNode> element();
    std::vector<Attribute> attributes();
    std::unique_ptr<ASTNode> textNode();
    std::unique_ptr<ASTNode> styleNode();

    Token advance();
    bool isAtEnd();
    Token peek();
    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    const std::vector<Token>& m_tokens;
    int m_current = 0;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
