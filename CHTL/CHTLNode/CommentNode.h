#pragma once

#include "BaseNode.h"
#include "../CHTLLexer/Token.h" // For TokenType

class CommentNode : public BaseNode {
public:
    std::string content;
    TokenType type; // To distinguish between single-line, multi-line, and generator comments

    CommentNode(const std::string& content, TokenType type) : content(content), type(type) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};