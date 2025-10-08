#pragma once

#include "BaseNode.h"

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& content) : content(content) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TextNode>(content);
    }
};