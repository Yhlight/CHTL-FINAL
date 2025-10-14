#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/NodeVisitor.h"
#include <string>
#include <sstream>
#include <vector>

namespace CHTL {

// HTML 生成器配置
struct GeneratorConfig {
    bool prettyPrint = true;        // 是否格式化输出
    size_t indentSize = 2;          // 缩进空格数
    bool selfClosingTags = true;    // 是否使用自闭合标签
    bool includeDoctype = false;    // 是否包含 DOCTYPE
};

// HTML 代码生成器
class Generator : public NodeVisitor {
public:
    explicit Generator(GeneratorConfig config = GeneratorConfig{});
    
    // 生成 HTML
    std::string generate(const std::vector<NodePtr>& ast);
    std::string generate(const BaseNode* node);
    
    // 访问者模式实现
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    
    // 获取生成的 HTML
    std::string getOutput() const { return output_.str(); }
    
    // 清空输出
    void reset();

private:
    GeneratorConfig config_;
    std::ostringstream output_;
    size_t indentLevel_;
    
    // 辅助函数
    void writeIndent();
    void writeLine(const std::string& text);
    void write(const std::string& text);
    void increaseIndent();
    void decreaseIndent();
    
    // HTML 辅助函数
    bool isSelfClosingTag(const std::string& tagName) const;
    std::string escapeHtml(const std::string& text) const;
    
    // 生成元素
    void generateElement(ElementNode& element);
    void generateAttributes(const std::map<std::string, std::string>& attributes);
};

} // namespace CHTL
