#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp"

TEST_CASE("CHTLJS Lexer can tokenize the bind operator") {
    std::string input = "&->";
    CHTLJSLexer lexer(input);
    std::vector<CHTLJSToken> tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTLJSTokenType::BIND_OPERATOR);
    REQUIRE(tokens[0].value == "&->");
}
