#pragma once

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <algorithm>

namespace CHTL {

/**
 * @brief 表示 script {} 块的 AST 节点
 * 
 * ScriptNode 存储 JavaScript 代码，并在生成 HTML 时
 * 输出为 <script> 标签
 */
class ScriptNode : public BaseNode {
public:
    ScriptNode() : BaseNode(NodeType::SCRIPT_BLOCK) {}
    
    /**
     * @brief 设置脚本内容
     */
    void setContent(const std::string& content) {
        content_ = content;
    }
    
    /**
     * @brief 获取脚本内容
     */
    const std::string& getContent() const {
        return content_;
    }
    
    /**
     * @brief 添加一行脚本代码
     */
    void appendLine(const std::string& line) {
        if (!content_.empty()) {
            content_ += "\n";
        }
        content_ += line;
    }
    
    /**
     * @brief 检查脚本是否为空
     */
    bool isEmpty() const {
        return content_.empty();
    }
    
    /**
     * @brief 获取脚本行数
     */
    size_t getLineCount() const {
        if (content_.empty()) return 0;
        return std::count(content_.begin(), content_.end(), '\n') + 1;
    }
    
    std::string toString() const override {
        return "ScriptNode{" + std::to_string(content_.length()) + " chars}";
    }
    
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

private:
    std::string content_;  // JavaScript 代码
};

} // namespace CHTL
