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

TEST_CASE("Generator correctly handles automatic class naming", "[generator]")
{
    std::string input = R"(
        div {
            style {
                .box {
                    width: 300px;
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
    std::string expected_html = "<head><style>.box{width:300px;}</style></head><div class=\"box\"></div>";
    REQUIRE(html_output == expected_html);
}

TEST_CASE("Generator correctly handles contextual '&' selector", "[generator]")
{
    std::string input = R"(
        div {
            style {
                .box { color: blue; }
                &:hover { color: red; }
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    std::string expected_html = "<head><style>.box{color:blue;}.box:hover{color:red;}</style></head><div class=\"box\"></div>";
    REQUIRE(html_output == expected_html);
}

TEST_CASE("Generator correctly handles conditional expressions", "[generator]")
{
    std::string input = R"(
        div {
            style {
                width: 100px;
                background-color: width > 50px ? "red" : "blue";
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    std::string expected_html = R"(<div style="width:100px;background-color:red;"></div>)";
    REQUIRE(html_output == expected_html);
}

TEST_CASE("Generator correctly handles style group templates", "[generator]")
{
    std::string input = R"(
        [Template] @Style DefaultText
        {
            color: "black";
            line-height: 1.6;
        }

        div
        {
            style
            {
                @Style DefaultText;
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    std::string expected_html = R"(<div style="color:black;line-height:1.6;"></div>)";
    REQUIRE(html_output == expected_html);
}

TEST_CASE("Generator correctly handles variable group templates", "[generator]")
{
    std::string input =
        "[Template] @Var ThemeColor\n"
        "{\n"
        "    tableColor: \"rgb(255, 192, 203)\";\n"
        "}\n"
        "\n"
        "div\n"
        "{\n"
        "    style\n"
        "    {\n"
        "        background-color: ThemeColor(tableColor);\n"
        "    }\n"
        "}\n";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    std::string expected_html = "<div style=\"background-color:rgb(255, 192, 203);\"></div>";
    REQUIRE(html_output == expected_html);
}

TEST_CASE("Generator correctly handles element templates", "[generator]")
{
    std::string input = R"(
        [Template] @Element Box
        {
            div { style { background-color: red; } }
            span { text { "A box" } }
        }

        body
        {
            @Element Box;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    std::string expected_html = R"(<body><div style="background-color:red;"></div><span>A box</span></body>)";
    REQUIRE(html_output == expected_html);
}
