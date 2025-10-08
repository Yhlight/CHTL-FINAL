#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/ExpressionParser.h"
#include "CHTL/CHTLNode/NumericLiteralExprNode.h"
#include "CHTL/CHTLNode/BinaryOpExprNode.h"

TEST_CASE("ExpressionParser correctly parses expressions", "[expression_parser]") {
    SECTION("Parse a single numeric literal") {
        Lexer lexer("123");
        ExpressionParser parser(lexer.tokenize());
        auto expr = parser.parse();

        REQUIRE(expr != nullptr);
        auto* literal = dynamic_cast<NumericLiteralExprNode*>(expr.get());
        REQUIRE(literal != nullptr);
        REQUIRE(literal->value == 123);
    }

    SECTION("Parse a simple addition expression") {
        Lexer lexer("1 + 2");
        ExpressionParser parser(lexer.tokenize());
        auto expr = parser.parse();

        REQUIRE(expr != nullptr);
        auto* binaryOp = dynamic_cast<BinaryOpExprNode*>(expr.get());
        REQUIRE(binaryOp != nullptr);
        REQUIRE(binaryOp->op == TokenType::PLUS);

        auto* left = dynamic_cast<NumericLiteralExprNode*>(binaryOp->left.get());
        REQUIRE(left != nullptr);
        REQUIRE(left->value == 1);

        auto* right = dynamic_cast<NumericLiteralExprNode*>(binaryOp->right.get());
        REQUIRE(right != nullptr);
        REQUIRE(right->value == 2);
    }

    SECTION("Parse an expression with operator precedence") {
        Lexer lexer("1 + 2 * 3");
        ExpressionParser parser(lexer.tokenize());
        auto expr = parser.parse();

        REQUIRE(expr != nullptr);
        auto* addOp = dynamic_cast<BinaryOpExprNode*>(expr.get());
        REQUIRE(addOp != nullptr);
        REQUIRE(addOp->op == TokenType::PLUS);

        auto* left = dynamic_cast<NumericLiteralExprNode*>(addOp->left.get());
        REQUIRE(left != nullptr);
        REQUIRE(left->value == 1);

        auto* multiplyOp = dynamic_cast<BinaryOpExprNode*>(addOp->right.get());
        REQUIRE(multiplyOp != nullptr);
        REQUIRE(multiplyOp->op == TokenType::STAR);

        auto* multLeft = dynamic_cast<NumericLiteralExprNode*>(multiplyOp->left.get());
        REQUIRE(multLeft != nullptr);
        REQUIRE(multLeft->value == 2);

        auto* multRight = dynamic_cast<NumericLiteralExprNode*>(multiplyOp->right.get());
        REQUIRE(multRight != nullptr);
        REQUIRE(multRight->value == 3);
    }
}