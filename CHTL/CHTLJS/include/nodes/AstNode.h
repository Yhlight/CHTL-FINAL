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
        // ... 其他 CHTL JS 节点类型
    };

    // CHTL JS AST 节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
        virtual std::unique_ptr<AstNode> clone() const = 0;
    };

} // namespace CHTLJS
