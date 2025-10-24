#pragma once

#include "AstNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS
{
    /**
     * @struct EventBindNode
     * @brief 表示事件绑定操作符 &-> 的AST节点
     * 
     * 支持以下语法形式：
     * 1. 单事件: {{box}} &-> click: () => {};
     * 2. 多事件: {{box}} &-> click, mouseenter: () => {};
     * 3. 链式: {{box}} &-> click: fn1, &-> mouseenter: fn2;
     * 4. 块: {{box}} &-> { click: fn1, mouseenter: fn2 }
     */
    struct EventBindNode : public AstNode
    {
        std::unique_ptr<AstNode> target;      // 目标DOM对象 (EnhancedSelectorNode)
        std::vector<std::string> eventNames;  // 事件名称列表 (支持多事件)
        std::string handler;                  // 事件处理函数
        bool isBlock;                         // 是否是块形式
        std::vector<std::pair<std::vector<std::string>, std::string>> blockEvents; // 块中的事件列表

        EventBindNode() : isBlock(false) {}

        NodeType GetType() const override { return NodeType::EventBind; }
        std::string ToString() const override { return "EventBindNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<EventBindNode> deserialize(std::istream& is);
    };

}
