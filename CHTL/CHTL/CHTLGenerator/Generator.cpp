#include "Generator.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Expression.h"
#include "../CHTLNode/Style.h"
#include "../CHTLEvaluator/Evaluator.h"
#include <memory>

namespace CHTL {

    std::string Generator::Generate(const Program* program) {
        global_styles.clear();
        CollectStyleRules(program);

        Evaluator evaluator;
        std::string body_content;
        for (const auto& stmt : program->statements) {
            body_content += GenerateStatement(stmt.get(), evaluator);
        }

        std::string global_style_content = RenderGlobalStyles();

        // A basic HTML structure. A more robust solution would be to build this
        // from the AST, but this is a simple way to inject the head and style.
        return "<html><head><style>" + global_style_content + "</style></head><body>" + body_content + "</body></html>";
    }

    void Generator::CollectStyleRules(const Node* node) {
        if (const auto elementStmt = dynamic_cast<const ElementStatement*>(node)) {
            for (const auto& child_stmt : elementStmt->Body->statements) {
                if (const auto styleStmt = dynamic_cast<const StyleStatement*>(child_stmt.get())) {
                    for (const auto& rule : styleStmt->Rules) {
                        global_styles.push_back(rule.get());
                    }
                }
                CollectStyleRules(child_stmt.get());
            }
        }
    }

    std::string Generator::RenderGlobalStyles() {
        std::string out;
        for (const auto& rule : global_styles) {
            out += rule->Selector->ToString() + " {";
            for (const auto& prop : rule->Properties) {
                out += " " + prop->Key->ToString() + ": " + prop->Value->ToString() + ";";
            }
            out += " } ";
        }
        return out;
    }


    std::string Generator::GenerateStatement(const Statement* stmt, Evaluator& evaluator) {
        if (const auto elementStmt = dynamic_cast<const ElementStatement*>(stmt)) {
            return GenerateElementStatement(elementStmt, evaluator);
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

    std::string Generator::GenerateElementStatement(const ElementStatement* stmt, Evaluator& evaluator) {
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
                    if (rule->token.type == TokenType::DOT && !class_present) {
                        if (!injected_classes.empty()) {
                            injected_classes += " ";
                        }
                        injected_classes += rule->Selector->ToString();
                    } else if (rule->token.type == TokenType::HASH && !id_present) {
                        out += " id=\"" + rule->Selector->ToString() + "\"";
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
                    auto evaluated = evaluator.Eval(prop->Value.get());
                    if (evaluated) {
                        style_string += " " + prop->Key->value + ": " + std::to_string(evaluated->Value) + evaluated->Unit + ";";
                    }
                }
            }
        }

        if (!style_string.empty()) {
            out += " style=\"" + style_string + "\"";
        }

        out += ">";
        out += GenerateBlockStatement(stmt->Body.get(), evaluator);
        out += "</" + stmt->Tag->value + ">";
        return out;
    }

    std::string Generator::GenerateBlockStatement(const BlockStatement* stmt, Evaluator& evaluator) {
        std::string out;
        for (const auto& innerStmt : stmt->statements) {
            out += GenerateStatement(innerStmt.get(), evaluator);
        }
        return out;
    }

} // namespace CHTL
