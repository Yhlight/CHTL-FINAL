#include "Generator.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Expression.h"
#include "../CHTLNode/Style.h"

namespace CHTL {

    std::string Generator::Generate(const Program* program) {
        std::string out;
        for (const auto& stmt : program->statements) {
            out += GenerateStatement(stmt.get());
        }
        return out;
    }

    std::string Generator::GenerateStatement(const Statement* stmt) {
        if (const auto elementStmt = dynamic_cast<const ElementStatement*>(stmt)) {
            return GenerateElementStatement(elementStmt);
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

    std::string Generator::GenerateElementStatement(const ElementStatement* stmt) {
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
                    style_string += " " + prop->Key->value + ": ";
                    if (const auto stringLiteral = dynamic_cast<const StringLiteral*>(prop->Value.get())) {
                        style_string += stringLiteral->value;
                    } else if (const auto identifier = dynamic_cast<const Identifier*>(prop->Value.get())) {
                        style_string += identifier->value;
                    }
                    style_string += ";";
                }
            }
        }

        if (!style_string.empty()) {
            out += " style=\"" + style_string + "\"";
        }

        out += ">";
        out += GenerateBlockStatement(stmt->Body.get());
        out += "</" + stmt->Tag->value + ">";
        return out;
    }

    std::string Generator::GenerateBlockStatement(const BlockStatement* stmt) {
        std::string out;
        for (const auto& innerStmt : stmt->statements) {
            out += GenerateStatement(innerStmt.get());
        }
        return out;
    }

} // namespace CHTL
