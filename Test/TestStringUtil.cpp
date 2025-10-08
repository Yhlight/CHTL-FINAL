#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "StringUtil/StringUtil.h"

TEST_CASE("StringUtil::trim", "[stringutil]") {
    REQUIRE(CHTL::StringUtil::trim("  hello  ") == "hello");
    REQUIRE(CHTL::StringUtil::trim("hello  ") == "hello");
    REQUIRE(CHTL::StringUtil::trim("  hello") == "hello");
    REQUIRE(CHTL::StringUtil::trim("hello") == "hello");
    REQUIRE(CHTL::StringUtil::trim("\t\n hello \n\t") == "hello");
    REQUIRE(CHTL::StringUtil::trim("  ") == "");
    REQUIRE(CHTL::StringUtil::trim("") == "");
}

TEST_CASE("StringUtil::split", "[stringutil]") {
    std::vector<std::string> expected = {"a", "b", "c"};
    REQUIRE(CHTL::StringUtil::split("a,b,c", ',') == expected);

    std::vector<std::string> expected2 = {"a"};
    REQUIRE(CHTL::StringUtil::split("a", ',') == expected2);

    std::vector<std::string> expected3 = {"a", ""};
    REQUIRE(CHTL::StringUtil::split("a,", ',') == expected3);

    std::vector<std::string> expected4 = {"", "a"};
    REQUIRE(CHTL::StringUtil::split(",a", ',') == expected4);

    std::vector<std::string> expected5 = {};
    REQUIRE(CHTL::StringUtil::split("", ',') == expected5);
}

TEST_CASE("StringUtil::startsWith", "[stringutil]") {
    REQUIRE(CHTL::StringUtil::startsWith("hello world", "hello") == true);
    REQUIRE(CHTL::StringUtil::startsWith("hello world", "world") == false);
    REQUIRE(CHTL::StringUtil::startsWith("hello", "hello world") == false);
    REQUIRE(CHTL::StringUtil::startsWith("hello", "hello") == true);
    REQUIRE(CHTL::StringUtil::startsWith("", "a") == false);
    REQUIRE(CHTL::StringUtil::startsWith("a", "") == true);
}

TEST_CASE("StringUtil::endsWith", "[stringutil]") {
    REQUIRE(CHTL::StringUtil::endsWith("hello world", "world") == true);
    REQUIRE(CHTL::StringUtil::endsWith("hello world", "hello") == false);
    REQUIRE(CHTL::StringUtil::endsWith("world", "hello world") == false);
    REQUIRE(CHTL::StringUtil::endsWith("world", "world") == true);
    REQUIRE(CHTL::StringUtil::endsWith("", "a") == false);
    REQUIRE(CHTL::StringUtil::endsWith("a", "") == true);
}