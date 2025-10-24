#pragma once

#include "AstNode.h"
#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace CHTLJS
{
    /**
     * @struct RouterNode
     * @brief 表示路由系统的AST节点
     * 
     * 支持以下配置:
     * - url + page 映射
     * - root 路径/容器设置
     * - mode (history/hash)
     */
    struct RouterNode : public AstNode
    {
        // URL到页面的映射
        std::vector<std::pair<std::string, std::string>> urlPageMappings;
        
        // 根路径或根容器
        std::string rootPath;
        std::unique_ptr<AstNode> rootContainer;  // EnhancedSelectorNode
        
        // 路由模式
        std::string mode;  // "history" or "hash"
        
        RouterNode() {}

        NodeType GetType() const override { return NodeType::Router; }
        std::string ToString() const override { return "RouterNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<RouterNode> deserialize(std::istream& is);
    };

}
