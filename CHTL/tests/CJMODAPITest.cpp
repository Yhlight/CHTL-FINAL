#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CJMODSystem/Syntax.h"
#include "CJMODSystem/Arg.h"
#include "CJMODSystem/CJMODScanner.h"
#include "CJMODSystem/CJMODGenerator.h"

#include <streambuf>
#include <sstream>

TEST_CASE("CJMOD API", "[cjmod]")
{
    SECTION("AnalyzeBasic")
    {
        Arg args = Syntax::analyze("$ ** $");
        REQUIRE(args.args.size() == 3);
        REQUIRE(args.args[0].placeholder == "$");
        REQUIRE(args.args[0].type == AtomArgType::PLACEHOLDER);
        REQUIRE(args.args[1].placeholder == "**");
        REQUIRE(args.args[1].type == AtomArgType::LITERAL);
        REQUIRE(args.args[2].placeholder == "$");
        REQUIRE(args.args[2].type == AtomArgType::PLACEHOLDER);
    }

    SECTION("AnalyzePlaceholders")
    {
        Arg args = Syntax::analyze("$ $? $! $_ ...");
        REQUIRE(args.args.size() == 5);
        REQUIRE(args.args[0].type == AtomArgType::PLACEHOLDER);
        REQUIRE(args.args[1].type == AtomArgType::OPTIONAL);
        REQUIRE(args.args[2].type == AtomArgType::REQUIRED);
        REQUIRE(args.args[3].type == AtomArgType::UNORDERED);
        REQUIRE(args.args[4].type == AtomArgType::VARIADIC);
    }

    SECTION("FullWorkflow")
    {
        Arg args = Syntax::analyze("$ ** $");
        args.bind("$", [](const std::string& value) { return value; });
        args.bind("**", [](const std::string& value) { return value; });

        std::string source_code = "console.log(3 ** 4);";
        Arg result = CJMODScanner::scan(args, "**", source_code);
        args.fillValue(result);

        REQUIRE(args.args[0].value == "3");
        REQUIRE(args.args[1].value == "**");
        REQUIRE(args.args[2].value == "4");

        // Ensure order of evaluation by calling 'match' in separate statements.
        std::string first_arg = args.match("$", [](const std::string& value) { return value; });
        std::string second_arg = args.match("$", [](const std::string& value) { return value; });
        std::string transformed_string = "pow(" + first_arg + ", " + second_arg + ")";
        args.transform(transformed_string);

        // Redirect cout to a stringstream to capture the output
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        CJMODGenerator::exportResult(args);

        std::cout.rdbuf(old); // Restore cout

        REQUIRE(buffer.str() == "pow(3, 4)\n");
    }
}
