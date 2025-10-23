#include "gtest/gtest.h"
#include "eval/Value.h"

using namespace CHTL;

// Test fixture for arithmetic operations
class ArithmeticTest : public ::testing::Test {
protected:
    Value v10px{ValueType::NUMBER, 10.0, "px"};
    Value v5px{ValueType::NUMBER, 5.0, "px"};
    Value v2em{ValueType::NUMBER, 2.0, "em"};
    Value v_no_unit{ValueType::NUMBER, 2.0, ""};
};

// Tests for Addition
TEST_F(ArithmeticTest, AdditionSameUnit) {
    Value result = v10px + v5px;
    EXPECT_EQ(result.num, 15.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, AdditionWithEmptyUnit) {
    Value result = v10px + v_no_unit;
    EXPECT_EQ(result.num, 12.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, AdditionToEmptyUnit) {
    Value result = v_no_unit + v10px;
    EXPECT_EQ(result.num, 12.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, AdditionDifferentUnits) {
    EXPECT_THROW(v10px + v2em, std::runtime_error);
}

// Tests for Subtraction
TEST_F(ArithmeticTest, SubtractionSameUnit) {
    Value result = v10px - v5px;
    EXPECT_EQ(result.num, 5.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, SubtractionWithEmptyUnit) {
    Value result = v10px - v_no_unit;
    EXPECT_EQ(result.num, 8.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, SubtractionFromEmptyUnit) {
    Value result = v_no_unit - v10px;
    EXPECT_EQ(result.num, -8.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, SubtractionDifferentUnits) {
    EXPECT_THROW(v10px - v2em, std::runtime_error);
}

// Tests for Multiplication
TEST_F(ArithmeticTest, MultiplicationWithEmptyUnit) {
    Value result = v10px * v_no_unit;
    EXPECT_EQ(result.num, 20.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, MultiplicationByEmptyUnit) {
    Value result = v_no_unit * v10px;
    EXPECT_EQ(result.num, 20.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, MultiplicationDifferentUnits) {
    EXPECT_THROW(v10px * v2em, std::runtime_error);
}

TEST_F(ArithmeticTest, MultiplicationSameUnits) {
    EXPECT_THROW(v10px * v5px, std::runtime_error);
}

// Tests for Division
TEST_F(ArithmeticTest, DivisionSameUnit) {
    Value result = v10px / v5px;
    EXPECT_EQ(result.num, 2.0);
    EXPECT_EQ(result.unit, ""); // Units should cancel out
}

TEST_F(ArithmeticTest, DivisionByEmptyUnit) {
    Value result = v10px / v_no_unit;
    EXPECT_EQ(result.num, 5.0);
    EXPECT_EQ(result.unit, "px");
}

TEST_F(ArithmeticTest, DivisionDifferentUnits) {
    EXPECT_THROW(v10px / v2em, std::runtime_error);
}

TEST_F(ArithmeticTest, DivisionByZero) {
    Value v0{ValueType::NUMBER, 0.0, ""};
    EXPECT_THROW(v10px / v0, std::runtime_error);
}
