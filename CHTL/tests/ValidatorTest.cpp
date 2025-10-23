#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "validator/Validator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

void checkValidatorErrors(CHTL::Validator& v, const CHTL::ProgramNode* program, int expected_errors)
{
    v.Validate(program);
    auto errors = v.GetErrors();
    REQUIRE(errors.size() == expected_errors);
}

TEST_CASE("Validator correctly identifies constraint violations", "[validator]")
{
    CHTL::Validator validator;

    SECTION("No violations")
    {
        std::string input = "div { p {} }";
        ::CHTL::Lexer l(input);
        ::CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkValidatorErrors(validator, program.get(), 0);
    }

    SECTION("Precise element violation")
    {
        std::string input = "div { except span; span {} }";
        ::CHTL::Lexer l(input);
        ::CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkValidatorErrors(validator, program.get(), 1);
    }

    SECTION("Type violation (@Html)")
    {
        std::string input = "div { except @Html; p {} }";
        ::CHTL::Lexer l(input);
        ::CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkValidatorErrors(validator, program.get(), 1);
    }

    SECTION("Nested scope violation")
    {
        std::string input = "div { except span; p { span {} } }";
        ::CHTL::Lexer l(input);
        ::CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkValidatorErrors(validator, program.get(), 1);
    }

    SECTION("No violation after exiting scope")
    {
        std::string input = "div { p { except span; } } span {}";
        ::CHTL::Lexer l(input);
        ::CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkValidatorErrors(validator, program.get(), 0);
    }
}
