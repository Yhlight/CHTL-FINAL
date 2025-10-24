#include <gtest/gtest.h>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLNode/ASTNode.h"
#include "CHTL/CHTL/CHTLGenerator/HTMLGenerator.h"
#include "TestUtils.h"

TEST(StylingTest, EndToEnd) {
    std::string source = readFile("styling.chtl");
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();
    CHTL::Parser parser(tokens);
    auto ast = parser.parse();
    CHTL::HTMLGenerator generator;
    std::string html = generator.generate(*ast);

    // Verify the generated class and id attributes
    std::string expected_body =
        "<div class=\"card\" id=\"main-card\" style=\"width:100%;\">"
            "<h1>Card Title</h1>"
        "</div>";

    // Verify the generated global CSS
    std::string expected_css =
        ".card{border:1pxsolid#ccc;border-radius:4px;}"
        "#main-card{box-shadow:02px4pxrgba(0,0,0,0.1);}";

    std::cout << "Generated HTML: " << html << std::endl;

    std::string normalized_html = normalize(html);

    // Check that the body contains the correct elements and attributes
    ASSERT_NE(normalized_html.find(normalize(expected_body)), std::string::npos);

    // Check that the generated CSS is present in the document
    ASSERT_NE(normalized_html.find(normalize(expected_css)), std::string::npos);
}
