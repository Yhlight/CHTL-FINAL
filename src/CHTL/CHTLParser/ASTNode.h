#ifndef CHTL_ASTNODE_H
#define CHTL_ASTNODE_H

#include <string>
#include <vector>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
};

class TextNode : public ASTNode {
public:
    explicit TextNode(std::string text) : text(std::move(text)) {}
    std::string toString() const override {
        return "TextNode(\"" + text + "\")";
    }

private:
    std::string text;
};

#endif //CHTL_ASTNODE_H
