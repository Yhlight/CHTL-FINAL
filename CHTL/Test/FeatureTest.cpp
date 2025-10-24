#include <gtest/gtest.h>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLNode/ASTNode.h"
#include "CHTL/CHTL/CHTLGenerator/HTMLGenerator.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> // For std::remove_if

// This macro is defined in CMakeLists.txt
#define CHTL_TEST_SRCDIR CHTL_SOURCE_DIR

std::string readFile(const std::string& path) {
    std::string full_path = std::string(CHTL_TEST_SRCDIR) + "/" + path;
    std::ifstream file(full_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + full_path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


TEST(FeatureTest, Lexer) {
    std::string source = readFile("features.chtl");
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // A simple smoke test to ensure tokenization is happening.
    ASSERT_GT(tokens.size(), 10);

    // Find the first non-comment token
    auto it = std::find_if(tokens.begin(), tokens.end(), [](const CHTL::Token& t) {
        return t.type != CHTL::TokenType::GeneratorComment;
    });

    ASSERT_NE(it, tokens.end());
    EXPECT_EQ(it->type, CHTL::TokenType::Identifier);
    EXPECT_EQ(it->lexeme, "div");
    it++;
    ASSERT_NE(it, tokens.end());
    EXPECT_EQ(it->type, CHTL::TokenType::OpenBrace);
}

TEST(FeatureTest, EndToEnd) {
    std::string source = readFile("features.chtl");
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();
    CHTL::Parser parser(tokens);
    auto ast = parser.parse();
    CHTL::HTMLGenerator generator;
    std::string html = generator.generate(*ast);

    // This is a "golden" test. We expect a certain output.
    std::string expected_html =
        "<div id=\"main-content\" class=\"container\">"
            "<h1 style=\"color:red;font-size:24px;\">"
                "Welcome to CHTL"
            "</h1>"
            "<p>"
                "This is a paragraph with some 'single-quoted' text."
            "</p>"
        "</div>";

    // Normalize both strings by removing whitespace
    auto normalize = [](const std::string& s) {
        std::string out;
        std::copy_if(s.begin(), s.end(), std::back_inserter(out), [](char c){ return !isspace(c); });
        return out;
    };

    std::string normalized_html = normalize(html);
    std::string normalized_expected = normalize(expected_html);

    std::cout << "Generated HTML: " << html << std::endl;
    std::cout << "Expected HTML: " << expected_html << std::endl;

    ASSERT_EQ(normalized_html, normalized_expected);
}
