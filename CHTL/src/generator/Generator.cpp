#include "generator/Generator.h"
#include "eval/Evaluator.h"
#include "nodes/IfNode.h"
#include "nodes/ElseNode.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
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
            case NodeType::Script:
                visit(static_cast<ScriptNode*>(node), context);
                break;
            case NodeType::Import:
                 visit(static_cast<ImportNode*>(node), context);
                 break;
            case NodeType::TemplateUsage:
                visit(static_cast<TemplateUsageNode*>(node), context);
                break;
            case NodeType::CustomUsage:
                visit(static_cast<CustomUsageNode*>(node), context);
                break;
            case NodeType::TemplateDefinition:
            case NodeType::CustomDefinition:
            case NodeType::Style:
            case NodeType::StyleRule:
            case NodeType::StyleProperty:
                // These nodes are handled contextually by their parents.
                break;
            case NodeType::If:
                visit(static_cast<IfNode*>(node), context);
                break;
            case NodeType::Else:
                visit(static_cast<ElseNode*>(node), context);
                break;
            default:
                throw std::runtime_error("Unknown AST node type in Generator");
        }
    }

    void Generator::visit(IfNode* node, EvalContext& context)
    {
        // This visitor is now only for content generation.
        // Style application is handled in the ElementNode visitor's first pass.
        Evaluator evaluator;
        Value result = evaluator.Eval(node->condition.get(), context);
        if (result.IsTruthy())
        {
            for (const auto& child : node->consequence)
            {
                // We only visit content-generating nodes here.
                if (child->GetType() != NodeType::StyleProperty) {
                    visit(child.get(), context);
                }
            }
        }
        else if (node->alternative)
        {
            visit(node->alternative.get(), context);
        }
    }

    void Generator::visit(ElseNode* node, EvalContext& context)
    {
        for (const auto& child : node->consequence)
        {
            visit(child.get(), context);
        }
    }

    // Helper to find the index of a target node based on a simple selector like "tag[index]"

    // Helper function to append styles to an element's style attribute
    void appendStyles(ElementNode* element, const std::string& new_styles)
    {
        if (new_styles.empty()) {
            return;
        }

        bool style_attr_exists = false;
        for (auto& attr : element->attributes) {
            if (attr.name == "style") {
                // Ensure there's a semicolon before adding new styles if the attribute is not empty
                if (!attr.value.empty() && attr.value.back() != ';') {
                    attr.value += ';';
                }
                attr.value += new_styles;
                style_attr_exists = true;
                break;
            }
        }
        if (!style_attr_exists) {
            element->attributes.push_back({"style", new_styles});
        }
    }

    static int findTargetIndex(const std::vector<AstNode*>& nodes, const std::string& selector)
    {
        std::string tag;
        int target_occurrence = 0; // 0-based index of the desired occurrence

        size_t bracket_pos = selector.find('[');
        if (bracket_pos != std::string::npos) {
            tag = selector.substr(0, bracket_pos);
            size_t end_bracket_pos = selector.find(']');
            if (end_bracket_pos != std::string::npos) {
                std::string index_str = selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
                try {
                    target_occurrence = std::stoi(index_str);
                } catch(...) {
                    target_occurrence = 0;
                }
            }
        } else {
            tag = selector;
        }

        int current_occurrence = 0;
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i] && nodes[i]->GetType() == NodeType::Element) {
                auto* el = static_cast<ElementNode*>(nodes[i]);
                if (el->tag_name == tag) {
                    if (current_occurrence == target_occurrence) {
                        return i;
                    }
                    current_occurrence++;
                }
            }
        }
        return -1; // Not found
    }

    void Generator::visit(CustomUsageNode* node, EvalContext& context)
    {
        if (node->type != "@Element") {
            // @Style and @Var usages are handled within visit(StyleNode*).
            return;
        }

        const CustomDefinitionNode* def = nullptr;
        if (m_programNode->customs.count(context.current_namespace) && m_programNode->customs.at(context.current_namespace).count(node->name))
        {
            def = m_programNode->customs.at(context.current_namespace).at(node->name);
        }
        else if (context.current_namespace != GLOBAL_NAMESPACE && m_programNode->customs.count(GLOBAL_NAMESPACE) && m_programNode->customs.at(GLOBAL_NAMESPACE).count(node->name))
        {
            def = m_programNode->customs.at(GLOBAL_NAMESPACE).at(node->name);
        }

        if (!def) { return; }

        // Use a vector of raw pointers. This is less safe but avoids incomplete clone logic for now.
        // The pointed-to nodes are owned by the ProgramNode and will outlive the generator.
        std::vector<AstNode*> processed_nodes;
        for(const auto& child : def->children) {
            processed_nodes.push_back(child.get());
        }

        // Apply specializations
        for (const auto& spec : node->specializations)
        {
            if (spec->GetType() == NodeType::InsertSpecialization)
            {
                auto* insert_spec = static_cast<InsertSpecializationNode*>(spec.get());
                std::vector<AstNode*> nodes_to_insert;
                for(const auto& content_node : insert_spec->content) {
                    nodes_to_insert.push_back(content_node.get());
                }

                if (insert_spec->position == "at top") {
                    processed_nodes.insert(processed_nodes.begin(), nodes_to_insert.begin(), nodes_to_insert.end());
                } else if (insert_spec->position == "at bottom") {
                    processed_nodes.insert(processed_nodes.end(), nodes_to_insert.begin(), nodes_to_insert.end());
                } else {
                    int target_idx = findTargetIndex(processed_nodes, insert_spec->target_selector);
                    if (target_idx != -1) {
                        if (insert_spec->position == "after") {
                            processed_nodes.insert(processed_nodes.begin() + target_idx + 1, nodes_to_insert.begin(), nodes_to_insert.end());
                        } else if (insert_spec->position == "before") {
                             processed_nodes.insert(processed_nodes.begin() + target_idx, nodes_to_insert.begin(), nodes_to_insert.end());
                        } else if (insert_spec->position == "replace") {
                            processed_nodes.erase(processed_nodes.begin() + target_idx);
                            processed_nodes.insert(processed_nodes.begin() + target_idx, nodes_to_insert.begin(), nodes_to_insert.end());
                        }
                    }
                }
            }
            else if (spec->GetType() == NodeType::DeleteSpecialization)
            {
                auto* delete_spec = static_cast<DeleteSpecializationNode*>(spec.get());
                int target_idx = findTargetIndex(processed_nodes, delete_spec->property_name); // Re-using property_name for selector
                if (target_idx != -1) {
                    processed_nodes.erase(processed_nodes.begin() + target_idx);
                }
            }
        }

        // Generate the final list of nodes
        for (const auto& child : processed_nodes)
        {
            visit(child, context);
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

    // Helper to check if a node is forbidden by any of the except constraints
    static bool isNodeForbidden(const AstNode* node, const std::vector<const ExceptNode*>& except_nodes)
    {
        if (!node) return false;

        for (const auto* except_node : except_nodes) {
            for (const auto& constraint : except_node->constraints) {
                if (node->GetType() == NodeType::Element) {
                    auto* el = static_cast<const ElementNode*>(node);
                    // Simple tag name check, e.g., "except span;"
                    if (constraint.path.size() == 1 && constraint.path[0] == el->tag_name) {
                        return true;
                    }
                }
                // More complex checks for [Custom] @Element Box etc. can be added here
            }
        }
        return false;
    }

    void Generator::visit(ElementNode* node, EvalContext& context)
    {
        // First pass: process style, if, and except nodes
        std::vector<const ExceptNode*> except_nodes;
        Evaluator evaluator;
        for (const auto& child : node->children)
        {
            if (child->GetType() == NodeType::Style)
            {
                visit(static_cast<StyleNode*>(child.get()), context, node);
            }
            else if (child->GetType() == NodeType::Except)
            {
                except_nodes.push_back(static_cast<const ExceptNode*>(child.get()));
            }
            else if (child->GetType() == NodeType::If)
            {
                // Find the first 'if' or 'else if' that is true in the chain.
                AstNode* current_in_chain = child.get();
                bool condition_met = false;
                while(current_in_chain && !condition_met) {
                    if (current_in_chain->GetType() == NodeType::If) {
                        auto* if_node = static_cast<IfNode*>(current_in_chain);
                        Value condition_result = evaluator.Eval(if_node->condition.get(), context);
                        if (condition_result.IsTruthy()) {
                            // Apply styles from this block
                            std::stringstream temp_style_stream;
                            for (const auto& prop_node : if_node->consequence) {
                                if (prop_node->GetType() == NodeType::StyleProperty) {
                                    visit(static_cast<StyleProperty*>(prop_node.get()), context, temp_style_stream);
                                }
                            }
                            std::string new_styles = temp_style_stream.str();
                            appendStyles(node, new_styles);
                            condition_met = true;
                        }
                        current_in_chain = if_node->alternative.get();
                    } else if (current_in_chain->GetType() == NodeType::Else) {
                        // This is the final 'else', it has no condition.
                        // Apply its styles.
                        auto* else_node = static_cast<ElseNode*>(current_in_chain);
                        std::stringstream temp_style_stream;
                        for (const auto& prop_node : else_node->consequence) {
                           if (prop_node->GetType() == NodeType::StyleProperty) {
                                visit(static_cast<StyleProperty*>(prop_node.get()), context, temp_style_stream);
                            }
                        }
                        std::string new_styles = temp_style_stream.str();
                        appendStyles(node, new_styles);
                        condition_met = true; // Stop searching
                    } else {
                        // Not an if or else, break the chain search.
                        break;
                    }
                }
            }
        }

        m_output << "<" << node->tag_name;
        for (const auto& attr : node->attributes)
        {
            m_output << " " << attr.name << "=\"" << attr.value << "\"";
        }
        m_output << ">";

        // Second pass: process and generate content children, checking constraints
        for (const auto& child : node->children)
        {
            // Skip style and except nodes, but now PROCESS if nodes
            if (child->GetType() == NodeType::Style || child->GetType() == NodeType::Except) {
                continue;
            }

            if (isNodeForbidden(child.get(), except_nodes)) {
                continue;
            }

            visit(child.get(), context);
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

    void Generator::visit(ScriptNode* node, EvalContext& context)
    {
        m_output << "<script>";
        if (node->js_ast)
        {
            visit(node->js_ast.get(), context);
        }
        m_output << "</script>";
    }

    // CHTL JS Visitors
    void Generator::visit(CHTLJS::AstNode* node, EvalContext& context)
    {
        if (!node) return;

        switch (node->GetType())
        {
            case CHTLJS::NodeType::Program:
                visit(static_cast<CHTLJS::ProgramNode*>(node), context);
                break;
            case CHTLJS::NodeType::EnhancedSelector:
                visit(static_cast<CHTLJS::EnhancedSelectorNode*>(node), context);
                break;
            case CHTLJS::NodeType::RawJS:
                visit(static_cast<CHTLJS::RawJSNode*>(node), context);
                break;
            default:
                // For now, do nothing for unknown JS nodes
                break;
        }
    }

    void Generator::visit(CHTLJS::ProgramNode* node, EvalContext& context)
    {
        for (const auto& child : node->children)
        {
            visit(child.get(), context);
        }
    }

    void Generator::visit(CHTLJS::RawJSNode* node, EvalContext& context)
    {
        m_output << node->content;
    }

    void Generator::visit(CHTLJS::EnhancedSelectorNode* node, EvalContext& context)
    {
        // Basic translation, doesn't handle all selector types from CHTL.md yet
        m_output << "document.querySelector('" << node->selector << "')";
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
