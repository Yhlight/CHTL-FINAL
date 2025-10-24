#pragma once

#include "AstNode.h"
#include <string>
#include <memory>

namespace CHTLJS
{
    /**
     * @struct ReactiveValueNode
     * @brief 表示响应式值 $变量名$ 的AST节点
     * 
     * 用于CHTL与CHTL JS之间的交互，允许在CHTL属性和样式中
     * 引用JavaScript变量
     * 
     * 语法: $variableName$
     */
    struct ReactiveValueNode : public AstNode
    {
        std::string variableName;  // 变量名
        
        ReactiveValueNode() {}
        ReactiveValueNode(const std::string& name) : variableName(name) {}

        NodeType GetType() const override { return NodeType::ReactiveValue; }
        std::string ToString() const override { return "ReactiveValueNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ReactiveValueNode> deserialize(std::istream& is);
    };

}
