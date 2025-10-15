#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Parser.h"
#include "Lexer.h"
#include "Generator.h"
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


TEST_CASE("Generator correctly generates HTML for basic elements", "[generator]")
{
    SECTION("Generates an empty element")
    {
        std::string input = "div {}";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = "<div></div>";
        REQUIRE(html_output == expected_html);
    }

    SECTION("Generates nested elements")
    {
        std::string input = "body { div {} }";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());
        std::string expected_html = "<body><div></div></body>";
        REQUIRE(html_output == expected_html);
    }
}

TEST_CASE("Generator correctly generates HTML for complex structures", "[generator]")
{
    std::string input = R"(
        div {
            id: "box";
            # this is a comment
            p {
                text { "hello" }
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    std::string expected_html = "<div id=\"box\"><!-- this is a comment --><p>hello</p></div>";
    REQUIRE(html_output == expected_html);
}
