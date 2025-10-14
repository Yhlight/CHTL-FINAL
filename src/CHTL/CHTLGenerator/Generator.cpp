#include "Generator.h"
#include <algorithm>

namespace CHTL {

Generator::Generator(GeneratorConfig config)
    : config_(config)
    , indentLevel_(0) {}

std::string Generator::generate(const std::vector<NodePtr>& ast) {
    reset();
    
    // 添加 DOCTYPE（如果需要）
    if (config_.includeDoctype) {
        writeLine("<!DOCTYPE html>");
    }
    
    // 遍历所有节点
    for (const auto& node : ast) {
        node->accept(*this);
    }
    
    return getOutput();
}

std::string Generator::generate(const BaseNode* node) {
    reset();
    const_cast<BaseNode*>(node)->accept(*this);
    return getOutput();
}

void Generator::visit(ElementNode& node) {
    generateElement(node);
}

void Generator::visit(TextNode& node) {
    if (!node.getContent().empty()) {
        if (config_.prettyPrint) {
            writeIndent();
        }
        write(escapeHtml(node.getContent()));
        if (config_.prettyPrint) {
            write("\n");
        }
    }
}

void Generator::visit(StyleNode& /* node */) {
    // StyleNode 不直接输出内容
    // 它的内容会被 ElementNode 收集并生成为 style 属性
    // 这里暂时不做处理
}

void Generator::reset() {
    output_.str("");
    output_.clear();
    indentLevel_ = 0;
}

void Generator::writeIndent() {
    if (config_.prettyPrint) {
        output_ << std::string(indentLevel_ * config_.indentSize, ' ');
    }
}

void Generator::writeLine(const std::string& text) {
    if (config_.prettyPrint) {
        writeIndent();
    }
    output_ << text;
    if (config_.prettyPrint) {
        output_ << "\n";
    }
}

void Generator::write(const std::string& text) {
    output_ << text;
}

void Generator::increaseIndent() {
    indentLevel_++;
}

void Generator::decreaseIndent() {
    if (indentLevel_ > 0) {
        indentLevel_--;
    }
}

bool Generator::isSelfClosingTag(const std::string& tagName) const {
    // HTML5 自闭合标签列表
    static const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) 
           != selfClosingTags.end();
}

std::string Generator::escapeHtml(const std::string& text) const {
    std::string result;
    result.reserve(text.length());
    
    for (char c : text) {
        switch (c) {
            case '<':  result += "&lt;"; break;
            case '>':  result += "&gt;"; break;
            case '&':  result += "&amp;"; break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default:   result += c; break;
        }
    }
    
    return result;
}

void Generator::generateElement(ElementNode& element) {
    const std::string& tagName = element.getTagName();
    
    // 写入开始标签
    if (config_.prettyPrint) {
        writeIndent();
    }
    write("<" + tagName);
    
    // 写入属性
    generateAttributes(element.getAttributes());
    
    // 检查是否是自闭合标签或空元素
    bool isEmpty = element.getChildCount() == 0;
    bool isSelfClosing = isSelfClosingTag(tagName);
    
    if (isEmpty && (isSelfClosing || config_.selfClosingTags)) {
        // 自闭合标签
        write(" />");
        if (config_.prettyPrint) {
            write("\n");
        }
    } else {
        // 闭合开始标签
        write(">");
        
        // 处理子节点
        if (element.getChildCount() > 0) {
            if (config_.prettyPrint) {
                write("\n");
            }
            
            increaseIndent();
            
            // 遍历子节点
            for (const auto& child : element.getChildren()) {
                child->accept(*this);
            }
            
            decreaseIndent();
            
            // 写入结束标签
            if (config_.prettyPrint) {
                writeIndent();
            }
        }
        
        write("</" + tagName + ">");
        if (config_.prettyPrint) {
            write("\n");
        }
    }
}

void Generator::generateAttributes(const std::map<std::string, std::string>& attributes) {
    for (const auto& [name, value] : attributes) {
        write(" " + name + "=\"" + escapeHtml(value) + "\"");
    }
}

} // namespace CHTL
