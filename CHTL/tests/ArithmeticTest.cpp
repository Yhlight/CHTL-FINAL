#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "eval/Value.h"

using namespace CHTL;

TEST_CASE("Arithmetic operations on Value objects", "[evaluator][arithmetic]")
{
    Value v10px{ValueType::NUMBER, 10.0, "px"};
    Value v5px{ValueType::NUMBER, 5.0, "px"};
    Value v2em{ValueType::NUMBER, 2.0, "em"};
    Value v_no_unit{ValueType::NUMBER, 2.0, ""};

    SECTION("Addition")
    {
        SECTION("Same Unit") {
            Value result = v10px + v5px;
            REQUIRE(result.num == 15.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("With Empty Unit") {
            Value result = v10px + v_no_unit;
            REQUIRE(result.num == 12.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("To Empty Unit") {
            Value result = v_no_unit + v10px;
            REQUIRE(result.num == 12.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("Different Units") {
            REQUIRE_THROWS(v10px + v2em);
        }
    }

    SECTION("Subtraction")
    {
        SECTION("Same Unit") {
            Value result = v10px - v5px;
            REQUIRE(result.num == 5.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("With Empty Unit") {
            Value result = v10px - v_no_unit;
            REQUIRE(result.num == 8.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("From Empty Unit") {
            Value result = v_no_unit - v10px;
            REQUIRE(result.num == -8.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("Different Units") {
            REQUIRE_THROWS(v10px - v2em);
        }
    }

    SECTION("Multiplication")
    {
        SECTION("With Empty Unit") {
            Value result = v10px * v_no_unit;
            REQUIRE(result.num == 20.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("By Empty Unit") {
            Value result = v_no_unit * v10px;
            REQUIRE(result.num == 20.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("Different Units") {
            REQUIRE_THROWS(v10px * v2em);
        }
        SECTION("Same Units") {
            REQUIRE_THROWS(v10px * v5px);
        }
    }

    SECTION("Division")
    {
        SECTION("Same Unit") {
            Value result = v10px / v5px;
            REQUIRE(result.num == 2.0);
            REQUIRE(result.unit == ""); // Units should cancel out
        }
        SECTION("By Empty Unit") {
            Value result = v10px / v_no_unit;
            REQUIRE(result.num == 5.0);
            REQUIRE(result.unit == "px");
        }
        SECTION("Different Units") {
            REQUIRE_THROWS(v10px / v2em);
        }
        SECTION("By Zero") {
            Value v0{ValueType::NUMBER, 0.0, ""};
            REQUIRE_THROWS(v10px / v0);
        }
    }
}
