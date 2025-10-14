#pragma once

#include "BaseNode.h"
#include <map>

namespace CHTL {

// 元素节点 - 代表 HTML 元素 (div, span, button 等)
class ElementNode : public BaseNode {
public:
    explicit ElementNode(std::string tagName)
        : BaseNode(NodeType::ELEMENT)
        , tagName_(std::move(tagName)) {}
    
    // 获取标签名
    const std::string& getTagName() const { return tagName_; }
    
    // 添加属性
    void addAttribute(const std::string& name, const std::string& value) {
        attributes_[name] = value;
    }
    
    // 获取属性
    const std::map<std::string, std::string>& getAttributes() const {
        return attributes_;
    }
    
    bool hasAttribute(const std::string& name) const {
        return attributes_.find(name) != attributes_.end();
    }
    
    std::string getAttribute(const std::string& name) const {
        auto it = attributes_.find(name);
        return it != attributes_.end() ? it->second : "";
    }
    
    // 子节点管理
    void addChild(NodePtr child) {
        children_.push_back(std::move(child));
    }
    
    const std::vector<NodePtr>& getChildren() const {
        return children_;
    }
    
    size_t getChildCount() const {
        return children_.size();
    }
    
    // 实现基类虚函数
    std::string toString() const override {
        return "<" + tagName_ + ">";
    }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string tagName_;
    std::map<std::string, std::string> attributes_;
    std::vector<NodePtr> children_;
};

} // namespace CHTL
