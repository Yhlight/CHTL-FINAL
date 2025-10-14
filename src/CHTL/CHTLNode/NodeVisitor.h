#pragma once

namespace CHTL {

// 前向声明
class ElementNode;
class TextNode;
class StyleNode;
class ScriptNode;

// 访问者模式 - 用于遍历和处理 AST
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
};

} // namespace CHTL
