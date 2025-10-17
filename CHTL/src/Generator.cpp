#include "Generator.h"
#include "Evaluator.h"
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <map>

namespace CHTL
{
    extern const std::string GLOBAL_NAMESPACE;

    std::string Generator::Generate(ProgramNode* program)
    {
        m_programNode = program;
        m_output.str("");
        m_output.clear();
        m_global_styles.str("");
        m_global_styles.clear();

        EvalContext context;
        context.program = program;
        context.current_namespace = GLOBAL_NAMESPACE;

        visit(program, context);

        std::string content = m_output.str();
        std::string final_styles = m_global_styles.str();
        if (!final_styles.empty())
        {
            std::stringstream head_ss;
            head_ss << "<head><style>" << final_styles << "</style></head>";
            return head_ss.str() + content;
        }

        return content;
    }

    void Generator::visit(AstNode* node, EvalContext& context)
    {
        if (!node) return;

        switch (node->GetType())
        {
            case NodeType::Program:
                visit(static_cast<ProgramNode*>(node), context);
                break;
            case NodeType::Element:
                visit(static_cast<ElementNode*>(node), context);
                break;
            case NodeType::Text:
                visit(static_cast<TextNode*>(node), context);
                break;
            case NodeType::Namespace:
                visit(static_cast<NamespaceNode*>(node), context);
                break;
            case NodeType::Comment:
                visit(static_cast<CommentNode*>(node), context);
                break;
            case NodeType::Origin:
                visit(static_cast<OriginNode*>(node), context);
                break;
            case NodeType::Import:
                 visit(static_cast<ImportNode*>(node), context);
                 break;
            case NodeType::TemplateUsage:
                visit(static_cast<TemplateUsageNode*>(node), context);
                break;
            case NodeType::TemplateDefinition:
            case NodeType::CustomDefinition:
            case NodeType::Style:
            case NodeType::CustomUsage:
            case NodeType::StyleRule:
            case NodeType::StyleProperty:
                // These nodes are handled contextually by their parents.
                break;
            default:
                throw std::runtime_error("Unknown AST node type in Generator");
        }
    }

    void Generator::visit(ProgramNode* node, EvalContext& context)
    {
        for (const auto& child : node->children)
        {
            visit(child.get(), context);
        }
    }

    void Generator::visit(NamespaceNode* node, EvalContext& context)
    {
        std::string previous_namespace = context.current_namespace;
        context.current_namespace = node->name;
        for (const auto& child : node->children)
        {
            visit(child.get(), context);
        }
        context.current_namespace = previous_namespace;
    }

    void Generator::visit(ElementNode* node, EvalContext& context)
    {
        // Process style node first to populate attributes (like class, id, style)
        for (const auto& child : node->children)
        {
            if (child->GetType() == NodeType::Style)
            {
                visit(static_cast<StyleNode*>(child.get()), context, node);
            }
        }

        m_output << "<" << node->tag_name;
        for (const auto& attr : node->attributes)
        {
            m_output << " " << attr.name << "=\"" << attr.value << "\"";
        }
        m_output << ">";

        // Process non-style children
        for (const auto& child : node->children)
        {
            if (child->GetType() != NodeType::Style)
            {
                visit(child.get(), context);
            }
        }

        m_output << "</" << node->tag_name << ">";
    }

    // Helper function to evaluate a style property and store it in a map, handling overrides.
    static void evaluateAndStoreProperty(StyleProperty* prop, EvalContext& context, std::map<std::string, std::string>& property_map) {
        Evaluator evaluator;
        Value result = evaluator.Eval(prop->value.get(), context);
        context.values[prop->name] = result; // Update context for subsequent properties
        std::stringstream ss;
        if (result.type == ValueType::STRING) {
            ss << result.str;
        } else {
            ss << result.num << result.unit;
        }
        property_map[prop->name] = ss.str();
    }

    void Generator::visit(StyleNode* node, EvalContext& context, ElementNode* parent)
    {
        std::map<std::string, std::string> inline_properties;
        EvalContext local_context = context;

        // First pass to find the main selector for '&' replacement
        std::string main_selector;
        for (const auto& child : node->children) {
            if (child->GetType() == NodeType::StyleRule) {
                auto* rule = static_cast<StyleRuleNode*>(child.get());
                if (main_selector.empty() && (rule->selector[0] == '.' || rule->selector[0] == '#')) {
                    main_selector = rule->selector;
                }
            }
        }

        for (const auto& child : node->children)
        {
            if (child->GetType() == NodeType::StyleProperty)
            {
                evaluateAndStoreProperty(static_cast<StyleProperty*>(child.get()), local_context, inline_properties);
            }
            else if (child->GetType() == NodeType::StyleRule)
            {
                auto* rule = static_cast<StyleRuleNode*>(child.get());
                if (rule->selector[0] == '&' && !main_selector.empty()) {
                    rule->selector.replace(0, 1, main_selector);
                }
                visit(rule, local_context, parent);
            }
            else if (child->GetType() == NodeType::TemplateUsage)
            {
                auto* usage = static_cast<TemplateUsageNode*>(child.get());
                const TemplateDefinitionNode* tmpl = nullptr;
                if (m_programNode->templates.count(local_context.current_namespace) && m_programNode->templates.at(local_context.current_namespace).count(usage->name)) {
                    tmpl = m_programNode->templates.at(local_context.current_namespace).at(usage->name);
                }
                else if (local_context.current_namespace != GLOBAL_NAMESPACE && m_programNode->templates.count(GLOBAL_NAMESPACE) && m_programNode->templates.at(GLOBAL_NAMESPACE).count(usage->name)) {
                    tmpl = m_programNode->templates.at(GLOBAL_NAMESPACE).at(usage->name);
                }

                if (tmpl) {
                    for (const auto& prop_ptr : tmpl->properties) {
                        evaluateAndStoreProperty(prop_ptr.get(), local_context, inline_properties);
                    }
                }
            }
            else if (child->GetType() == NodeType::CustomUsage)
            {
                 auto* usage = static_cast<CustomUsageNode*>(child.get());
                 const CustomDefinitionNode* custom_def = nullptr;
                 if (m_programNode->customs.count(local_context.current_namespace) && m_programNode->customs.at(local_context.current_namespace).count(usage->name)) {
                     custom_def = m_programNode->customs.at(local_context.current_namespace).at(usage->name);
                 }
                 else if (local_context.current_namespace != GLOBAL_NAMESPACE && m_programNode->customs.count(GLOBAL_NAMESPACE) && m_programNode->customs.at(GLOBAL_NAMESPACE).count(usage->name)) {
                     custom_def = m_programNode->customs.at(GLOBAL_NAMESPACE).at(usage->name);
                 }

                if (custom_def) {
                    std::unordered_set<std::string> deleted_properties;
                    for (const auto& spec : usage->specializations) {
                        if (spec->GetType() == NodeType::DeleteSpecialization) {
                            deleted_properties.insert(static_cast<DeleteSpecializationNode*>(spec.get())->property_name);
                        }
                    }

                    // Process base properties
                    for (const auto& child_node : custom_def->children) {
                        if (child_node->GetType() == NodeType::StyleProperty) {
                            auto* prop_ptr = static_cast<StyleProperty*>(child_node.get());
                            if (deleted_properties.find(prop_ptr->name) == deleted_properties.end()) {
                                evaluateAndStoreProperty(prop_ptr, local_context, inline_properties);
                            }
                        }
                    }
                    // Process overrides/additions from the usage block
                    for (const auto& spec : usage->specializations) {
                        if (spec->GetType() == NodeType::StyleProperty) {
                            evaluateAndStoreProperty(static_cast<StyleProperty*>(spec.get()), local_context, inline_properties);
                        }
                    }
                }
            }
        }

        std::stringstream inline_styles_stream;
        for (const auto& pair : inline_properties) {
            inline_styles_stream << pair.first << ":" << pair.second << ";";
        }

        std::string inline_style_str = inline_styles_stream.str();
        if (!inline_style_str.empty())
        {
            bool style_attr_exists = false;
            for (auto& attr : parent->attributes) {
                if (attr.name == "style") {
                    attr.value += inline_style_str;
                    style_attr_exists = true;
                    break;
                }
            }
            if (!style_attr_exists) {
                parent->attributes.push_back({"style", inline_style_str});
            }
        }
    }

    void Generator::visit(StyleRuleNode* node, EvalContext& context, ElementNode* parent)
    {
        std::string selector = node->selector;
        // Handle automatic class/id addition based on selector
        if (parent) {
            if (selector.rfind(".", 0) == 0) { // starts with .
                 bool class_exists = false;
                 std::string class_name = selector.substr(1);
                 for(const auto& attr : parent->attributes) {
                     if (attr.name == "class") {
                         class_exists = true;
                         // TODO: Append to existing class list if needed
                         break;
                     }
                 }
                 if (!class_exists) {
                     parent->attributes.push_back({"class", class_name});
                 }
            } else if (selector.rfind("#", 0) == 0) { // starts with #
                 bool id_exists = false;
                 std::string id_name = selector.substr(1);
                 for(const auto& attr : parent->attributes) {
                     if (attr.name == "id") {
                         id_exists = true;
                         break;
                     }
                 }
                 if (!id_exists) {
                     parent->attributes.push_back({"id", id_name});
                 }
            }
        }

        m_global_styles << selector << "{";
        std::stringstream rule_content_stream;
        for (const auto& prop : node->properties)
        {
            visit(prop.get(), context, rule_content_stream);
        }
        m_global_styles << rule_content_stream.str() << "}";
    }

    void Generator::visit(StyleProperty* node, EvalContext& context, std::stringstream& style_stream)
    {
        Evaluator evaluator;
        Value result = evaluator.Eval(node->value.get(), context);
        context.values[node->name] = result; // Update context for subsequent properties

        style_stream << node->name << ":";
        if (result.type == ValueType::STRING)
        {
            style_stream << result.str;
        }
        else
        {
            style_stream << result.num << result.unit;
        }
        style_stream << ";";
    }

    void Generator::visit(TextNode* node, EvalContext& context)
    {
        m_output << node->value;
    }

    void Generator::visit(CommentNode* node, EvalContext& context)
    {
        m_output << "<!-- " << node->value << " -->";
    }

    void Generator::visit(OriginNode* node, EvalContext& context)
    {
        m_output << node->content;
    }

    void Generator::visit(TemplateUsageNode* node, EvalContext& context)
    {
        if (node->type == "@Element")
        {
            const TemplateDefinitionNode* tmpl = nullptr;
            if (m_programNode->templates.count(context.current_namespace) && m_programNode->templates.at(context.current_namespace).count(node->name))
            {
                tmpl = m_programNode->templates.at(context.current_namespace).at(node->name);
            }
            else if (context.current_namespace != GLOBAL_NAMESPACE && m_programNode->templates.count(GLOBAL_NAMESPACE) && m_programNode->templates.at(GLOBAL_NAMESPACE).count(node->name))
            {
                tmpl = m_programNode->templates.at(GLOBAL_NAMESPACE).at(node->name);
            }

            if (tmpl)
            {
                for (const auto& child : tmpl->body)
                {
                    visit(child.get(), context);
                }
            }
        }
    }

    void Generator::visit(CustomDefinitionNode* node, EvalContext& context)
    {
        // Intentionally empty. Custom definitions are not directly generated, but used.
    }

    void Generator::visit(ImportNode* node, EvalContext& context)
    {
        // Intentionally empty. The import logic is handled by the parser.
        // The generator's job is just to not crash on this node.
    }
}
