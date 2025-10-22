#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTLJS/include/parser/Parser.h"
#include "CHTLJS/include/lexer/Lexer.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include <vector>
#include <string>

// Helper function to check for CHTL JS parser errors
void checkCHTLJSParserErrors(const CHTLJS::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }

    std::string error_message = "CHTLJS Parser has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("Test parsing a simple Enhanced Selector", "[chtljs_parser]")
{
    std::string input = "{{ .box }}";
    CHTLJS::Lexer l(input);
    CHTLJS::Parser p(l);
    auto program = p.ParseProgram();

    checkCHTLJSParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* selector_node = dynamic_cast<CHTLJS::EnhancedSelectorNode*>(program->children[0].get());
    REQUIRE(selector_node != nullptr);
    REQUIRE(selector_node->selector == ".box");
}

TEST_CASE("Test parsing an Enhanced Selector surrounded by JS code", "[chtljs_parser]")
{
    std::string input = "let element = {{ #main-id }};";
    CHTLJS::Lexer l(input);
    CHTLJS::Parser p(l);
    auto program = p.ParseProgram();

    checkCHTLJSParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 3);

    auto* raw_node1 = dynamic_cast<CHTLJS::RawJSNode*>(program->children[0].get());
    REQUIRE(raw_node1 != nullptr);
    REQUIRE(raw_node1->content == "let element = ");

    auto* selector_node = dynamic_cast<CHTLJS::EnhancedSelectorNode*>(program->children[1].get());
    REQUIRE(selector_node != nullptr);
    REQUIRE(selector_node->selector == "#main-id");

    auto* raw_node2 = dynamic_cast<CHTLJS::RawJSNode*>(program->children[2].get());
    REQUIRE(raw_node2 != nullptr);
    REQUIRE(raw_node2->content == ";");
}

TEST_CASE("Test parsing multiple Enhanced Selectors", "[chtljs_parser]")
{
    std::string input = "let el1 = {{ .a }}; let el2 = {{ .b }};";
    CHTLJS::Lexer l(input);
    CHTLJS::Parser p(l);
    auto program = p.ParseProgram();

    checkCHTLJSParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 5);

    auto* raw1 = dynamic_cast<CHTLJS::RawJSNode*>(program->children[0].get());
    REQUIRE(raw1 != nullptr);
    REQUIRE(raw1->content == "let el1 = ");

    auto* sel1 = dynamic_cast<CHTLJS::EnhancedSelectorNode*>(program->children[1].get());
    REQUIRE(sel1 != nullptr);
    REQUIRE(sel1->selector == ".a");

    auto* raw2 = dynamic_cast<CHTLJS::RawJSNode*>(program->children[2].get());
    REQUIRE(raw2 != nullptr);
    REQUIRE(raw2->content == "; let el2 = ");

    auto* sel2 = dynamic_cast<CHTLJS::EnhancedSelectorNode*>(program->children[3].get());
    REQUIRE(sel2 != nullptr);
    REQUIRE(sel2->selector == ".b");

    auto* raw3 = dynamic_cast<CHTLJS::RawJSNode*>(program->children[4].get());
    REQUIRE(raw3 != nullptr);
    REQUIRE(raw3->content == ";");
}
