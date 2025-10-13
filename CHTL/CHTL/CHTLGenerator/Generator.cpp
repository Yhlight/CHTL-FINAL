#include "Generator.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Expression.h"
#include "../CHTLNode/Style.h"
#include "../CHTLEvaluator/Evaluator.h"
#include <memory>

namespace CHTL {

    std::string Generator::Generate(const Program* program) {
        Evaluator evaluator;
        std::string out;
        for (const auto& stmt : program->statements) {
            out += GenerateStatement(stmt.get(), evaluator);
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

        for (const auto& attr : stmt->Attributes) {
            out += " " + attr->Key->value + "=\"";
            if (const auto stringLiteral = dynamic_cast<const StringLiteral*>(attr->Value.get())) {
                out += stringLiteral->value;
            } else if (const auto identifier = dynamic_cast<const Identifier*>(attr->Value.get())) {
                out += identifier->value;
            }
            out += "\"";
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
