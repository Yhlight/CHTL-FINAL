#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "compiler/Compiler.h"
#include "config/ConfigurationManager.h"
#include "config/ConfigScanner.h"
#include "parser/Parser.h"
#include "AstNode.h"
#include <fstream>
#include <cstdio>

// Helper to check for compiler errors
static void checkCompilerErrors(const CHTL::Compiler& c) {
    const auto& errors = c.GetErrors();
    if (errors.empty()) {
        return;
    }
    std::string error_message = "Compiler has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("End-to-end test of the two-phase compilation process", "[integration][compiler]")
{
    // 1. Prepare the test source file content
    std::string source_content = R"(
        [Configuration] {
            [Name: spanish]
            style = "estilo";
            text = "texto";
        }

        // --- Main CHTL Code ---

        // 'use spanish;' would normally be here. We simulate it.

        div {
            estilo {
                color: "red";
            }
        }

        p {
            texto {
                "Hello in Spanish"
            }
        }
    )";

    // 2. Write to a temporary file
    const std::string test_filename = "integration_test.chtl";
    std::ofstream test_file(test_filename);
    test_file << source_content;
    test_file.close();

    // 3. Instantiate the Compiler
    CHTL::Compiler compiler;
    compiler.Reset(); // Ensure a clean state

    // 4. First Pass (simulation): Scan config and set active config
    // In a real scenario, the compiler would also parse 'use' statements here.
    CHTL::ConfigScanner scanner(source_content);
    scanner.Scan();
    CHTL::ConfigurationManager::GetInstance().SetActiveConfiguration("spanish");

    // 5. Second Pass: Compile the source with the active configuration
    auto program = compiler.Compile(test_filename);
    checkCompilerErrors(compiler);

    // 6. Validate the AST
    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() > 1);

    // Check the div -> estilo block
    auto* div_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(div_node != nullptr);
    REQUIRE(div_node->children.size() == 1);
    auto* style_node = dynamic_cast<CHTL::StyleNode*>(div_node->children[0].get());
    REQUIRE(style_node != nullptr); // Should be a StyleNode now
    REQUIRE(style_node->children.size() == 1);
    auto* style_prop = dynamic_cast<CHTL::StyleProperty*>(style_node->children[0].get());
    REQUIRE(style_prop->name == "color");

    // Check the p -> texto block
    auto* p_node = dynamic_cast<CHTL::ElementNode*>(program->children[1].get());
    REQUIRE(p_node != nullptr);
    REQUIRE(p_node->children.size() == 1);
    auto* text_node = dynamic_cast<CHTL::TextNode*>(p_node->children[0].get());
    REQUIRE(text_node != nullptr); // Should be a TextNode now
    REQUIRE(text_node->value == "Hello in Spanish");


    // 7. Clean up
    std::remove(test_filename.c_str());
}
