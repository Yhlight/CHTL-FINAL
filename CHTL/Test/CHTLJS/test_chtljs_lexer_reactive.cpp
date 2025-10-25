#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp"

TEST_CASE("CHTLJS Lexer can tokenize a reactive value") {
    std::string input = "$boxClass$";
    CHTLJSLexer lexer(input);
    std::vector<CHTLJSToken> tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTLJSTokenType::REACTIVE_VALUE);
    REQUIRE(tokens[0].value == "boxClass");
}
