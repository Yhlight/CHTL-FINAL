#pragma once

#include "Node.h"
#include "Expression.h"
#include "Attribute.h"
#include <vector>
#include <memory>

namespace CHTL {

    class BlockStatement : public Statement {
    public:
        std::string ToString() const override {
            std::string out = "{";
            for (const auto& stmt : statements) {
                out += stmt->ToString();
            }
            out += "}";
            return out;
        }

        std::vector<std::unique_ptr<Statement>> statements;
    };

    class ElementStatement : public Statement {
    public:
        ElementStatement(std::unique_ptr<Identifier> tag, std::unique_ptr<BlockStatement> body)
            : Tag(std::move(tag)), Body(std::move(body)) {}

        std::string ToString() const override {
            std::string out = Tag->ToString();
            for (const auto& attr : Attributes) {
                out += " " + attr->ToString();
            }
            out += " " + Body->ToString();
            return out;
        }

        std::unique_ptr<Identifier> Tag;
        std::vector<std::unique_ptr<Attribute>> Attributes;
        std::unique_ptr<BlockStatement> Body;
    };

    class TextStatement : public Statement {
    public:
        TextStatement(Token token, std::unique_ptr<Expression> text)
            : token(std::move(token)), Text(std::move(text)) {}

        std::string ToString() const override {
            return Text->ToString();
        }

        Token token; // The Token::TEXT token
        std::unique_ptr<Expression> Text;
    };

} // namespace CHTL
