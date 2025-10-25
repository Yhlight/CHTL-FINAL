#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp"

TEST_CASE("CHTLJS Lexer can tokenize an enhanced selector") {
    std::string input = "{{.box}}";
    CHTLJSLexer lexer(input);
    std::vector<CHTLJSToken> tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTLJSTokenType::ENHANCED_SELECTOR);
    REQUIRE(tokens[0].value == ".box");
}
