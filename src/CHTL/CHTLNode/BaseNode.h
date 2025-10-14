#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ostream>

namespace CHTL {

// AST 节点类型
enum class NodeType {
    ELEMENT,        // 元素节点 (div, span, etc.)
    TEXT,           // 文本节点
    ATTRIBUTE,      // 属性节点
    STYLE_BLOCK,    // 样式块节点
    SCRIPT_BLOCK,   // 脚本块节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    IMPORT,         // 导入节点
    NAMESPACE,      // 命名空间节点
    COMMENT,        // 注释节点
};

// AST 基础节点类
class BaseNode {
public:
    explicit BaseNode(NodeType type) : type_(type), line_(0), column_(0) {}
    virtual ~BaseNode() = default;
    
    // 禁止拷贝，允许移动
    BaseNode(const BaseNode&) = delete;
    BaseNode& operator=(const BaseNode&) = delete;
    BaseNode(BaseNode&&) = default;
    BaseNode& operator=(BaseNode&&) = default;
    
    // 获取节点类型
    NodeType getType() const { return type_; }
    
    // 设置/获取位置信息
    void setPosition(size_t line, size_t column) {
        line_ = line;
        column_ = column;
    }
    
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    // 纯虚函数 - 子类必须实现
    virtual std::string toString() const = 0;
    virtual void accept(class NodeVisitor& visitor) = 0;

protected:
    NodeType type_;
    size_t line_;
    size_t column_;
};

// 节点智能指针类型
using NodePtr = std::unique_ptr<BaseNode>;

// 辅助函数：将 NodeType 转换为字符串
const char* nodeTypeToString(NodeType type);

} // namespace CHTL
