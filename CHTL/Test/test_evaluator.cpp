#include "catch.hpp"
#include "CHTLEvaluator/Evaluator.hpp"
#include "CHTLNode/LiteralNode.hpp"
#include "CHTLNode/BinaryOpNode.hpp"

TEST_CASE(EvaluatorCanEvaluateLiteral, "Evaluator can evaluate a literal") {
    LiteralNode literal("100px");
    Evaluator evaluator;
    std::string result = evaluator.evaluate(literal);
    REQUIRE(result == "100px");
}

TEST_CASE(EvaluatorCanEvaluateBinaryOperation, "Evaluator can evaluate a binary operation") {
    auto left = std::make_unique<LiteralNode>("100px");
    auto right = std::make_unique<LiteralNode>("50px");
    BinaryOpNode bin_op(std::move(left), "+", std::move(right));
    Evaluator evaluator;
    std::string result = evaluator.evaluate(bin_op);
    REQUIRE(result == "150px");
}
