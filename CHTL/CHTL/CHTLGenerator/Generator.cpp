#include "Generator.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Expression.h"
#include "../CHTLNode/Style.h"
#include "../CHTLEvaluator/Evaluator.h"
#include "../CHTLAnalyzer/DocumentMap.h"
#include <memory>

namespace CHTL {

    std::string Generator::Generate(const Program* program) {
        global_styles.clear();
        CollectStyleRules(program, nullptr);

        DocumentMapBuilder docBuilder;
        ElementMap docMap = docBuilder.Build(program);

        Evaluator evaluator;
        std::map<std::string, std::unique_ptr<Object>> context;
        std::string body_content;
        for (const auto& stmt : program->statements) {
            body_content += GenerateStatement(stmt.get(), evaluator, context, docMap);
        }

        std::string global_style_content = RenderGlobalStyles(evaluator, context, docMap);

        // A basic HTML structure. A more robust solution would be to build this
        // from the AST, but this is a simple way to inject the head and style.
        return "<html><head><style>" + global_style_content + "</style></head><body>" + body_content + "</body></html>";
    }

    void Generator::CollectStyleRules(const Node* node, const ElementStatement* parent) {
        if (const auto elementStmt = dynamic_cast<const ElementStatement*>(node)) {
            for (const auto& child_stmt : elementStmt->Body->statements) {
                if (const auto styleStmt = dynamic_cast<const StyleStatement*>(child_stmt.get())) {
                    for (const auto& rule : styleStmt->Rules) {
                        global_styles.push_back({rule.get(), elementStmt});
                    }
                }
                CollectStyleRules(child_stmt.get(), elementStmt);
            }
        }
    }

    std::string Generator::RenderGlobalStyles(Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap) {
        std::string out;
        for (const auto& pair : global_styles) {
            const auto rule = pair.first;
            const auto parent = pair.second;
            std::string selector = rule->Selector->ToString();

            if (selector.find('&') != std::string::npos) {
                std::string parent_selector;
                if (parent) {
                    // Find the primary selector from the parent's style rules
                    bool found = false;
                    for (const auto& child_stmt : parent->Body->statements) {
                        if (const auto styleStmt = dynamic_cast<const StyleStatement*>(child_stmt.get())) {
                            for (const auto& r : styleStmt->Rules) {
                                if (r->token.type == TokenType::DOT || r->token.type == TokenType::HASH) {
                                    parent_selector = r->Selector->ToString();
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if (found) break;
                    }
                }
                selector.replace(selector.find('&'), 1, parent_selector);
            }

            out += selector + " {";
            for (const auto& prop : rule->Properties) {
                auto evaluated = evaluator.Eval(prop->Value.get(), context, docMap);
                if (evaluated) {
                    std::string value_str = std::to_string(evaluated->Value);
                    value_str.erase(value_str.find_last_not_of('0') + 1, std::string::npos);
                    if (value_str.back() == '.') value_str.pop_back();
                    out += " " + prop->Key->ToString() + ": " + value_str + evaluated->Unit + ";";
                }
            }
            out += " } ";
        }
        return out;
    }

    std::string Generator::GenerateStatement(const Statement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap) {
        if (const auto elementStmt = dynamic_cast<const ElementStatement*>(stmt)) {
            return GenerateElementStatement(elementStmt, evaluator, context, docMap);
        }
        if (const auto textStmt = dynamic_cast<const TextStatement*>(stmt)) {
            return GenerateTextStatement(textStmt);
        }
        // StyleStatements are handled in the two-pass process and don't produce direct output here
        return "";
    }

    std::string Generator::GenerateTextStatement(const TextStatement* stmt) {
        if (const auto stringLiteral = dynamic_cast<const StringLiteral*>(stmt->Text.get())) {
            return stringLiteral->value;
        }
        if (const auto unquotedLiteral = dynamic_cast<const UnquotedLiteral*>(stmt->Text.get())) {
            return unquotedLiteral->value;
        }
        return "";
    }

    std::string Generator::GenerateElementStatement(const ElementStatement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap) {
        std::string out;
        out += "<" + stmt->Tag->value;

        // Add explicit attributes
        for (const auto& attr : stmt->Attributes) {
            out += " " + attr->Key->value + "=\"";
            if (const auto stringLiteral = dynamic_cast<const StringLiteral*>(attr->Value.get())) {
                out += stringLiteral->value;
            } else if (const auto identifier = dynamic_cast<const Identifier*>(attr->Value.get())) {
                out += identifier->value;
            }
            out += "\"";
        }

        // Auto-inject class/id from style rules if not present
        bool class_present = false;
        bool id_present = false;
        for (const auto& attr : stmt->Attributes) {
            if (attr->Key->value == "class") class_present = true;
            if (attr->Key->value == "id") id_present = true;
        }

        std::string injected_classes;
        for (const auto& child_stmt : stmt->Body->statements) {
            if (const auto styleStmt = dynamic_cast<const StyleStatement*>(child_stmt.get())) {
                for (const auto& rule : styleStmt->Rules) {
                    std::string selector_text = rule->Selector->ToString();
                    if (rule->token.type == TokenType::DOT && !class_present) {
                        if (!injected_classes.empty()) {
                            injected_classes += " ";
                        }
                        injected_classes += selector_text.substr(1); // remove leading '.'
                    } else if (rule->token.type == TokenType::HASH && !id_present) {
                        out += " id=\"" + selector_text.substr(1) + "\""; // remove leading '#'
                        id_present = true;
                    }
                }
            }
        }
        if (!injected_classes.empty()) {
            out += " class=\"" + injected_classes + "\"";
        }


        std::string style_string;
        for (const auto& child_stmt : stmt->Body->statements) {
            if (const auto styleStmt = dynamic_cast<const StyleStatement*>(child_stmt.get())) {
                 for (const auto& prop : styleStmt->Properties) {
                    auto evaluated = evaluator.Eval(prop->Value.get(), context, docMap);
                    if (evaluated) {
                        context[prop->Key->value] = std::make_unique<Object>(*evaluated);
                         if (evaluated->Type == ObjectType::STRING) {
                            style_string += " " + prop->Key->value + ": " + evaluated->StringValue + ";";
                        } else {
                            std::string value_str = std::to_string(evaluated->Value);
                            value_str.erase(value_str.find_last_not_of('0') + 1, std::string::npos);
                            if (value_str.back() == '.') value_str.pop_back();
                            style_string += " " + prop->Key->value + ": " + value_str + evaluated->Unit + ";";
                        }
                    }
                }
            }
        }

        if (!style_string.empty()) {
            out += " style=\"" + style_string + "\"";
        }

        out += ">";
        out += GenerateBlockStatement(stmt->Body.get(), evaluator, context, docMap);
        out += "</" + stmt->Tag->value + ">";
        return out;
    }

    std::string Generator::GenerateBlockStatement(const BlockStatement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap) {
        std::string out;
        for (const auto& innerStmt : stmt->statements) {
            out += GenerateStatement(innerStmt.get(), evaluator, context, docMap);
        }
        return out;
    }

} // namespace CHTL
