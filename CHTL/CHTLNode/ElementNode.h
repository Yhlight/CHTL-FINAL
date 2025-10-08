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
};