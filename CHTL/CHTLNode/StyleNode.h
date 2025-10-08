#ifndef CHTL_STYLENODE_H
#define CHTL_STYLENODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <vector>
#include <utility>

namespace CHTL {

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(Token token, std::string key, std::unique_ptr<ExpressionNode> value)
        : m_token(std::move(token)), m_key(std::move(key)), m_value(std::move(value)) {}

    std::string GetTokenLiteral() const override {
        return m_token.literal;
    }

    std::string ToString(int indent = 0) const override {
        std::string value_str = m_value ? m_value->ToString(0) : "null";
        return indentString(indent) + m_key + ": " + value_str + ";\n";
    }

private:
    Token m_token;
    std::string m_key;
    std::unique_ptr<ExpressionNode> m_value;
};

class StyleNode : public BaseNode {
public:
    StyleNode(Token token) : m_token(std::move(token)) {}

    std::string GetTokenLiteral() const override {
        return m_token.literal;
    }

    std::string ToString(int indent = 0) const override {
        std::string out = indentString(indent) + "style {\n";
        for (const auto& child : m_children) {
            out += child->ToString(indent + 1);
        }
        out += indentString(indent) + "}\n";
        return out;
    }

private:
    Token m_token;
};

} // namespace CHTL

#endif // CHTL_STYLENODE_H