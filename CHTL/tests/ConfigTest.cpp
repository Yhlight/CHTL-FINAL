#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "generator/Generator.h"
#include "AstNode.h"
#include "lexer/Lexer.h"
#include <vector>
#include <string>

// Helper function to check for parser errors
void checkParserErrors(const CHTL::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }

    std::string error_message = "Parser has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("Test default configuration is loaded", "[config]")
{
    std::string input = "div {}";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    REQUIRE(program->config != nullptr);
    REQUIRE(program->config->IsDebugMode() == false);
    REQUIRE(program->config->GetIndexInitialCount() == 0);
}

TEST_CASE("Test unnamed [Configuration] block overrides defaults", "[config]")
{
    std::string input = R"(
        [Configuration] {
            DEBUG_MODE = true;
            INDEX_INITIAL_COUNT = 1;
        }
        div {}
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    REQUIRE(program->config != nullptr);
    REQUIRE(program->config->IsDebugMode() == true);
    REQUIRE(program->config->GetIndexInitialCount() == 1);
}

TEST_CASE("Test generator uses the program's configuration", "[config][generator]")
{
    std::string input = R"(
        [Configuration] {
            DEBUG_MODE = true;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    CHTL::Generator g(nullptr);
    std::string output = g.Generate(program.get());

    REQUIRE(output.find("<!-- CHTL DEBUG MODE ENABLED -->") != std::string::npos);
}

TEST_CASE("Test named configurations are stored", "[config]")
{
    std::string input = R"(
        [Configuration] @Config MyConfig {
            DEBUG_MODE = true;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    REQUIRE(program->named_configs.size() == 1);
    REQUIRE(program->named_configs.count("MyConfig") == 1);
    REQUIRE(program->named_configs.at("MyConfig")->IsDebugMode() == true);

    // Default config should not be affected
    REQUIRE(program->config->IsDebugMode() == false);
}

TEST_CASE("Test 'use @Config' statement applies the named configuration", "[config][use]")
{
    std::string input = R"(
        [Configuration] @Config MyConfig {
            DEBUG_MODE = true;
        }
        use @Config MyConfig;
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);

    REQUIRE(program->config != nullptr);
    REQUIRE(program->config->IsDebugMode() == true);
}

TEST_CASE("Test 'use' statement with a non-existent configuration", "[config][use]")
{
    std::string input = "use @Config NonExistentConfig;";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    p.ParseProgram(); // Don't check errors, we expect one

    const auto& errors = p.GetErrors();
    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].find("Configuration with name 'NonExistentConfig' not found.") != std::string::npos);
}
