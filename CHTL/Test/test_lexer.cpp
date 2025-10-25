#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.hpp"

TEST_CASE(LexerCanTokenizeComment, "Lexer can tokenize a simple comment") {
    std::string input = "// This is a comment";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::COMMENT);
    REQUIRE(tokens[0].value == " This is a comment");
}
