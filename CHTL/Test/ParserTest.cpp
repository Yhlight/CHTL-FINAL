#include <gtest/gtest.h>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLNode/ASTNode.h"

class TestVisitor : public CHTL::ASTVisitor {
public:
    void visit(const CHTL::TextNode& node) override {
        visitedTextNode = true;
        text = node.getText();
    }

    void visit(const CHTL::ElementNode& node) override {
        for (const auto& child : node.getChildren()) {
            child->accept(*this);
        }
    }

    bool visitedTextNode = false;
    std::string text;
};

TEST(ParserTest, HelloWorld) {
    std::string source = "text { \"Hello, World!\" }";
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();
    CHTL::Parser parser(tokens);
    std::unique_ptr<CHTL::ASTNode> ast = parser.parse();

    TestVisitor visitor;
    ast->accept(visitor);

    ASSERT_TRUE(visitor.visitedTextNode);
    EXPECT_EQ(visitor.text, "Hello, World!");
}
