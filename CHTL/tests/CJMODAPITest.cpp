#include "gtest/gtest.h"
#include "CJMODSystem/Syntax.h"
#include "CJMODSystem/Arg.h"
#include "CJMODSystem/CJMODScanner.h"
#include "CJMODSystem/CJMODGenerator.h"

#include <streambuf>
#include <sstream>

TEST(CJMODAPITest, AnalyzeBasic) {
    Arg args = Syntax::analyze("$ ** $");
    ASSERT_EQ(args.args.size(), 3);
    EXPECT_EQ(args.args[0].placeholder, "$");
    EXPECT_EQ(args.args[1].placeholder, "**");
    EXPECT_EQ(args.args[2].placeholder, "$");
}

TEST(CJMODAPITest, FullWorkflow) {
    Arg args = Syntax::analyze("$ ** $");
    args.bind("$", [](const std::string& value) { return value; });
    args.bind("**", [](const std::string& value) { return value; });

    std::string source_code = "console.log(3 ** 4);";
    Arg result = CJMODScanner::scan(args, "**", source_code);
    args.fillValue(result);

    ASSERT_EQ(args.args[0].value, "3");
    ASSERT_EQ(args.args[1].value, "**");
    ASSERT_EQ(args.args[2].value, "4");

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

    EXPECT_EQ(buffer.str(), "pow(3, 4)\n");
}
