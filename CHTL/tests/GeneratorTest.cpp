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


TEST_CASE("Generator correctly generates HTML from AST", "[generator]")
{
    SECTION("Generates HTML for a complex element")
    {
        std::string input = R"(
            div {
                id: box;
                class = "container";
                style {
                    width: 100px;
                    color: red;
                }
                text { "Hello CHTL" }
                p {
                    text { "A paragraph." }
                }
            }
        )";

        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());

        std::string expected_html =
            "<div id=\"box\" class=\"container\" style=\"width:100px;color:red;\">"
            "Hello CHTL"
            "<p>A paragraph.</p>"
            "</div>";

        REQUIRE(html_output == expected_html);
    }
}
