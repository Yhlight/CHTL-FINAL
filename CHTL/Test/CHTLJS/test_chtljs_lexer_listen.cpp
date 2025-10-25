#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp"

TEST_CASE("CHTLJS Lexer can tokenize the Listen keyword") {
    std::string input = "Listen";
    CHTLJSLexer lexer(input);
    std::vector<CHTLJSToken> tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTLJSTokenType::KEYWORD_LISTEN);
    REQUIRE(tokens[0].value == "Listen");
}
