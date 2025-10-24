#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <cassert>

void test_simple_tokens() {
    std::string source = "div { text: \"Hello\"; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    assert(tokens.size() == 8);
    assert(tokens[0].type == TokenType::Identifier);
    assert(tokens[0].value == "div");
    assert(tokens[1].type == TokenType::OpenBrace);
    assert(tokens[2].type == TokenType::Identifier);
    assert(tokens[2].value == "text");
    assert(tokens[3].type == TokenType::Colon);
    assert(tokens[4].type == TokenType::String);
    assert(tokens[4].value == "Hello");
    assert(tokens[5].type == TokenType::Semicolon);
    assert(tokens[6].type == TokenType::CloseBrace);

    std::cout << "test_simple_tokens passed." << std::endl;
}

int main() {
    test_simple_tokens();
    return 0;
}
