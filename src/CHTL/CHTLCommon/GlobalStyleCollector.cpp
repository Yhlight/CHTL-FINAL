#include "GlobalStyleCollector.h"
#include <algorithm>

namespace CHTL {

GlobalStyleCollector& GlobalStyleCollector::getInstance() {
    static GlobalStyleCollector instance;
    return instance;
}

void GlobalStyleCollector::addRule(const std::string& selector, 
                                   const std::map<std::string, std::string>& properties) {
    if (selector.empty() || properties.empty()) {
        return;
    }
    
    // 如果选择器已存在，合并属性
    auto& ruleProps = rules_[selector];
    for (const auto& [prop, value] : properties) {
        ruleProps[prop] = value;  // 后者覆盖前者
    }
}

void GlobalStyleCollector::addProperty(const std::string& selector, 
                                       const std::string& property, 
                                       const std::string& value) {
    if (selector.empty() || property.empty()) {
        return;
    }
    
    rules_[selector][property] = value;
}

std::string GlobalStyleCollector::generateCSS(bool prettyPrint, int indent) const {
    if (rules_.empty()) {
        return "";
    }
    
    std::ostringstream css;
    std::string indentStr = makeIndent(indent);
    std::string propIndentStr = makeIndent(indent + 1);
    
    bool first = true;
    for (const auto& [selector, properties] : rules_) {
        if (properties.empty()) {
            continue;
        }
        
        // 添加选择器之间的空行（美化模式）
        if (prettyPrint && !first) {
            css << "\n";
        }
        first = false;
        
        // 选择器
        if (prettyPrint) {
            css << indentStr;
        }
        css << selector;
        
        if (prettyPrint) {
            css << " {\n";
        } else {
            css << "{";
        }
        
        // 属性（按字母顺序排序）
        std::vector<std::pair<std::string, std::string>> sortedProps(
            properties.begin(), properties.end()
        );
        std::sort(sortedProps.begin(), sortedProps.end());
        
        for (const auto& [prop, value] : sortedProps) {
            if (prettyPrint) {
                css << propIndentStr << prop << ": " << value << ";\n";
            } else {
                css << prop << ":" << value << ";";
            }
        }
        
        // 结束大括号
        if (prettyPrint) {
            css << indentStr << "}\n";
        } else {
            css << "}";
        }
    }
    
    return css.str();
}

bool GlobalStyleCollector::hasRules() const {
    return !rules_.empty();
}

size_t GlobalStyleCollector::getRuleCount() const {
    return rules_.size();
}

void GlobalStyleCollector::clear() {
    rules_.clear();
}

void GlobalStyleCollector::merge() {
    // 规则已经在 addRule 中自动合并
    // 这个方法保留以供将来使用（如合并相似的选择器）
}

void GlobalStyleCollector::sort() {
    // std::map 已经自动按键排序
    // 这个方法保留以供将来使用
}

std::vector<std::string> GlobalStyleCollector::getSelectors() const {
    std::vector<std::string> selectors;
    selectors.reserve(rules_.size());
    
    for (const auto& [selector, _] : rules_) {
        selectors.push_back(selector);
    }
    
    return selectors;
}

std::map<std::string, std::string> GlobalStyleCollector::getProperties(
    const std::string& selector) const {
    auto it = rules_.find(selector);
    if (it != rules_.end()) {
        return it->second;
    }
    return {};
}

std::string GlobalStyleCollector::makeIndent(int level) const {
    return std::string(level * 2, ' ');  // 每级缩进 2 个空格
}

} // namespace CHTL
