#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility>

namespace CHTL {

class Attribute {
public:
    Attribute(std::string key, std::string value)
        : m_key(std::move(key)), m_value(std::move(value)) {}

    std::string ToString() const {
        return m_key + ": " + m_value + ";";
    }

private:
    std::string m_key;
    std::string m_value;
};

class ElementNode : public BaseNode {
public:
    ElementNode(Token token) : m_token(std::move(token)) {}

    std::string GetTokenLiteral() const override {
        return m_token.literal;
    }

    void AddAttribute(const Attribute& attr) {
        m_attributes.push_back(attr);
    }

    std::string ToString(int indent = 0) const override {
        std::string out = indentString(indent) + "<" + m_token.literal + ">\n";
        for (const auto& attr : m_attributes) {
            out += indentString(indent + 1) + attr.ToString() + "\n";
        }
        for (const auto& child : m_children) {
            out += child->ToString(indent + 1);
        }
        return out;
    }

private:
    Token m_token;
    std::vector<Attribute> m_attributes;
};

} // namespace CHTL

#endif // CHTL_ELEMENTNODE_H