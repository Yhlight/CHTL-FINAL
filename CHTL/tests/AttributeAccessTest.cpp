#include "gtest/gtest.h"
#include "eval/Evaluator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "AstNode.h"
#include <memory>

using namespace CHTL;

class AttributeAccessTest : public ::testing::Test {
protected:
    void SetUp() override {
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
        program = p.ParseProgram();
        EXPECT_TRUE(p.GetErrors().empty());
        context.program = program.get();
    }

    std::unique_ptr<ProgramNode> program;
    Evaluator evaluator;
    EvalContext context;
};

TEST_F(AttributeAccessTest, AccessByTag) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = "div";
    expr->attribute_name = "width"; // The first div has 'width'

    // The selector "div" matches two nodes. Without an index, the evaluator
    // should pick the first one, which is the outer div with id="main-container".
    // This div has a `width` of 500px.
    auto result = evaluator.Eval(expr.get(), context);
    EXPECT_EQ(result.type, ValueType::NUMBER);
    EXPECT_DOUBLE_EQ(result.num, 500.0);
    EXPECT_EQ(result.unit, "px");
}


TEST_F(AttributeAccessTest, AccessById) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = "#main-container";
    expr->attribute_name = "width";
    auto result = evaluator.Eval(expr.get(), context);
    EXPECT_EQ(result.type, ValueType::NUMBER);
    EXPECT_DOUBLE_EQ(result.num, 500.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(AttributeAccessTest, AccessByClass) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = ".container";
    expr->attribute_name = "width";
    auto result = evaluator.Eval(expr.get(), context);
    EXPECT_EQ(result.type, ValueType::NUMBER);
    EXPECT_DOUBLE_EQ(result.num, 500.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(AttributeAccessTest, IndexedAccess) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = ".item[1]"; // The second span with class="item"
    expr->attribute_name = "font-size";
    auto result = evaluator.Eval(expr.get(), context);
    EXPECT_EQ(result.type, ValueType::NUMBER);
    EXPECT_DOUBLE_EQ(result.num, 16.0);
    EXPECT_EQ(result.unit, "px");
}


// --- Error Handling Tests ---
TEST_F(AttributeAccessTest, ThrowsOnInvalidSelector) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = ".non-existent-class";
    expr->attribute_name = "width";
    EXPECT_THROW(evaluator.Eval(expr.get(), context), std::runtime_error);
}

TEST_F(AttributeAccessTest, ThrowsOnInvalidAttribute) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = "#main-container";
    expr->attribute_name = "non-existent-attribute";
    EXPECT_THROW(evaluator.Eval(expr.get(), context), std::runtime_error);
}

TEST_F(AttributeAccessTest, ThrowsOnIndexOutOfBounds) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = ".item[10]";
    expr->attribute_name = "height";
    EXPECT_THROW(evaluator.Eval(expr.get(), context), std::runtime_error);
}

// --- Descendant Selector Test ---
TEST_F(AttributeAccessTest, AccessByDescendantSelector) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = ".container .item"; // First span inside .container
    expr->attribute_name = "height";
    auto result = evaluator.Eval(expr.get(), context);
    EXPECT_EQ(result.type, ValueType::NUMBER);
    EXPECT_DOUBLE_EQ(result.num, 50.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(AttributeAccessTest, AccessByComplexDescendantSelector) {
    auto expr = std::make_unique<AttributeAccessExpression>();
    expr->selector = "#main-container .inner-div span"; // The span inside .inner-div
    expr->attribute_name = "font-size";
    auto result = evaluator.Eval(expr.get(), context);
    EXPECT_EQ(result.type, ValueType::NUMBER);
    EXPECT_DOUBLE_EQ(result.num, 16.0);
    EXPECT_EQ(result.unit, "px");
}
