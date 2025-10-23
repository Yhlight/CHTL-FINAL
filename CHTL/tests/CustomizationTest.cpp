#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "generator/Generator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "AstNode.h"
#include <memory>

using namespace CHTL;

// Helper to parse input and get the generated output
static std::string generateOutput(const std::string& input) {
    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();
    const auto& errors = p.GetErrors();
    if (!errors.empty()) {
        std::string error_msg = "Parser errors:\n";
        for (const auto& err : errors) {
            error_msg += err + "\n";
        }
        FAIL(error_msg);
    }
    Generator g(nullptr);
    return g.Generate(program.get());
}

TEST_CASE("Custom Specializations", "[generator][custom]") {

    SECTION("@Style Specializations") {
        std::string input = R"(
            [Custom] @Style MyTextStyle {
                font-size: 16px;
                color: "blue";
                font-weight; // Valueless
            }

            p {
                style {
                    @Style MyTextStyle {
                        delete font-size;
                        color: "red"; // Override
                        font-weight: bold; // Provide value
                        text-decoration: "underline"; // Add new
                    }
                }
            }
        )";

        std::string expected = R"(<p style="color:red;font-weight:bold;text-decoration:underline;"></p>)";
        std::string generated = generateOutput(input);
        REQUIRE(generated == expected);
    }

    SECTION("@Element Specializations - Insert") {
        std::string input = R"(
            [Custom] @Element MyContainer {
                div { id: "first"; }
                div { id: "second"; }
            }

            @Element MyContainer {
                insert at top { p { text: "header"; } }
                insert after div[0] { span {} }
                insert at bottom { p { text: "footer"; } }
            }
        )";

        std::string expected = R"(<p>header</p><div id="first"></div><span></span><div id="second"></div><p>footer</p>)";
        std::string generated = generateOutput(input);
        REQUIRE(generated == expected);
    }

    SECTION("@Element Specializations - Delete and Replace") {
        std::string input = R"(
            [Custom] @Element MyContainer {
                p { id: "a"; }
                p { id: "b"; }
                p { id: "c"; }
            }

            @Element MyContainer {
                delete p[1]; // delete "b"
                insert replace p[0] {
                    span { text:"replacement"; }
                }
            }
        )";

        std::string expected = R"(<span>replacement</span><p id="c"></p>)";
        std::string generated = generateOutput(input);
        REQUIRE(generated == expected);
    }
}
