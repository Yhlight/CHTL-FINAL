#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "generator/Generator.h"
#include "AstNode.h"

#include <string>
#include <memory>

// Helper to check for parsing errors from previous steps
void checkParserErrors(const CHTL::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }
    std::string error_messages;
    for (const auto& msg : errors) {
        error_messages += msg + "\n";
    }
    FAIL("Parser has errors:\n" << error_messages);
}

TEST_CASE("Generator handles conditional rendering with if/else blocks", "[generator][if]")
{
    SECTION("Renders content when if condition is true")
    {
        std::string input = R"(
            div {
                if {
                    condition: 1 > 0;
                    p { text: "Success!"; }
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = R"(<div><p>Success!</p></div>)";
        REQUIRE(html_output == expected_html);
    }

    SECTION("Does not render content when if condition is false")
    {
        std::string input = R"(
            div {
                if {
                    condition: 1 < 0;
                    p { text: "Failure!"; }
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = "<div></div>";
        REQUIRE(html_output == expected_html);
    }

    SECTION("Renders else block when if condition is false")
    {
        std::string input = R"(
            div {
                if {
                    condition: 1 < 0;
                    p { text: "IF"; }
                } else {
                    p { text: "ELSE"; }
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = R"(<div><p>ELSE</p></div>)";
        REQUIRE(html_output == expected_html);
    }

    SECTION("Correctly processes if-else if-else chain")
    {
        std::string input = R"(
            div {
                if {
                    condition: 1 < 0;
                    p { text: "IF"; }
                } else if {
                    condition: 1 > 0;
                    p { text: "ELSE IF"; }
                } else {
                    p { text: "ELSE"; }
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = R"(<div><p>ELSE IF</p></div>)";
        REQUIRE(html_output == expected_html);
    }

    SECTION("Handles nested if statements correctly")
    {
        std::string input = R"(
            div {
                if {
                    condition: 1 > 0;
                    p { text: "Outer IF"; }
                    if {
                        condition: 1 > 0;
                        span { text: "Inner IF"; }
                    }
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = R"(<div><p>Outer IF</p><span>Inner IF</span></div>)";
        REQUIRE(html_output == expected_html);
    }

    SECTION("Applies conditional styles correctly")
    {
        std::string input = R"(
            div {
                if {
                    condition: 1 > 0;
                    width: 100px;
                } else {
                    width: 200px;
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = R"(<div style="width:100px;"></div>)";
        REQUIRE(html_output == expected_html);
    }
}
