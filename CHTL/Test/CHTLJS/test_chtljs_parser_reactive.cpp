#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSParser/CHTLJSParser.hpp"
#include "CHTLJSLexer/CHTLJSLexer.hpp"

TEST_CASE("CHTLJS Parser can parse a reactive value") {
    std::string input = "$boxClass$";
    CHTLJSLexer lexer(input);
    CHTLJSParser parser(lexer);
    std::unique_ptr<CHTLJSNode> root = parser.parse();
    REQUIRE(root != nullptr);
}
