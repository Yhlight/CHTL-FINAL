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

void Generator::visit(ScriptNode& node) {
    // 生成 <script> 标签
    if (config_.prettyPrint) {
        writeIndent();
    }
    
    write("<script>");
    
    if (!node.getContent().empty()) {
        if (config_.prettyPrint) {
            write("\n");
            increaseIndent();
        }
        
        // 输出脚本内容（可能需要缩进每一行）
        if (config_.prettyPrint) {
            std::istringstream iss(node.getContent());
            std::string line;
            while (std::getline(iss, line)) {
                writeIndent();
                write(line);
                write("\n");
            }
            decreaseIndent();
            writeIndent();
        } else {
            write(node.getContent());
        }
    }
    
    write("</script>");
    
    if (config_.prettyPrint) {
        write("\n");
    }
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
    
    // 收集内联样式（从 StyleNode 子节点）
    std::string inlineStyle;
    std::vector<const BaseNode*> nonStyleChildren;
    
    for (const auto& child : element.getChildren()) {
        if (child->getType() == NodeType::STYLE_BLOCK) {
            // 收集 style 属性
            auto* styleNode = dynamic_cast<StyleNode*>(child.get());
            if (styleNode) {
                for (const auto& [name, value] : styleNode->getProperties()) {
                    if (!inlineStyle.empty()) {
                        inlineStyle += "; ";
                    }
                    inlineStyle += name + ": " + value;
                }
            }
        } else {
            // 非 style 节点，保留用于后续处理（包括 script 块）
            nonStyleChildren.push_back(child.get());
        }
    }
    
    // 写入开始标签
    if (config_.prettyPrint) {
        writeIndent();
    }
    write("<" + tagName);
    
    // 写入普通属性
    generateAttributes(element.getAttributes());
    
    // 写入内联样式属性
    if (!inlineStyle.empty()) {
        write(" style=\"" + inlineStyle + "\"");
    }
    
    // 检查是否是自闭合标签或空元素
    bool isEmpty = nonStyleChildren.empty();
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
        
        // 处理非 style 子节点
        if (!nonStyleChildren.empty()) {
            if (config_.prettyPrint) {
                write("\n");
            }
            
            increaseIndent();
            
            // 遍历非 style 子节点
            for (const auto* child : nonStyleChildren) {
                const_cast<BaseNode*>(child)->accept(*this);
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
