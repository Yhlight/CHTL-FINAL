#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTLJS/src/preprocessor/Preprocessor.h"
#include "CHTLJS/include/lexer/Lexer.h"

TEST_CASE("CHTLJS Preprocessor correctly wraps CHTL JS blocks", "[preprocessor]")
{
    SECTION("Simple CHTLJS block")
    {
        std::string input = "Listen { click: () => {} };";
        std::string expected = " [__CHTLJS__] Listen { click: () => {} }; [__CHTLJSEND__] ";
        REQUIRE(CHTLJS::Preprocessor::Process(input) == expected);
    }

    SECTION("Mixed JS and CHTLJS")
    {
        std::string input = "console.log('hello');\n"
                            "{{button}}->Listen { click: () => {} };\n"
                            "var x = 10;";
        std::string expected = "console.log('hello');\n"
                               " [__CHTLJS__] {{button}}->Listen { click: () => {} }; [__CHTLJSEND__] \n"
                               "var x = 10;";
        REQUIRE(CHTLJS::Preprocessor::Process(input) == expected);
    }

    SECTION("Multiple CHTLJS blocks")
    {
        std::string input = "Animate { duration: 100 };\n"
                            "console.log('between');\n"
                            "Vir myVir = Listen { };";
        std::string expected = " [__CHTLJS__] Animate { duration: 100 }; [__CHTLJSEND__] \n"
                               "console.log('between');\n"
                               " [__CHTLJS__] Vir myVir = Listen { }; [__CHTLJSEND__] ";
        REQUIRE(CHTLJS::Preprocessor::Process(input) == expected);
    }
}

// TEST_CASE("CHTLJS Lexer correctly handles preprocessed code", "[lexer]")
// {
//     SECTION("Lexer tokenizes preprocessed CHTLJS as RAW_JS")
//     {
//         std::string input = "[__CHTLJS__]Listen { click: () => {} };[__CHTLJSEND__]";
//         CHTLJS::Lexer lexer(input);
//         CHTLJS::Token token = lexer.NextToken();
//         REQUIRE(token.type == CHTLJS::TokenType::RAW_JS);
//         REQUIRE(token.literal == "Listen { click: () => {} };");
//     }

//     SECTION("Lexer handles mixed content")
//     {
//         std::string input = "var x = 10; [__CHTLJS__]{{button}}->click();[__CHTLJSEND__] console.log('done');";
//         CHTLJS::Lexer lexer(input);

//         CHTLJS::Token token1 = lexer.NextToken();
//         REQUIRE(token1.type == CHTLJS::TokenType::RAW_JS);
//         REQUIRE(token1.literal == "var x = 10; ");

//         CHTLJS::Token token2 = lexer.NextToken();
//         REQUIRE(token2.type == CHTLJS::TokenType::RAW_JS);
//         REQUIRE(token2.literal == "{{button}}->click();");

//         CHTLJS::Token token3 = lexer.NextToken();
//         REQUIRE(token3.type == CHTLJS::TokenType::RAW_JS);
//         REQUIRE(token3.literal == " console.log('done');");
//     }
// }
