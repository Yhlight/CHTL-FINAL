#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

class StyleNode : public BaseNode {
public:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};