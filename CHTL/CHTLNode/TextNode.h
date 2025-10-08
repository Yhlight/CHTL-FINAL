#ifndef CHTL_TEXTNODE_H
#define CHTL_TEXTNODE_H

#include "BaseNode.h"
#include <string>
#include <utility>

namespace CHTL {

class TextNode : public BaseNode {
public:
    TextNode(Token token, std::string text)
        : m_token(std::move(token)), m_text(std::move(text)) {}

    std::string GetTokenLiteral() const override {
        return m_token.literal;
    }

    const std::string& GetText() const {
        return m_text;
    }

    std::string ToString(int indent = 0) const override {
        return indentString(indent) + "\"" + m_text + "\"\n";
    }

private:
    Token m_token;
    std::string m_text;
};

} // namespace CHTL

#endif // CHTL_TEXTNODE_H