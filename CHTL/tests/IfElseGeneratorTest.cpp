#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "generator/Generator.h"
#include "AstNode.h"

#include <string>
#include <memory>
#include <unordered_map>

// Create a default keyword map for testing purposes
static const std::unordered_map<std::string, CHTL::Token> s_default_keywords = {
    {"text", {CHTL::TokenType::KEYWORD_TEXT, "text"}},
    {"style", {CHTL::TokenType::KEYWORD_STYLE, "style"}},
    {"script", {CHTL::TokenType::KEYWORD_SCRIPT, "script"}},
    {"inherit", {CHTL::TokenType::KEYWORD_INHERIT, "inherit"}},
    {"delete", {CHTL::TokenType::KEYWORD_DELETE, "delete"}},
    {"insert", {CHTL::TokenType::KEYWORD_INSERT, "insert"}},
    {"after", {CHTL::TokenType::KEYWORD_AFTER, "after"}},
    {"before", {CHTL::TokenType::KEYWORD_BEFORE, "before"}},
    {"replace", {CHTL::TokenType::KEYWORD_REPLACE, "replace"}},
    {"at", {CHTL::TokenType::KEYWORD_AT, "at"}},
    {"top", {CHTL::TokenType::KEYWORD_TOP, "top"}},
    {"bottom", {CHTL::TokenType::KEYWORD_BOTTOM, "bottom"}},
    {"from", {CHTL::TokenType::KEYWORD_FROM, "from"}},
    {"as", {CHTL::TokenType::KEYWORD_AS, "as"}},
    {"except", {CHTL::TokenType::KEYWORD_EXCEPT, "except"}},
    {"use", {CHTL::TokenType::KEYWORD_USE, "use"}},
    {"html5", {CHTL::TokenType::KEYWORD_HTML5, "html5"}},
    {"if", {CHTL::TokenType::KEYWORD_IF, "if"}},
    {"else", {CHTL::TokenType::KEYWORD_ELSE, "else"}},
    {"[Template]", {CHTL::TokenType::KEYWORD_TEMPLATE, "[Template]"}},
    {"[Custom]", {CHTL::TokenType::KEYWORD_CUSTOM, "[Custom]"}},
    {"[Import]", {CHTL::TokenType::KEYWORD_IMPORT, "[Import]"}},
    {"[Namespace]", {CHTL::TokenType::KEYWORD_NAMESPACE, "[Namespace]"}},
    {"[Origin]", {CHTL::TokenType::KEYWORD_ORIGIN, "[Origin]"}},
    {"[Configuration]", {CHTL::TokenType::KEYWORD_CONFIGURATION, "[Configuration]"}},
    {"[Name]", {CHTL::TokenType::KEYWORD_NAME, "[Name]"}},
};

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
        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
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
        CHTL::Lexer l(input, s_default_keywords);
        CHTL::Parser p(l, s_default_keywords);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = R"(<div style="width:100px;"></div>)";
        REQUIRE(html_output == expected_html);
    }
}
