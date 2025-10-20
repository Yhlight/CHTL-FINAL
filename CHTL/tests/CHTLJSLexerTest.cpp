#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTLJS/include/lexer/Lexer.h"
#include "CHTLJS/include/lexer/Token.h"
#include <vector>

// Helper function to run tests on a sequence of CHTL JS tokens
void runCHTLJSLexerTest(const std::string& input, const std::vector<CHTLJS::Token>& expected_tokens) {
    CHTLJS::Lexer l(input);
    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        CHTLJS::Token tok = l.NextToken();
        const auto& expected = expected_tokens[i];

        INFO("Test case " << i << ": " << "Expected: " << CHTLJS::TokenTypeToString(expected.type) << " ('" << expected.literal << "'), Got: " << CHTLJS::TokenTypeToString(tok.type) << " ('" << tok.literal << "')");

        REQUIRE(tok.type == expected.type);
        REQUIRE(tok.literal == expected.literal);
    }
}

TEST_CASE("Test CHTL JS Lexer with Enhanced Selector", "[chtljs_lexer]")
{
    std::string input = "{{ .box }}";
    std::vector<CHTLJS::Token> expected = {
        {CHTLJS::TokenType::LBRACE_BRACE, "{{"},
        {CHTLJS::TokenType::RAW_JS, " .box "},
        {CHTLJS::TokenType::RBRACE_BRACE, "}}"},
        {CHTLJS::TokenType::END_OF_FILE, ""},
    };
    runCHTLJSLexerTest(input, expected);
}

TEST_CASE("Test CHTL JS Lexer with surrounding JS code", "[chtljs_lexer]")
{
    std::string input = "let a = {{.box}};";
    std::vector<CHTLJS::Token> expected = {
        {CHTLJS::TokenType::RAW_JS, "let a = "},
        {CHTLJS::TokenType::LBRACE_BRACE, "{{"},
        {CHTLJS::TokenType::RAW_JS, ".box"},
        {CHTLJS::TokenType::RBRACE_BRACE, "}}"},
        {CHTLJS::TokenType::RAW_JS, ";"},
        {CHTLJS::TokenType::END_OF_FILE, ""},
    };
    runCHTLJSLexerTest(input, expected);
}
