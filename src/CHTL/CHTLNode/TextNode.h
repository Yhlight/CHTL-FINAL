#pragma once

#include "BaseNode.h"

namespace CHTL {

// 文本节点 - 代表文本内容
class TextNode : public BaseNode {
public:
    explicit TextNode(std::string content)
        : BaseNode(NodeType::TEXT)
        , content_(std::move(content)) {}
    
    // 获取文本内容
    const std::string& getContent() const { return content_; }
    
    // 设置文本内容
    void setContent(std::string content) {
        content_ = std::move(content);
    }
    
    // 检查是否为空
    bool isEmpty() const {
        return content_.empty();
    }
    
    // 实现基类虚函数
    std::string toString() const override {
        return "Text: \"" + content_ + "\"";
    }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string content_;
};

} // namespace CHTL
