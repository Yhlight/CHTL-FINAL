#pragma once

#include "nodes/ProgramNode.h"
#include "AstNode.h"
#include <string>
#include <sstream>
#include <memory>

namespace CHTL
{
    /**
     * @struct FormatterOptions
     * @brief CHTL代码格式化选项
     */
    struct FormatterOptions
    {
        int indentSize = 4;          // 缩进空格数
        bool useTabs = false;        // 使用Tab还是空格
        bool insertSpaces = true;    // 在操作符周围插入空格
        int maxLineLength = 120;     // 最大行长度
        bool alignProperties = true; // 对齐属性
        bool sortImports = true;     // 排序导入语句
        bool trailingComma = false;  // 数组/对象末尾逗号
    };

    /**
     * @class Formatter
     * @brief CHTL代码格式化器
     * 
     * 功能:
     * - 统一代码缩进
     * - 规范化空格和换行
     * - 对齐属性和值
     * - 美化嵌套结构
     * - 排序导入语句
     */
    class Formatter
    {
    public:
        /**
         * @brief 构造函数
         * @param options 格式化选项
         */
        explicit Formatter(const FormatterOptions& options = FormatterOptions());

        /**
         * @brief 格式化CHTL代码
         * @param input 输入的CHTL代码
         * @return 格式化后的代码
         */
        std::string Format(const std::string& input);

        /**
         * @brief 格式化AST
         * @param program AST根节点
         * @return 格式化后的代码
         */
        std::string Format(const ProgramNode& program);

        /**
         * @brief 获取格式化选项
         */
        const FormatterOptions& GetOptions() const { return m_options; }

        /**
         * @brief 设置格式化选项
         */
        void SetOptions(const FormatterOptions& options) { m_options = options; }

    private:
        void visit(const AstNode& node);
        void visitChildren(const ProgramNode& node);
        
        // 各种节点的格式化
        void formatElement(const class ElementNode& node);
        void formatText(const class TextNode& node);
        void formatStyle(const class StyleNode& node);
        void formatScript(const class ScriptNode& node);
        void formatTemplate(const class TemplateDefinitionNode& node);
        void formatCustom(const class CustomDefinitionNode& node);
        void formatImport(const class ImportNode& node);
        void formatNamespace(const class NamespaceNode& node);
        void formatComment(const class CommentNode& node);
        void formatConfiguration(const class ConfigurationNode& node);
        
        // 辅助方法
        void writeIndent();
        void writeLine(const std::string& line = "");
        void increaseIndent();
        void decreaseIndent();
        std::string trimString(const std::string& str);
        std::string normalizeSpaces(const std::string& str);
        bool shouldBreakLine() const;
        
        FormatterOptions m_options;
        std::ostringstream m_output;
        int m_currentIndent = 0;
        int m_currentLineLength = 0;
        bool m_needsIndent = true;
    };

} // namespace CHTL
