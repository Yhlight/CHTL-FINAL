#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "eval/Evaluator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "AstNode.h"
#include <memory>

using namespace CHTL;

TEST_CASE("Attribute Access operations", "[evaluator][attribute_access]")
{
    // Setup a common AST structure for all tests
    std::string input = R"(
        div {
            id: "main-container";
            class: "container";
            style {
                width: 500px;
            }

            span {
                class: "item";
                style {
                    height: 50px;
                }
            }

            div {
                class: "inner-div";
                style {
                    opacity: 0.8;
                }

                span {
                     class: "item";
                     style {
                        font-size: 16px;
                     }
                }
            }
        }
        span {
            class: "outer-item";
            style {
                height: 30px;
            }
        }
    )";
    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();
    REQUIRE(p.GetErrors().empty());

    Evaluator evaluator;
    EvalContext context;
    context.program = program.get();

    SECTION("AccessByTag") {
        auto expr = std::make_unique<AttributeAccessExpression>();
        expr->selector = "div";
        expr->attribute_name = "width"; // The first div has 'width'
        auto result = evaluator.Eval(expr.get(), context);
        REQUIRE(result.type == ValueType::NUMBER);
        REQUIRE(result.num == 500.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("AccessById") {
        auto expr = std::make_unique<AttributeAccessExpression>();
        expr->selector = "#main-container";
        expr->attribute_name = "width";
        auto result = evaluator.Eval(expr.get(), context);
        REQUIRE(result.type == ValueType::NUMBER);
        REQUIRE(result.num == 500.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("AccessByClass") {
        auto expr = std::make_unique<AttributeAccessExpression>();
        expr->selector = ".container";
        expr->attribute_name = "width";
        auto result = evaluator.Eval(expr.get(), context);
        REQUIRE(result.type == ValueType::NUMBER);
        REQUIRE(result.num == 500.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("IndexedAccess") {
        auto expr = std::make_unique<AttributeAccessExpression>();
        expr->selector = ".item[1]"; // The second span with class="item"
        expr->attribute_name = "font-size";
        auto result = evaluator.Eval(expr.get(), context);
        REQUIRE(result.type == ValueType::NUMBER);
        REQUIRE(result.num == 16.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("Error Handling") {
        SECTION("ThrowsOnInvalidSelector") {
            auto expr = std::make_unique<AttributeAccessExpression>();
            expr->selector = ".non-existent-class";
            expr->attribute_name = "width";
            REQUIRE_THROWS(evaluator.Eval(expr.get(), context));
        }

        SECTION("ThrowsOnInvalidAttribute") {
            auto expr = std::make_unique<AttributeAccessExpression>();
            expr->selector = "#main-container";
            expr->attribute_name = "non-existent-attribute";
            REQUIRE_THROWS(evaluator.Eval(expr.get(), context));
        }

        SECTION("ThrowsOnIndexOutOfBounds") {
            auto expr = std::make_unique<AttributeAccessExpression>();
            expr->selector = ".item[10]";
            expr->attribute_name = "height";
            REQUIRE_THROWS(evaluator.Eval(expr.get(), context));
        }
    }

    SECTION("Descendant Selector") {
        SECTION("AccessByDescendantSelector") {
            auto expr = std::make_unique<AttributeAccessExpression>();
            expr->selector = ".container .item"; // First span inside .container
            expr->attribute_name = "height";
            auto result = evaluator.Eval(expr.get(), context);
            REQUIRE(result.type == ValueType::NUMBER);
            REQUIRE(result.num == 50.0);
            REQUIRE(result.unit == "px");
        }

        SECTION("AccessByComplexDescendantSelector") {
            auto expr = std::make_unique<AttributeAccessExpression>();
            expr->selector = "#main-container .inner-div span"; // The span inside .inner-div
            expr->attribute_name = "font-size";
            auto result = evaluator.Eval(expr.get(), context);
            REQUIRE(result.type == ValueType::NUMBER);
            REQUIRE(result.num == 16.0);
            REQUIRE(result.unit == "px");
        }
    }
}
