#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include <string>
#include <memory>
#include <vector>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual std::string ToString(int indent = 0) const = 0;
    virtual std::string GetTokenLiteral() const = 0;

    void AddChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& GetChildren() const {
        return m_children;
    }

protected:
    std::string indentString(int indent) const {
        return std::string(indent * 2, ' ');
    }

    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL

#endif // CHTL_BASENODE_H