#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Parser.h"
#include "Lexer.h"

#include <memory>

TEST_CASE("Parser correctly parses a simple element", "[parser]")
{
    std::string input = "div {}";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);

    auto program = p.ParseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(p.GetErrors().empty());
    REQUIRE(program->statements.size() == 1);

    auto elementNode = dynamic_cast<CHTL::ElementNode*>(program->statements[0].get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->tagName == "div");
    REQUIRE(elementNode->children.empty());
}
