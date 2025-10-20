#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "config/ConfigParser.h"
#include "config/ConfigLexer.h"
#include <vector>

void checkParserErrors(const CHTL::ConfigParser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }
    std::string error_message = "ConfigParser has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("Test ConfigParser Revised", "[config_parser]")
{
    SECTION("Parse a group with one rule: [@Style]")
    {
        std::string input = "[@Style]";
        CHTL::ConfigLexer l(input);
        CHTL::ConfigParser p(l);
        auto group = p.ParseRuleGroup();
        checkParserErrors(p);

        REQUIRE(group.size() == 1);
        const auto& rule1 = group[0];
        REQUIRE(rule1.size() == 2);
        REQUIRE(rule1[0].type == CHTL::TokenType::AT);
        REQUIRE(rule1[0].literal == "@");
        REQUIRE(rule1[1].type == CHTL::TokenType::IDENT);
        REQUIRE(rule1[1].literal == "Style");
    }

    SECTION("Parse a group with multiple rules: [@Style, @style]")
    {
        std::string input = "[@Style, @style]";
        CHTL::ConfigLexer l(input);
        CHTL::ConfigParser p(l);
        auto group = p.ParseRuleGroup();
        checkParserErrors(p);

        REQUIRE(group.size() == 2);

        const auto& rule1 = group[0];
        REQUIRE(rule1.size() == 2);
        REQUIRE(rule1[0].type == CHTL::TokenType::AT);
        REQUIRE(rule1[1].literal == "Style");

        const auto& rule2 = group[1];
        REQUIRE(rule2.size() == 2);
        REQUIRE(rule2[0].type == CHTL::TokenType::AT);
        REQUIRE(rule2[1].literal == "style");
    }

    SECTION("Parse a single composite rule: [Template]")
    {
        std::string input = "[Template]";
        CHTL::ConfigLexer l(input);
        CHTL::ConfigParser p(l);
        auto group = p.ParseRuleGroup();
        checkParserErrors(p);

        REQUIRE(group.size() == 1);
        const auto& rule1 = group[0];
        REQUIRE(rule1.size() == 1);
        REQUIRE(rule1[0].type == CHTL::TokenType::IDENT);
        REQUIRE(rule1[0].literal == "Template");
    }

    SECTION("Parse a single non-group rule: inherit")
    {
        std::string input = "inherit";
        CHTL::ConfigLexer l(input);
        CHTL::ConfigParser p(l);
        auto group = p.ParseRuleGroup();
        checkParserErrors(p);

        REQUIRE(group.size() == 1);
        const auto& rule1 = group[0];
        REQUIRE(rule1.size() == 1);
        REQUIRE(rule1[0].type == CHTL::TokenType::IDENT);
        REQUIRE(rule1[0].literal == "inherit");
    }

    SECTION("Handle errors gracefully")
    {
        std::string input = "[@Style, @style"; // Missing closing bracket
        CHTL::ConfigLexer l(input);
        CHTL::ConfigParser p(l);
        p.ParseRuleGroup();
        REQUIRE(p.GetErrors().size() == 1);
        REQUIRE(p.GetErrors()[0].find("Expected ']'") != std::string::npos);
    }
}
