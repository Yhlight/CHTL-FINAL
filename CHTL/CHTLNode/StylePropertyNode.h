#pragma once

#include "BaseNode.h"
#include <string>

class StylePropertyNode : public BaseNode {
public:
    std::string key;
    std::string value;

    StylePropertyNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};