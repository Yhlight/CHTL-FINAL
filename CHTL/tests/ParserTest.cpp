#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "nodes/ProgramNode.h"
// ... other includes ...

TEST_CASE("Test parsing a simple ElementNode", "[Parser]")
{
    std::string source = "div { text: \"hello\"; }";
    CHTL::Lexer lexer(source, CHTL::GetDefaultKeywords());
    CHTL::Parser parser(lexer, CHTL::GetDefaultKeywords(), "");
    auto program = parser.ParseProgram();
    REQUIRE(parser.GetErrors().empty());
    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);
}

// NOTE: The following tests are temporarily disabled because the responsibility
// for parsing [Configuration] and [Use] statements has been moved from the Parser
// to the ConfigScanner as part of a two-phase compilation process.
/*
... (commented out tests) ...
*/
