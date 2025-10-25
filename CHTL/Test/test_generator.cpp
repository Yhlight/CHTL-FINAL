#include "catch.hpp"
#include "CHTLGenerator/CHTLGenerator.hpp"
#include "CHTLParser/CHTLParser.hpp"

TEST_CASE(GeneratorCanGenerateSimpleElement, "Generator can generate a simple element") {
    std::string input = "div {}";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();
    CHTLGenerator generator;
    std::string html = generator.generate(*root);

    REQUIRE(html == "<div></div>");
}

TEST_CASE(GeneratorCanGenerateNestedElements, "Generator can generate nested elements") {
    std::string input = "div { span {} }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();
    CHTLGenerator generator;
    std::string html = generator.generate(*root);

    REQUIRE(html == "<div><span></span></div>");
}

TEST_CASE(GeneratorCanGenerateTextNodes, "Generator can generate text nodes") {
    std::string input = "div { text { \"hello\" } }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();
    CHTLGenerator generator;
    std::string html = generator.generate(*root);

    REQUIRE(html == "<div>hello</div>");
}

TEST_CASE(GeneratorCanGenerateAttributes, "Generator can generate attributes") {
    std::string input = "div { id: \"box\"; }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();
    CHTLGenerator generator;
    std::string html = generator.generate(*root);

    REQUIRE(html == "<div id=\"box\"></div>");
}

TEST_CASE(GeneratorCanGenerateInlineStyles, "Generator can generate inline styles") {
    std::string input = "div { style { width: 100px; } }";
    CHTLLexer lexer(input);
    CHTLParser parser(lexer.tokenize());
    std::unique_ptr<AstNode> root = parser.parse();
    CHTLGenerator generator;
    std::string html = generator.generate(*root);

    REQUIRE(html == "<div style=\"width:100px;\"></div>");
}
