#pragma once

#include "Node.h"
#include "Expression.h"
#include <memory>
#include <utility>

namespace CHTL {

    class Attribute : public Node {
    public:
        Attribute(Token token, std::unique_ptr<Identifier> key, std::unique_ptr<Expression> value)
            : token(std::move(token)), Key(std::move(key)), Value(std::move(value)) {}

        std::string ToString() const override {
            std::string out = Key->ToString();
            out += ": ";
            out += Value->ToString();
            out += ";";
            return out;
        }

        Token token; // The ':' or '=' token
        std::unique_ptr<Identifier> Key;
        std::unique_ptr<Expression> Value;
    };

} // namespace CHTL
