#pragma once

#include <memory>
#include <string>
#include <vector>

namespace CHTLJS
{
    // CHTL JS AST 节点类型
    enum class NodeType
    {
        Program,
        EnhancedSelector,
        RawJS,
        ScriptLoader,
        Listen,
        Delegate,
        Animate,
        Vir,
        EventBind,
        Router,
        ReactiveValue,
        // ... 其他 CHTL JS 节点类型
    };

    // CHTL JS AST 节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
        virtual std::unique_ptr<AstNode> clone() const = 0;
        virtual void serialize(std::ostream& os) const = 0;
        static std::unique_ptr<AstNode> deserialize(std::istream& is);
    };

    // CHTL JS 程序根节点
    struct ProgramNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override { return "ProgramNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ProgramNode> deserialize(std::istream& is);
    };

} // namespace CHTLJS
