#include "Generator.h"
#include <algorithm>

namespace CHTL {

Generator::Generator(GeneratorConfig config)
    : config_(config)
    , indentLevel_(0)
    , globalStylesInjected_(false) {}

std::string Generator::generate(const std::vector<NodePtr>& ast) {
    reset();
    globalStylesInjected_ = false;
    
    // 如果有全局样式，确保它们被注入到 <head>
    if (GlobalStyleCollector::getInstance().hasRules()) {
        ensureGlobalStylesInjected(const_cast<std::vector<NodePtr>&>(ast));
    }
    
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
    
    // 特殊处理：如果是 <head> 并且有全局样式，不能使用自闭合标签
    bool needsGlobalStyles = (tagName == "head" && !globalStylesInjected_ && 
                              GlobalStyleCollector::getInstance().hasRules());
    
    if (isEmpty && (isSelfClosing || config_.selfClosingTags) && !needsGlobalStyles) {
        // 自闭合标签
        write(" />");
        if (config_.prettyPrint) {
            write("\n");
        }
    } else {
        // 闭合开始标签
        write(">");
        
        // 处理非 style 子节点
        if (!nonStyleChildren.empty() || tagName == "head") {
            if (config_.prettyPrint) {
                write("\n");
            }
            
            increaseIndent();
            
            // 如果是 <head> 元素，注入全局样式
            if (tagName == "head" && !globalStylesInjected_) {
                injectGlobalStyles();
                globalStylesInjected_ = true;
            }
            
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

void Generator::injectGlobalStyles() {
    // 获取全局样式收集器
    auto& collector = GlobalStyleCollector::getInstance();
    
    // 如果没有全局样式，直接返回
    if (!collector.hasRules()) {
        return;
    }
    
    // 生成 <style> 标签
    if (config_.prettyPrint) {
        writeIndent();
    }
    write("<style>");
    
    if (config_.prettyPrint) {
        write("\n");
        increaseIndent();
    }
    
    // 生成 CSS 内容
    std::string css = collector.generateCSS(config_.prettyPrint, indentLevel_);
    write(css);
    
    if (config_.prettyPrint) {
        decreaseIndent();
        writeIndent();
    }
    
    write("</style>");
    
    if (config_.prettyPrint) {
        write("\n");
    }
}

// 查找 <html> 元素
ElementNode* Generator::findHtmlElement(const std::vector<NodePtr>& ast) {
    for (const auto& node : ast) {
        if (node->getType() == NodeType::ELEMENT) {
            ElementNode* elem = dynamic_cast<ElementNode*>(node.get());
            if (elem && elem->getTagName() == "html") {
                return elem;
            }
        }
    }
    return nullptr;
}

// 在 <html> 中查找或创建 <head> 元素
ElementNode* Generator::findOrCreateHeadElement(ElementNode* htmlElement) {
    if (!htmlElement) return nullptr;
    
    // 查找现有的 <head>
    for (const auto& child : htmlElement->getChildren()) {
        if (child->getType() == NodeType::ELEMENT) {
            ElementNode* elem = dynamic_cast<ElementNode*>(child.get());
            if (elem && elem->getTagName() == "head") {
                return elem;
            }
        }
    }
    
    // 如果没有 <head>，创建一个（但实际上在 ensureGlobalStylesInjected 中处理）
    return nullptr;
}

// 确保全局样式被注入（必要时创建 <head>）
void Generator::ensureGlobalStylesInjected(std::vector<NodePtr>& ast) {
    // 查找 <html> 元素
    ElementNode* htmlElement = findHtmlElement(ast);
    
    if (!htmlElement) {
        // 如果没有 <html> 元素，无法注入全局样式
        // 这种情况下，样式将不会被注入（但仍会为元素添加类名/id）
        return;
    }
    
    // 查找 <head> 元素
    bool hasHead = false;
    for (const auto& child : htmlElement->getChildren()) {
        if (child->getType() == NodeType::ELEMENT) {
            ElementNode* elem = dynamic_cast<ElementNode*>(child.get());
            if (elem && elem->getTagName() == "head") {
                hasHead = true;
                break;
            }
        }
    }
    
    // 如果没有 <head>，创建一个并插入到开头
    if (!hasHead) {
        auto headElement = std::make_unique<ElementNode>("head");
        htmlElement->prependChild(std::move(headElement));
    }
}

} // namespace CHTL
