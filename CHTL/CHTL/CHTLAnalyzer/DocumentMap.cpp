#include "DocumentMap.h"

namespace CHTL {

    ElementMap DocumentMapBuilder::Build(const Program* program) {
        elementMap.clear();
        Traverse(program);
        return elementMap;
    }

    void DocumentMapBuilder::Traverse(const Node* node) {
        if (const auto elementStmt = dynamic_cast<const ElementStatement*>(node)) {
            for (const auto& attr : elementStmt->Attributes) {
                if (attr->Key->value == "id") {
                    if (const auto value = dynamic_cast<const Identifier*>(attr->Value.get())) {
                        elementMap["#" + value->value] = elementStmt;
                    } else if (const auto value = dynamic_cast<const StringLiteral*>(attr->Value.get())) {
                        elementMap["#" + value->value] = elementStmt;
                    }
                } else if (attr->Key->value == "class") {
                    if (const auto value = dynamic_cast<const Identifier*>(attr->Value.get())) {
                        elementMap["." + value->value] = elementStmt;
                    } else if (const auto value = dynamic_cast<const StringLiteral*>(attr->Value.get())) {
                        elementMap["." + value->value] = elementStmt;
                    }
                }
            }
            Traverse(elementStmt->Body.get());
        } else if (const auto program = dynamic_cast<const Program*>(node)) {
            for (const auto& stmt : program->statements) {
                Traverse(stmt.get());
            }
        } else if (const auto block = dynamic_cast<const BlockStatement*>(node)) {
            for (const auto& stmt : block->statements) {
                Traverse(stmt.get());
            }
        }
    }

} // namespace CHTL
