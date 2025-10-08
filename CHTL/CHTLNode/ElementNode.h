#pragma once

#include "BaseNode.h"
#include "StyleNode.h"
#include <map>
#include <memory>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::unique_ptr<StyleNode> style;
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(const std::string& tagName) : tagName(tagName), style(nullptr) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<ElementNode>(tagName);
        newNode->attributes = attributes;
        if (style) {
            newNode->style = std::unique_ptr<StyleNode>(static_cast<StyleNode*>(style->clone().release()));
        }
        for (const auto& child : children) {
            newNode->children.push_back(child->clone());
        }
        return newNode;
    }
};