#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>

namespace CHTL {

/**
 * 全局样式收集器
 * 
 * 单例模式，用于收集所有需要添加到全局 <style> 标签的 CSS 规则
 * 支持类选择器、id选择器、伪类、伪元素等
 */
class GlobalStyleCollector {
public:
    // 获取单例实例
    static GlobalStyleCollector& getInstance();
    
    // 禁止拷贝和赋值
    GlobalStyleCollector(const GlobalStyleCollector&) = delete;
    GlobalStyleCollector& operator=(const GlobalStyleCollector&) = delete;
    
    /**
     * 添加 CSS 规则
     * @param selector CSS 选择器（如 ".box", "#header", ".box:hover"）
     * @param properties CSS 属性映射（属性名 -> 属性值）
     */
    void addRule(const std::string& selector, 
                 const std::map<std::string, std::string>& properties);
    
    /**
     * 添加单个属性到选择器
     * @param selector CSS 选择器
     * @param property 属性名
     * @param value 属性值
     */
    void addProperty(const std::string& selector, 
                     const std::string& property, 
                     const std::string& value);
    
    /**
     * 生成格式化的 CSS 代码
     * @param prettyPrint 是否美化输出
     * @param indent 缩进级别
     * @return CSS 代码字符串
     */
    std::string generateCSS(bool prettyPrint = true, int indent = 0) const;
    
    /**
     * 检查是否有任何规则
     * @return 如果有规则返回 true
     */
    bool hasRules() const;
    
    /**
     * 获取规则数量
     * @return 规则数量
     */
    size_t getRuleCount() const;
    
    /**
     * 清空所有规则（每次编译前调用）
     */
    void clear();
    
    /**
     * 合并重复选择器的属性
     * 相同选择器的属性会被合并，冲突时后者覆盖前者
     */
    void merge();
    
    /**
     * 按字母顺序排序选择器
     */
    void sort();
    
    /**
     * 获取所有选择器列表
     * @return 选择器名称列表
     */
    std::vector<std::string> getSelectors() const;
    
    /**
     * 获取指定选择器的属性
     * @param selector 选择器名称
     * @return 属性映射
     */
    std::map<std::string, std::string> getProperties(const std::string& selector) const;

private:
    // 私有构造函数（单例模式）
    GlobalStyleCollector() = default;
    ~GlobalStyleCollector() = default;
    
    // 存储 CSS 规则
    // key: 选择器（如 ".box", "#header", ".box:hover"）
    // value: 属性映射（属性名 -> 属性值）
    std::map<std::string, std::map<std::string, std::string>> rules_;
    
    // 辅助函数：生成缩进字符串
    std::string makeIndent(int level) const;
};

} // namespace CHTL
