#pragma once

#include "Node.h"
#include "Expression.h"
#include <vector>
#include <memory>
#include <utility>

namespace CHTL {

    class StyleProperty : public Node {
    public:
        StyleProperty(Token token, std::unique_ptr<Identifier> key, std::unique_ptr<Expression> value)
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

    class StyleRule : public Node {
    public:
        StyleRule(Token token, std::unique_ptr<Expression> selector)
            : token(std::move(token)), Selector(std::move(selector)) {}

        std::string ToString() const override {
            std::string out = Selector->ToString();
            out += " {";
            for (const auto& prop : Properties) {
                out += " " + prop->ToString();
            }
            out += " }";
            return out;
        }

        Token token; // The '.' or '#' token
        std::unique_ptr<Expression> Selector;
        std::vector<std::unique_ptr<StyleProperty>> Properties;
    };


    class StyleStatement : public Statement {
    public:
        StyleStatement(Token token) : token(std::move(token)) {}

        std::string ToString() const override {
            std::string out = "style {";
            for (const auto& prop : Properties) {
                out += " " + prop->ToString();
            }
            for (const auto& rule : Rules) {
                out += " " + rule->ToString();
            }
            out += " }";
            return out;
        }

        Token token; // The 'style' token
        std::vector<std::unique_ptr<StyleProperty>> Properties;
        std::vector<std::unique_ptr<StyleRule>> Rules;
    };

} // namespace CHTL
