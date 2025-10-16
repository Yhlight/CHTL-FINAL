#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Evaluator.h"
#include "AstNode.h"

#include <memory>

TEST_CASE("Evaluator correctly evaluates expressions", "[evaluator]")
{
    CHTL::Evaluator evaluator;
    CHTL::EvalContext context; // Create a single empty context for all tests

    SECTION("Evaluates a NumberLiteral")
    {
        auto num = std::make_unique<CHTL::NumberLiteral>();
        num->value = 123.45;
        REQUIRE(evaluator.Eval(num.get(), context).num == 123.45);
        REQUIRE(evaluator.Eval(num.get(), context).unit == "");
    }

    SECTION("Evaluates a simple addition")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 20;

        infix->left = std::move(left);
        infix->op = "+";
        infix->right = std::move(right);

        REQUIRE(evaluator.Eval(infix.get(), context).num == 30.0);
    }

    SECTION("Evaluates with correct operator precedence")
    {
        // AST for "10 + 20 * 3" should be (10 + (20 * 3))
        auto root = std::make_unique<CHTL::InfixExpression>();
        root->op = "+";

        auto left_10 = std::make_unique<CHTL::NumberLiteral>();
        left_10->value = 10;
        root->left = std::move(left_10);

        auto right_mult = std::make_unique<CHTL::InfixExpression>();
        right_mult->op = "*";

        auto left_20 = std::make_unique<CHTL::NumberLiteral>();
        left_20->value = 20;
        right_mult->left = std::move(left_20);

        auto right_3 = std::make_unique<CHTL::NumberLiteral>();
        right_3->value = 3;
        right_mult->right = std::move(right_3);

        root->right = std::move(right_mult);

        REQUIRE(evaluator.Eval(root.get(), context).num == 70.0);
    }

    // --- Unit Tests ---
    SECTION("Evaluates same-unit addition")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        left->unit = "px";
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 5;
        right->unit = "px";
        infix->left = std::move(left);
        infix->op = "+";
        infix->right = std::move(right);

        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.num == 15.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("Evaluates number + unit addition")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 5;
        right->unit = "px";
        infix->left = std::move(left);
        infix->op = "+";
        infix->right = std::move(right);

        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.num == 15.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("Evaluates multiplication")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        left->unit = "px";
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 2;
        infix->left = std::move(left);
        infix->op = "*";
        infix->right = std::move(right);

        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.num == 20.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("Throws error for different units addition")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        left->unit = "px";
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 5;
        right->unit = "em";
        infix->left = std::move(left);
        infix->op = "+";
        infix->right = std::move(right);

        REQUIRE_THROWS(evaluator.Eval(infix.get(), context));
    }
}

TEST_CASE("Evaluator correctly handles modulo and power operators", "[evaluator]")
{
    CHTL::Evaluator evaluator;
    CHTL::EvalContext context;

    SECTION("Evaluates power operator")
    {
        // 2 ** 3
        auto root = std::make_unique<CHTL::InfixExpression>();
        root->op = "**";
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 2;
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 3;
        root->left = std::move(left);
        root->right = std::move(right);

        REQUIRE(evaluator.Eval(root.get(), context).num == 8.0);
    }

    SECTION("Evaluates modulo operator")
    {
        // 10 % 3
        auto root = std::make_unique<CHTL::InfixExpression>();
        root->op = "%";
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 3;
        root->left = std::move(left);
        root->right = std::move(right);

        REQUIRE(evaluator.Eval(root.get(), context).num == 1.0);
    }

    SECTION("Evaluates with correct power precedence")
    {
        // AST for "2 + 3 * 2 ** 2" should be (2 + (3 * (2 ** 2)))
        // -> (2 + (3 * 4)) -> (2 + 12) -> 14

        // Outermost: +
        auto root = std::make_unique<CHTL::InfixExpression>();
        root->op = "+";

        // Left of +: 2
        auto left_2 = std::make_unique<CHTL::NumberLiteral>();
        left_2->value = 2;
        root->left = std::move(left_2);

        // Right of +: (3 * (2 ** 2))
        auto right_mult = std::make_unique<CHTL::InfixExpression>();
        right_mult->op = "*";

        // Left of *: 3
        auto left_3 = std::make_unique<CHTL::NumberLiteral>();
        left_3->value = 3;
        right_mult->left = std::move(left_3);

        // Right of *: (2 ** 2)
        auto right_pow = std::make_unique<CHTL::InfixExpression>();
        right_pow->op = "**";

        auto pow_left_2 = std::make_unique<CHTL::NumberLiteral>();
        pow_left_2->value = 2;
        right_pow->left = std::move(pow_left_2);

        auto pow_right_2 = std::make_unique<CHTL::NumberLiteral>();
        pow_right_2->value = 2;
        right_pow->right = std::move(pow_right_2);

        right_mult->right = std::move(right_pow);
        root->right = std::move(right_mult);

        REQUIRE(evaluator.Eval(root.get(), context).num == 14.0);
    }
}
