#pragma once

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <map>
#include <string>

namespace CHTL {

// CSS 样式节点
// 表示 style {} 块中的样式声明
class StyleNode : public BaseNode {
public:
    StyleNode() : BaseNode(NodeType::STYLE_BLOCK) {}
    
    // 添加 CSS 属性
    void addProperty(const std::string& name, const std::string& value) {
        properties_[name] = value;
    }
    
    // 获取所有 CSS 属性
    const std::map<std::string, std::string>& getProperties() const {
        return properties_;
    }
    
    // 获取单个属性值
    std::string getProperty(const std::string& name) const {
        auto it = properties_.find(name);
        return it != properties_.end() ? it->second : "";
    }
    
    // 检查是否有某个属性
    bool hasProperty(const std::string& name) const {
        return properties_.find(name) != properties_.end();
    }
    
    // 获取属性数量
    size_t getPropertyCount() const {
        return properties_.size();
    }
    
    // 转换为字符串（调试用）
    std::string toString() const override {
        std::string result = "StyleNode {\n";
        for (const auto& [name, value] : properties_) {
            result += "  " + name + ": " + value + ";\n";
        }
        result += "}";
        return result;
    }
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

private:
    std::map<std::string, std::string> properties_;  // CSS 属性映射
};

} // namespace CHTL
