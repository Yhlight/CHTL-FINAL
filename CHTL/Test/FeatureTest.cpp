#include <gtest/gtest.h>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLNode/ASTNode.h"
#include "CHTL/CHTL/CHTLGenerator/HTMLGenerator.h"
#include "TestUtils.h"
#include <algorithm> // For std::find_if

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

    std::cout << "Generated HTML: " << html << std::endl;

    // The generator uses a map for attributes, so the order is not guaranteed.
    // We will check for the presence of the key components as substrings.
    ASSERT_NE(html.find("id=\"main-content\""), std::string::npos);
    ASSERT_NE(html.find("class=\"container\""), std::string::npos);
    ASSERT_NE(html.find("style=\"color:red;font-size:24px;\""), std::string::npos);

    // Check for h1 content, resilient to attributes
    ASSERT_NE(html.find("<h1"), std::string::npos);
    ASSERT_NE(html.find(">Welcome to CHTL</h1>"), std::string::npos);

    // Check for p content
    ASSERT_NE(html.find("<p>This is a paragraph with some 'single-quoted' text.</p>"), std::string::npos);
}
