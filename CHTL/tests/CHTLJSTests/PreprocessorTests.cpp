#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTLJS/include/Preprocessor.h"

TEST_CASE("CHTLJS Preprocessor wraps ScriptMain", "[preprocessor]") {
    std::string input = R"(
        ScriptMain {
            // some chtl js code
        }
    )";
    std::string expected = R"(
        [__CHTLJS__]ScriptMain {
            // some chtl js code
        }[__ENDCHTLJS__]
    )";
    REQUIRE(CHTLJS::Preprocessor::process(input) == expected);
}

TEST_CASE("CHTLJS Preprocessor wraps Listen", "[preprocessor]") {
    std::string input = R"(
        {{box}}->Listen {
            click: () => {}
        };
    )";
    std::string expected = R"(
        [__CHTLJS__]{{box}}->Listen {
            click: () => {}
        };[__ENDCHTLJS__]
    )";
    REQUIRE(CHTLJS::Preprocessor::process(input) == expected);
}

TEST_CASE("CHTLJS Preprocessor wraps multiple blocks", "[preprocessor]") {
    std::string input = R"(
        let a = 1;
        ScriptMain {
            // chtl js
        }
        let b = 2;
        {{box}}->Listen {
            click: () => {}
        };
    )";
    std::string expected = R"(
        let a = 1;
        [__CHTLJS__]ScriptMain {
            // chtl js
        }[__ENDCHTLJS__]
        let b = 2;
        [__CHTLJS__]{{box}}->Listen {
            click: () => {}
        };[__ENDCHTLJS__]
    )";
    REQUIRE(CHTLJS::Preprocessor::process(input) == expected);
}

TEST_CASE("CHTLJS Preprocessor handles no blocks", "[preprocessor]") {
    std::string input = R"(
        let a = 1;
        let b = 2;
        function foo() {
            return a + b;
        }
    )";
    REQUIRE(CHTLJS::Preprocessor::process(input) == input);
}
