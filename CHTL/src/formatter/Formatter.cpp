#include "formatter/Formatter.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "nodes/ElementNode.h"
#include "nodes/TextNode.h"
#include "nodes/StyleNode.h"
#include "nodes/StyleRuleNode.h"
#include "nodes/ScriptNode.h"
#include "nodes/TemplateDefinitionNode.h"
#include "nodes/CustomDefinitionNode.h"
#include "nodes/ImportNode.h"
#include "nodes/NamespaceNode.h"
#include "nodes/CommentNode.h"
#include "nodes/ConfigurationNode.h"
#include <algorithm>
#include <cctype>

namespace CHTL
{
    Formatter::Formatter(const FormatterOptions& options)
        : m_options(options)
    {
    }

    std::string Formatter::Format(const std::string& input)
    {
        // 解析输入代码为AST
        Lexer lexer(input);
        Parser parser(lexer);
        auto program = parser.ParseProgram();
        
        if (!program || !parser.GetErrors().empty())
        {
            // 如果解析失败，返回原始输入
            return input;
        }
        
        return Format(*program);
    }

    std::string Formatter::Format(const ProgramNode& program)
    {
        m_output.str("");
        m_output.clear();
        m_currentIndent = 0;
        m_currentLineLength = 0;
        m_needsIndent = true;
        
        visitChildren(program);
        
        return m_output.str();
    }

    void Formatter::visit(const AstNode& node)
    {
        switch (node.GetType())
        {
            case NodeType::Element:
                formatElement(static_cast<const ElementNode&>(node));
                break;
            case NodeType::Text:
                formatText(static_cast<const TextNode&>(node));
                break;
            case NodeType::Style:
                formatStyle(static_cast<const StyleNode&>(node));
                break;
            case NodeType::StyleRule:
                // 格式化样式规则
                {
                    auto& rule = static_cast<const class StyleRuleNode&>(node);
                    writeIndent();
                    m_output << rule.selector << " {";
                    writeLine();
                    increaseIndent();
                    
                    // 格式化属性
                    for (const auto& prop : rule.properties)
                    {
                        if (prop)
                        {
                            writeIndent();
                            // StyleProperty没有通过AstNode访问，这里简化处理
                            m_output << "/* property */";
                            writeLine();
                        }
                    }
                    
                    decreaseIndent();
                    writeIndent();
                    m_output << "}";
                    writeLine();
                }
                break;
            case NodeType::Script:
                formatScript(static_cast<const ScriptNode&>(node));
                break;
            case NodeType::TemplateDefinition:
                formatTemplate(static_cast<const TemplateDefinitionNode&>(node));
                break;
            case NodeType::CustomDefinition:
                formatCustom(static_cast<const CustomDefinitionNode&>(node));
                break;
            case NodeType::Import:
                formatImport(static_cast<const ImportNode&>(node));
                break;
            case NodeType::Namespace:
                formatNamespace(static_cast<const NamespaceNode&>(node));
                break;
            case NodeType::Comment:
                formatComment(static_cast<const CommentNode&>(node));
                break;
            case NodeType::Configuration:
                formatConfiguration(static_cast<const ConfigurationNode&>(node));
                break;
            default:
                // 其他节点类型
                break;
        }
    }

    void Formatter::visitChildren(const ProgramNode& node)
    {
        for (const auto& child : node.children)
        {
            if (child)
            {
                visit(*child);
            }
        }
    }

    void Formatter::formatElement(const ElementNode& node)
    {
        writeIndent();
        m_output << node.tag_name;
        
        // 格式化属性
        if (!node.attributes.empty())
        {
            m_output << " {";
            
            if (node.attributes.size() > 3 || m_options.alignProperties)
            {
                // 多属性时换行
                writeLine();
                increaseIndent();
                
                for (const auto& [key, value] : node.attributes)
                {
                    writeIndent();
                    m_output << key;
                    if (m_options.insertSpaces)
                        m_output << " = ";
                    else
                        m_output << "=";
                    m_output << value;
                    writeLine(";");
                }
                
                decreaseIndent();
                writeIndent();
            }
            else
            {
                // 少量属性时同行
                bool first = true;
                for (const auto& [key, value] : node.attributes)
                {
                    if (!first) m_output << ";";
                    m_output << " " << key;
                    if (m_options.insertSpaces)
                        m_output << " = ";
                    else
                        m_output << "=";
                    m_output << value;
                    first = false;
                }
                m_output << " ";
            }
            m_output << "}";
        }
        
        // 格式化子节点
        if (!node.children.empty())
        {
            m_output << " {";
            writeLine();
            increaseIndent();
            
            for (const auto& child : node.children)
            {
                if (child)
                {
                    visit(*child);
                }
            }
            
            decreaseIndent();
            writeIndent();
            m_output << "}";
        }
        
        writeLine();
    }

    void Formatter::formatText(const TextNode& node)
    {
        writeIndent();
        m_output << "text { " << node.value << " }";
        writeLine();
    }

    void Formatter::formatStyle(const StyleNode& node)
    {
        writeIndent();
        m_output << "style {";
        writeLine();
        increaseIndent();
        
        // 格式化样式块内的子节点（StyleRuleNode）
        for (const auto& child : node.children)
        {
            if (child)
            {
                visit(*child);
            }
        }
        
        decreaseIndent();
        writeIndent();
        m_output << "}";
        writeLine();
    }

    void Formatter::formatScript(const ScriptNode& node)
    {
        writeIndent();
        m_output << "script {";
        writeLine();
        
        // Script包含CHTL JS AST，这里简化处理
        increaseIndent();
        if (node.js_ast)
        {
            writeIndent();
            m_output << "// CHTL JS content";
            writeLine();
        }
        decreaseIndent();
        
        writeIndent();
        m_output << "}";
        writeLine();
    }

    void Formatter::formatTemplate(const TemplateDefinitionNode& node)
    {
        writeIndent();
        m_output << "[Template " << node.type << " " << node.name << "]";
        writeLine();
        increaseIndent();
        
        // 格式化模板体
        for (const auto& child : node.body)
        {
            if (child)
            {
                visit(*child);
            }
        }
        
        decreaseIndent();
    }

    void Formatter::formatCustom(const CustomDefinitionNode& node)
    {
        writeIndent();
        m_output << "[Custom " << node.type << " " << node.name << "]";
        writeLine();
        increaseIndent();
        
        for (const auto& child : node.children)
        {
            if (child)
            {
                visit(*child);
            }
        }
        
        decreaseIndent();
    }

    void Formatter::formatImport(const ImportNode& node)
    {
        writeIndent();
        m_output << "[Import ";
        
        if (!node.alias.empty())
        {
            m_output << node.alias << " = ";
        }
        
        m_output << node.path << "]";
        writeLine();
    }

    void Formatter::formatNamespace(const NamespaceNode& node)
    {
        writeIndent();
        m_output << "[Namespace " << node.name << "]";
        writeLine();
        increaseIndent();
        
        for (const auto& child : node.children)
        {
            if (child)
            {
                visit(*child);
            }
        }
        
        decreaseIndent();
    }

    void Formatter::formatComment(const CommentNode& node)
    {
        writeIndent();
        
        // 保持注释原样
        if (node.value.find('\n') != std::string::npos)
        {
            // 多行注释
            m_output << "/*" << node.value << "*/";
        }
        else
        {
            // 单行注释
            m_output << "// " << node.value;
        }
        writeLine();
    }

    void Formatter::formatConfiguration(const ConfigurationNode& node)
    {
        writeIndent();
        if (!node.name.empty())
        {
            m_output << "[Configuration " << node.name << "]";
        }
        else
        {
            m_output << "[Configuration]";
        }
        writeLine();
        increaseIndent();
        
        // 格式化配置设置
        for (const auto& [key, value] : node.settings)
        {
            writeIndent();
            m_output << key << " = " << value << ";";
            writeLine();
        }
        
        decreaseIndent();
    }

    void Formatter::writeIndent()
    {
        if (m_needsIndent)
        {
            int spaces = m_currentIndent * m_options.indentSize;
            if (m_options.useTabs)
            {
                for (int i = 0; i < m_currentIndent; ++i)
                {
                    m_output << '\t';
                }
            }
            else
            {
                for (int i = 0; i < spaces; ++i)
                {
                    m_output << ' ';
                }
            }
            m_needsIndent = false;
            m_currentLineLength = spaces;
        }
    }

    void Formatter::writeLine(const std::string& line)
    {
        m_output << line << '\n';
        m_needsIndent = true;
        m_currentLineLength = 0;
    }

    void Formatter::increaseIndent()
    {
        m_currentIndent++;
    }

    void Formatter::decreaseIndent()
    {
        if (m_currentIndent > 0)
        {
            m_currentIndent--;
        }
    }

    std::string Formatter::trimString(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \t");
        if (start == std::string::npos)
            return "";
        
        size_t end = str.find_last_not_of(" \t");
        return str.substr(start, end - start + 1);
    }

    std::string Formatter::normalizeSpaces(const std::string& str)
    {
        std::string result;
        bool lastWasSpace = false;
        
        for (char c : str)
        {
            if (std::isspace(c))
            {
                if (!lastWasSpace)
                {
                    result += ' ';
                    lastWasSpace = true;
                }
            }
            else
            {
                result += c;
                lastWasSpace = false;
            }
        }
        
        return result;
    }

    bool Formatter::shouldBreakLine() const
    {
        return m_currentLineLength >= m_options.maxLineLength;
    }

} // namespace CHTL
