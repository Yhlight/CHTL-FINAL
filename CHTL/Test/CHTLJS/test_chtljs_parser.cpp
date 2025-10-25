#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSParser/CHTLJSParser.hpp"
#include "CHTLJSLexer/CHTLJSLexer.hpp"

TEST_CASE("CHTLJS Parser can parse an enhanced selector") {
    std::string input = "{{.box}}";
    CHTLJSLexer lexer(input);
    CHTLJSParser parser(lexer);
    std::unique_ptr<CHTLJSNode> root = parser.parse();
    REQUIRE(root != nullptr);
}
