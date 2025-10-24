#include <gtest/gtest.h>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLNode/ASTNode.h"
#include "CHTL/CHTL/CHTLGenerator/HTMLGenerator.h"
#include "TestUtils.h"

TEST(ContextSelectorTest, EndToEnd) {
    std::string source = readFile("context-selector.chtl");
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();
    CHTL::Parser parser(tokens);
    auto ast = parser.parse();
    CHTL::HTMLGenerator generator;
    std::string html = generator.generate(*ast);

    // Verify the generated global CSS
    std::string expected_css =
        ".btn { padding: 10px 20px; background-color: blue; color: white; }"
        ".btn:hover { background-color: darkblue; }";

    std::cout << "Generated HTML: " << html << std::endl;

    std::string normalized_html = normalize(html);

    // Check that the generated CSS is present in the document
    ASSERT_NE(normalized_html.find(normalize(expected_css)), std::string::npos);
}
