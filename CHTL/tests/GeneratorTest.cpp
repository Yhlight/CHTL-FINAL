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

    std::string style_content = html_output.substr(html_output.find("style=\"") + 7);
    style_content = style_content.substr(0, style_content.find("\""));

    REQUIRE(style_content.find("width:100px") != std::string::npos);
    REQUIRE(style_content.find("background-color:red") != std::string::npos);
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

    std::string style_content = html_output.substr(html_output.find("style=\"") + 7);
    style_content = style_content.substr(0, style_content.find("\""));

    REQUIRE(style_content.find("color:black") != std::string::npos);
    REQUIRE(style_content.find("line-height:1.6") != std::string::npos);
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

TEST_CASE("Generator correctly handles custom style definitions with specialization", "[generator]")
{
    std::string input = R"(
        [Custom] @Style BaseBox
        {
            width: 100px;
            height: 200px;
            background-color: "blue";
            border: "1px solid black";
        }

        div
        {
            style
            {
                @Style BaseBox {
                    delete border;
                    delete height;
                }
                background-color: "red"; // Override
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());
    // Note: The order of properties is not guaranteed due to unordered_map lookups.
    // A robust test would parse the style attribute and check for properties.
    // For now, we rely on a stable (though not guaranteed) order.
    std::string expected_html = R"(<div style="width:100px;background-color:blue;background-color:red;"></div>)";
    // Let's create a more robust check
    std::string style_content = html_output.substr(html_output.find("style=\"") + 7);
    style_content = style_content.substr(0, style_content.find("\""));

    REQUIRE(style_content.find("width:100px") != std::string::npos);
    REQUIRE(style_content.find("height:200px") == std::string::npos);
    REQUIRE(style_content.find("border:1px solid black") == std::string::npos);
    // Later properties should override earlier ones.
    REQUIRE(style_content.find("background-color:blue") == std::string::npos);
    REQUIRE(style_content.find("background-color:red") != std::string::npos);
}

TEST_CASE("Generator correctly handles imported templates", "[generator]")
{
    std::string input = R"(
        [Import] @Chtl from "../tests/resources/imported_template.chtl";

        div {
            style {
                @Style ImportedStyle;
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string html_output = generator.Generate(program.get());

    std::string style_content = html_output.substr(html_output.find("style=\"") + 7);
    style_content = style_content.substr(0, style_content.find("\""));

    REQUIRE(style_content.find("font-size:16px") != std::string::npos);
    REQUIRE(style_content.find("color:imported-green") != std::string::npos);
}

TEST_CASE("Generator correctly handles namespaced template usage", "[generator]")
{
    SECTION("Template defined and used in the same namespace")
    {
        std::string input = R"(
            [Namespace] MySpace {
                [Template] @Style MyTemplate {
                    color: "purple";
                }

                div {
                    style {
                        @Style MyTemplate;
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
        REQUIRE(html_output.find("color:purple") != std::string::npos);
    }

    SECTION("Template in global namespace is accessible from within another namespace")
    {
        std::string input = R"(
            [Template] @Style GlobalTemplate {
                font-weight: bold;
            }

            [Namespace] MySpace {
                div {
                    style {
                        @Style GlobalTemplate;
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
        REQUIRE(html_output.find("font-weight:bold") != std::string::npos);
    }
}

TEST_CASE("Generator correctly renders comments", "[generator]")
{
    std::string input = R"(# My Comment)";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator generator;
    std::string output = generator.Generate(program.get());
    REQUIRE(output.find("<!-- My Comment -->") != std::string::npos);
}

TEST_CASE("Test generating a simple text node", "[generator]")
{
    auto text_node = std::make_unique<CHTL::TextNode>();
    text_node->value = "Just some simple text.";

    auto program = std::make_unique<CHTL::ProgramNode>();
    program->children.push_back(std::move(text_node));

    CHTL::Generator generator;
    std::string output = generator.Generate(program.get());

    REQUIRE(output == "Just some simple text.");
}

TEST_CASE("Test generating text inside an element", "[generator]")
{
    auto element_node = std::make_unique<CHTL::ElementNode>();
    element_node->tag_name = "p";

    auto text_node = std::make_unique<CHTL::TextNode>();
    text_node->value = "Hello from inside a paragraph.";
    element_node->children.push_back(std::move(text_node));

    auto program = std::make_unique<CHTL::ProgramNode>();
    program->children.push_back(std::move(element_node));

    CHTL::Generator generator;
    std::string output = generator.Generate(program.get());

    REQUIRE(output.find("<p>") != std::string::npos);
    REQUIRE(output.find("</p>") != std::string::npos);
    REQUIRE(output.find("Hello from inside a paragraph.") != std::string::npos);
}

TEST_CASE("Generator correctly handles chained and optional-else conditional expressions", "[generator]")
{
    // Common expression list for all sections
    const std::string expr_list = "background-color: width > 200px ? \"black\", width > 150px ? \"grey\", width > 100px ? \"white\" : \"blue\";";

    SECTION("Selects first matching optional-else expression")
    {
        std::string input = "div { style { width: 175px; " + expr_list + " } }";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());

        REQUIRE(html_output.find("background-color:grey") != std::string::npos);
        REQUIRE(html_output.find("background-color:black") == std::string::npos);
        REQUIRE(html_output.find("background-color:white") == std::string::npos);
        REQUIRE(html_output.find("background-color:blue") == std::string::npos);
    }

    SECTION("Selects first matching expression with final else")
    {
        std::string input = "div { style { width: 125px; " + expr_list + " } }";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());

        REQUIRE(html_output.find("background-color:white") != std::string::npos);
        REQUIRE(html_output.find("background-color:black") == std::string::npos);
        REQUIRE(html_output.find("background-color:grey") == std::string::npos);
        REQUIRE(html_output.find("background-color:blue") == std::string::npos);
    }

    SECTION("Falls through to final else")
    {
        std::string input = "div { style { width: 50px; " + expr_list + " } }";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        CHTL::Generator generator;
        std::string html_output = generator.Generate(program.get());

        REQUIRE(html_output.find("background-color:blue") != std::string::npos);
        REQUIRE(html_output.find("background-color:black") == std::string::npos);
        REQUIRE(html_output.find("background-color:grey") == std::string::npos);
        REQUIRE(html_output.find("background-color:white") == std::string::npos);
    }
}
