#include "catch.hpp"
#include "CHTLLexer/Lexer.h"
#include "CHTLLexer/Token.h"
#include <vector>

void checkTokens(const std::string& input, const std::vector<CHTL::TokenType>& expected_types) {
    CHTL::Lexer lexer(input);
    for (size_t i = 0; i < expected_types.size(); ++i) {
        CHTL::Token token = lexer.nextToken();
        REQUIRE(token.type == expected_types[i]);
    }
}

TEST_CASE("Lexer::Punctuation and Operators", "[lexer]") {
    const std::string input = R"(=:;(){},.+-*/%**<> >= <= == != && || ? # & $ @ -> &->)";
    const std::vector<CHTL::TokenType> expected = {
        CHTL::TokenType::EQUAL, CHTL::TokenType::COLON, CHTL::TokenType::SEMICOLON,
        CHTL::TokenType::L_PAREN, CHTL::TokenType::R_PAREN, CHTL::TokenType::L_BRACE, CHTL::TokenType::R_BRACE, CHTL::TokenType::COMMA,
        CHTL::TokenType::DOT, CHTL::TokenType::PLUS, CHTL::TokenType::MINUS, CHTL::TokenType::ASTERISK, CHTL::TokenType::SLASH, CHTL::TokenType::PERCENT, CHTL::TokenType::POWER,
        CHTL::TokenType::LESS_THAN, CHTL::TokenType::GREATER_THAN, CHTL::TokenType::GREATER_EQUAL, CHTL::TokenType::LESS_EQUAL,
        CHTL::TokenType::EQUAL_EQUAL, CHTL::TokenType::NOT_EQUAL, CHTL::TokenType::LOGICAL_AND, CHTL::TokenType::LOGICAL_OR,
        CHTL::TokenType::QUESTION_MARK, CHTL::TokenType::HASH, CHTL::TokenType::AMPERSAND, CHTL::TokenType::DOLLAR, CHTL::TokenType::AT_SIGN,
        CHTL::TokenType::ARROW, CHTL::TokenType::EVENT_BIND_OP,
        CHTL::TokenType::END_OF_FILE
    };
    checkTokens(input, expected);
}

TEST_CASE("Lexer::Identifiers and Literals", "[lexer]") {
    const std::string input = R"(
        my_var "a string" 'another string' 123 45.67
    )";
    const std::vector<CHTL::TokenType> expected = {
        CHTL::TokenType::IDENTIFIER,
        CHTL::TokenType::STRING_LITERAL,
        CHTL::TokenType::STRING_LITERAL,
        CHTL::TokenType::NUMERIC_LITERAL,
        CHTL::TokenType::NUMERIC_LITERAL,
        CHTL::TokenType::END_OF_FILE
    };
    checkTokens(input, expected);
}

TEST_CASE("Lexer::Keywords", "[lexer]") {
    const std::string input = R"(
        text style script inherit delete insert after before replace
        at top bottom from as except use html5
    )";
    const std::vector<CHTL::TokenType> expected = {
        CHTL::TokenType::KEYWORD_TEXT, CHTL::TokenType::KEYWORD_STYLE, CHTL::TokenType::KEYWORD_SCRIPT,
        CHTL::TokenType::KEYWORD_INHERIT, CHTL::TokenType::KEYWORD_DELETE, CHTL::TokenType::KEYWORD_INSERT,
        CHTL::TokenType::KEYWORD_AFTER, CHTL::TokenType::KEYWORD_BEFORE, CHTL::TokenType::KEYWORD_REPLACE,
        CHTL::TokenType::KEYWORD_AT, CHTL::TokenType::KEYWORD_TOP, CHTL::TokenType::KEYWORD_BOTTOM,
        CHTL::TokenType::KEYWORD_FROM, CHTL::TokenType::KEYWORD_AS, CHTL::TokenType::KEYWORD_EXCEPT,
        CHTL::TokenType::KEYWORD_USE, CHTL::TokenType::KEYWORD_HTML5,
        CHTL::TokenType::END_OF_FILE
    };
    checkTokens(input, expected);
}


TEST_CASE("Lexer::Bracketed Keywords", "[lexer]") {
    const std::string input = R"(
        [Custom] [Template] [Origin] [Import] [Namespace] [Configuration] [Info] [Export]
    )";
    const std::vector<CHTL::TokenType> expected = {
        CHTL::TokenType::KEYWORD_CUSTOM, CHTL::TokenType::KEYWORD_TEMPLATE, CHTL::TokenType::KEYWORD_ORIGIN,
        CHTL::TokenType::KEYWORD_IMPORT, CHTL::TokenType::KEYWORD_NAMESPACE, CHTL::TokenType::KEYWORD_CONFIGURATION,
        CHTL::TokenType::KEYWORD_INFO, CHTL::TokenType::KEYWORD_EXPORT,
        CHTL::TokenType::END_OF_FILE
    };
    checkTokens(input, expected);
}


TEST_CASE("Lexer::Comments", "[lexer]") {
    const std::string input = R"(
        // this is a single line comment
        div /* this is a
               multi-line comment */ span
    )";
    const std::vector<CHTL::TokenType> expected = {
        CHTL::TokenType::IDENTIFIER, // div
        CHTL::TokenType::IDENTIFIER, // span
        CHTL::TokenType::END_OF_FILE
    };
    checkTokens(input, expected);
}

TEST_CASE("Lexer::A simple CHTL block", "[lexer]") {
    const std::string input = R"(
        div {
            id: my_box;
            class: "box red";
            text {
                Hello World
            }
        }
    )";
    // Note: "Hello World" is not tokenized yet as an UNQUOTED_LITERAL
    // This will be handled by the parser, which is context-aware.
    // The lexer will see it as two identifiers.
    const std::vector<CHTL::TokenType> expected = {
        CHTL::TokenType::IDENTIFIER, CHTL::TokenType::L_BRACE,
        CHTL::TokenType::IDENTIFIER, CHTL::TokenType::COLON, CHTL::TokenType::IDENTIFIER, CHTL::TokenType::SEMICOLON,
        CHTL::TokenType::IDENTIFIER, CHTL::TokenType::COLON, CHTL::TokenType::STRING_LITERAL, CHTL::TokenType::SEMICOLON,
        CHTL::TokenType::KEYWORD_TEXT, CHTL::TokenType::L_BRACE,
        CHTL::TokenType::IDENTIFIER, CHTL::TokenType::IDENTIFIER,
        CHTL::TokenType::R_BRACE,
        CHTL::TokenType::R_BRACE,
        CHTL::TokenType::END_OF_FILE
    };
    checkTokens(input, expected);
}