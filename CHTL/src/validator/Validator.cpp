#include "validator/Validator.h"
#include "nodes/ElementNode.h"
#include "nodes/NamespaceNode.h"
#include "nodes/ExceptNode.h"

namespace CHTL
{
    void Validator::Validate(const ProgramNode* program)
    {
        m_errors.clear();
        m_scope_stack.clear();
        validateNode(program);
    }

    void Validator::validateNode(const AstNode* node)
    {
        if (!node) return;

        if (isViolating(node))
        {
            // Simple error message for now
            m_errors.push_back("Constraint violation detected.");
        }

        // Descend into children and manage scopes
        if (auto* program = dynamic_cast<const ProgramNode*>(node)) {
            for (const auto& child : program->children) {
                validateNode(child.get());
            }
        } else if (auto* element = dynamic_cast<const ElementNode*>(node)) {
            enterScope(element);
            for (const auto& child : element->children) {
                validateNode(child.get());
            }
            exitScope();
        } else if (auto* ns = dynamic_cast<const NamespaceNode*>(node)) {
            enterScope(ns);
            for (const auto& child : ns->children) {
                validateNode(child.get());
            }
            exitScope();
        }
        // Other node types with children that can contain constrained nodes
        else if (auto* tmpl = dynamic_cast<const TemplateDefinitionNode*>(node)) {
             for (const auto& child : tmpl->body) {
                validateNode(child.get());
            }
        }
    }

    void Validator::enterScope(const AstNode* node)
    {
        Scope new_scope;
        if (auto* element = dynamic_cast<const ElementNode*>(node)) {
            for (const auto& child : element->children) {
                if (auto* except_node = dynamic_cast<const ExceptNode*>(child.get())) {
                    new_scope.constraints.insert(new_scope.constraints.end(), except_node->constraints.begin(), except_node->constraints.end());
                }
            }
        } else if (auto* ns = dynamic_cast<const NamespaceNode*>(node)) {
            for (const auto& child : ns->children) {
                if (auto* except_node = dynamic_cast<const ExceptNode*>(child.get())) {
                    new_scope.constraints.insert(new_scope.constraints.end(), except_node->constraints.begin(), except_node->constraints.end());
                }
            }
        }
        m_scope_stack.push_back(new_scope);
    }

    void Validator::exitScope()
    {
        if (!m_scope_stack.empty())
        {
            m_scope_stack.pop_back();
        }
    }

    bool Validator::isViolating(const AstNode* node)
    {
        for (const auto& scope : m_scope_stack)
        {
            for (const auto& constraint : scope.constraints)
            {
                if (constraint.is_type_constraint)
                {
                    // Handle type constraints like @Html
                    if (auto* element = dynamic_cast<const ElementNode*>(node))
                    {
                        if (constraint.path[0] == "@Html") // Simple check for now
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    // Handle precise constraints like 'span' or '[Custom] @Element Box'
                    if (auto* element = dynamic_cast<const ElementNode*>(node))
                    {
                        if (constraint.path.size() == 1 && constraint.path[0] == element->tag_name)
                        {
                            return true;
                        }
                    }
                    else if (auto* usage = dynamic_cast<const TemplateUsageNode*>(node))
                    {
                        if (constraint.path.size() == 3 &&
                            constraint.path[0] == "[Custom]" &&
                            constraint.path[1] == usage->type &&
                            constraint.path[2] == usage->name)
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
}
