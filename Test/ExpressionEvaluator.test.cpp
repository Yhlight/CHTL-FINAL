#include "catch.hpp"
#include "CHTL/CHTLGenerator/ExpressionEvaluator.h"
#include "CHTL/CHTLNode/NumericLiteralExprNode.h"
#include "CHTL/CHTLNode/IdentifierExprNode.h"
#include "CHTL/CHTLNode/BinaryOpExprNode.h"

TEST_CASE("ExpressionEvaluator correctly evaluates expressions", "[expression_evaluator]") {
    SECTION("Evaluate a single numeric literal") {
        NumericLiteralExprNode node(123, "px");
        ExpressionEvaluator evaluator;
        auto result = evaluator.evaluate(node);
        REQUIRE(result.isNumeric);
        REQUIRE(result.value == 123);
        REQUIRE(result.unit == "px");
    }

    SECTION("Evaluate a simple addition expression") {
        auto left = std::make_unique<NumericLiteralExprNode>(10, "px");
        auto right = std::make_unique<NumericLiteralExprNode>(5, "px");
        BinaryOpExprNode node(std::move(left), TokenType::PLUS, std::move(right));

        ExpressionEvaluator evaluator;
        auto result = evaluator.evaluate(node);
        REQUIRE(result.isNumeric);
        REQUIRE(result.value == 15);
        REQUIRE(result.unit == "px");
    }

    SECTION("Evaluate an expression with operator precedence") {
        // 10 + 2 * 5 -> should be 20
        auto two = std::make_unique<NumericLiteralExprNode>(2);
        auto five = std::make_unique<NumericLiteralExprNode>(5);
        auto mult = std::make_unique<BinaryOpExprNode>(std::move(two), TokenType::STAR, std::move(five));

        auto ten = std::make_unique<NumericLiteralExprNode>(10);
        BinaryOpExprNode add(std::move(ten), TokenType::PLUS, std::move(mult));

        ExpressionEvaluator evaluator;
        auto result = evaluator.evaluate(add);
        REQUIRE(result.isNumeric);
        REQUIRE(result.value == 20);
    }

    SECTION("Evaluate an expression with an identifier") {
        IdentifierExprNode node("auto");
        ExpressionEvaluator evaluator;
        auto result = evaluator.evaluate(node);
        REQUIRE_FALSE(result.isNumeric);
        REQUIRE(result.stringValue == "auto");
    }

    SECTION("Throw on incompatible units") {
        auto left = std::make_unique<NumericLiteralExprNode>(10, "px");
        auto right = std::make_unique<NumericLiteralExprNode>(5, "em");
        BinaryOpExprNode node(std::move(left), TokenType::PLUS, std::move(right));

        ExpressionEvaluator evaluator;
        REQUIRE_THROWS_WITH(evaluator.evaluate(node), "Incompatible units for arithmetic operation: px and em");
    }
}