#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "eval/Evaluator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
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

TEST_CASE("Evaluator handles indexed attribute access", "[Evaluator]")
{
    std::string input = R"(
        div {
            class = "button";
            style { width: 100px; }
        }
        div {
            class = "button";
            style { width: 150px; }
        }
        div {
            class = "button";
            style { width: 200px; }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    REQUIRE(p.GetErrors().empty());

    CHTL::Evaluator evaluator;
    CHTL::EvalContext context;
    context.program = program.get();

    auto expr1 = std::make_unique<CHTL::AttributeAccessExpression>();
    expr1->selector = ".button[1]";
    expr1->attribute_name = "width";
    CHTL::Value result1 = evaluator.Eval(expr1.get(), context);
    REQUIRE(result1.type == CHTL::ValueType::NUMBER);
    REQUIRE(result1.num == 150.0);
    REQUIRE(result1.unit == "px");

    auto expr2 = std::make_unique<CHTL::AttributeAccessExpression>();
    expr2->selector = ".button[2]";
    expr2->attribute_name = "width";
    CHTL::Value result2 = evaluator.Eval(expr2.get(), context);
    REQUIRE(result2.type == CHTL::ValueType::NUMBER);
    REQUIRE(result2.num == 200.0);
    REQUIRE(result2.unit == "px");
}

TEST_CASE("Evaluator handles decoupled string expressions", "[evaluator][expression]")
{
    CHTL::Evaluator evaluator;
    CHTL::EvalContext context;

    SECTION("Evaluates a decoupled string multiplication")
    {
        // AST for "(linear 0.5s all) * 2"
        auto root = std::make_unique<CHTL::InfixExpression>();
        root->op = "*";

        auto left_decoupled = std::make_unique<CHTL::DecoupledStringExpression>();
        left_decoupled->string_part = "linear %s all";
        auto num_part = std::make_unique<CHTL::NumberLiteral>();
        num_part->value = 0.5;
        num_part->unit = "s";
        left_decoupled->number_part = std::move(num_part);
        root->left = std::move(left_decoupled);

        auto right_num = std::make_unique<CHTL::NumberLiteral>();
        right_num->value = 2;
        root->right = std::move(right_num);

        auto result = evaluator.Eval(root.get(), context);
        REQUIRE(result.type == CHTL::ValueType::NUMBER);
        REQUIRE(result.num == 1.0);
        REQUIRE(result.unit == "s");
        REQUIRE(result.string_template == "linear %s all");
    }
}

TEST_CASE("Attribute Access", "[evaluator]")
{
    CHTL::Evaluator evaluator;
    CHTL::ProgramNode program;
    CHTL::EvalContext context{ {}, &program};

    // Setup a simple DOM-like structure for testing
    auto div1 = std::make_unique<CHTL::ElementNode>();
    div1->tag_name = "div";
    div1->attributes.push_back({"id", "box"});
    div1->attributes.push_back({"class", "container"});
    auto style1 = std::make_unique<CHTL::StyleNode>();
    auto prop1 = std::make_unique<CHTL::StyleProperty>();
    prop1->name = "width";
    auto val1 = std::make_unique<CHTL::NumberLiteral>();
    val1->value = 100;
    val1->unit = "px";
    prop1->value = std::move(val1);
    style1->children.push_back(std::move(prop1));
    div1->children.push_back(std::move(style1));

    auto span1 = std::make_unique<CHTL::ElementNode>();
    span1->tag_name = "span";
    span1->attributes.push_back({"class", "item"});
    div1->children.push_back(std::move(span1));

    program.children.push_back(std::move(div1));

    SECTION("Access by ID")
    {
        auto access = std::make_unique<CHTL::AttributeAccessExpression>();
        access->selector = "#box";
        access->attribute_name = "width";
        auto result = evaluator.Eval(access.get(), context);
        REQUIRE(result.type == CHTL::ValueType::NUMBER);
        REQUIRE(result.num == 100);
        REQUIRE(result.unit == "px");
    }

    SECTION("Access by Class")
    {
        auto access = std::make_unique<CHTL::AttributeAccessExpression>();
        access->selector = ".container";
        access->attribute_name = "width";
        auto result = evaluator.Eval(access.get(), context);
        REQUIRE(result.type == CHTL::ValueType::NUMBER);
        REQUIRE(result.num == 100);
        REQUIRE(result.unit == "px");
    }

    SECTION("Access by Tag Name")
    {
        auto access = std::make_unique<CHTL::AttributeAccessExpression>();
        access->selector = "div";
        access->attribute_name = "width";
        auto result = evaluator.Eval(access.get(), context);
        REQUIRE(result.type == CHTL::ValueType::NUMBER);
        REQUIRE(result.num == 100);
        REQUIRE(result.unit == "px");
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

TEST_CASE("Evaluator handles division with units", "[evaluator]")
{
    CHTL::Evaluator evaluator;
    CHTL::EvalContext context;

    SECTION("Division of same units results in a unitless number")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 100;
        left->unit = "px";
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 2;
        right->unit = "px";
        infix->left = std::move(left);
        infix->op = "/";
        infix->right = std::move(right);

        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.num == 50.0);
        REQUIRE(result.unit == "");
    }

    SECTION("Division by a unitless number preserves the unit")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 100;
        left->unit = "px";
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 2;
        infix->left = std::move(left);
        infix->op = "/";
        infix->right = std::move(right);

        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.num == 50.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("Throws error for division of different units")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        left->value = 10;
        left->unit = "px";
        auto right = std::make_unique<CHTL::NumberLiteral>();
        right->value = 5;
        right->unit = "em";
        infix->left = std::move(left);
        infix->op = "/";
        infix->right = std::move(right);

        REQUIRE_THROWS(evaluator.Eval(infix.get(), context));
    }
}

TEST_CASE("Evaluator handles logical operators", "[evaluator][logical]")
{
    CHTL::Evaluator evaluator;
    CHTL::EvalContext context;

    // Helper to create an AST for a comparison to get a boolean Value
    auto make_bool_expr = [](bool val) {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        auto left = std::make_unique<CHTL::NumberLiteral>();
        auto right = std::make_unique<CHTL::NumberLiteral>();
        if (val) { // true -> 1 > 0
            left->value = 1;
            right->value = 0;
        } else { // false -> 0 > 1
            left->value = 0;
            right->value = 1;
        }
        infix->left = std::move(left);
        infix->op = ">";
        infix->right = std::move(right);
        return infix;
    };


    SECTION("Logical NOT")
    {
        auto prefix = std::make_unique<CHTL::PrefixExpression>();
        prefix->op = "!";
        prefix->right = make_bool_expr(true); // !true
        auto result = evaluator.Eval(prefix.get(), context);
        REQUIRE(result.type == CHTL::ValueType::BOOL);
        REQUIRE(result.boolean == false);

        prefix->right = make_bool_expr(false); // !false
        result = evaluator.Eval(prefix.get(), context);
        REQUIRE(result.type == CHTL::ValueType::BOOL);
        REQUIRE(result.boolean == true);
    }

    SECTION("Logical AND")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        infix->op = "&&";

        // true && true
        infix->left = make_bool_expr(true);
        infix->right = make_bool_expr(true);
        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.type == CHTL::ValueType::BOOL);
        REQUIRE(result.boolean == true);

        // true && false
        infix->left = make_bool_expr(true);
        infix->right = make_bool_expr(false);
        result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.type == CHTL::ValueType::BOOL);
        REQUIRE(result.boolean == false);
    }

    SECTION("Logical OR")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        infix->op = "||";

        // false || true
        infix->left = make_bool_expr(false);
        infix->right = make_bool_expr(true);
        auto result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.type == CHTL::ValueType::BOOL);
        REQUIRE(result.boolean == true);

        // false || false
        infix->left = make_bool_expr(false);
        infix->right = make_bool_expr(false);
        result = evaluator.Eval(infix.get(), context);
        REQUIRE(result.type == CHTL::ValueType::BOOL);
        REQUIRE(result.boolean == false);
    }

    SECTION("Throws on non-boolean operands")
    {
        auto infix = std::make_unique<CHTL::InfixExpression>();
        infix->op = "&&";
        infix->left = make_bool_expr(true);
        infix->right = std::make_unique<CHTL::NumberLiteral>(10, "");
        REQUIRE_THROWS(evaluator.Eval(infix.get(), context));

        auto prefix = std::make_unique<CHTL::PrefixExpression>();
        prefix->op = "!";
        prefix->right = std::make_unique<CHTL::NumberLiteral>(0, "");
        REQUIRE_THROWS(evaluator.Eval(prefix.get(), context));
    }
}
