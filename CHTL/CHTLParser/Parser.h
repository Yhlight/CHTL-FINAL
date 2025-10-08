#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens, CHTLContext& context);
    std::unique_ptr<RootNode> parse();

private:
    std::vector<Token> tokens;
    CHTLContext& context;
    size_t current = 0;

    Token advance();
    Token peek();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    void parseAttribute(ElementNode* element);
    std::unique_ptr<StyleNode> parseStyle();
    std::unique_ptr<StyleRuleNode> parseStyleRule();
    std::unique_ptr<BaseNode> parseTemplateOrCustomDeclaration();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();
    std::unique_ptr<DeleteNode> parseDeleteStatement();
    std::unique_ptr<InsertNode> parseInsertStatement();
};