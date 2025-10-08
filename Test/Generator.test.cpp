#include "catch.hpp"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/RootNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"

TEST_CASE("Generator produces correct HTML", "[generator]") {
    SECTION("Generate a simple element") {
        auto root = std::make_unique<RootNode>();
        root->children.push_back(std::make_unique<ElementNode>("div"));

        Generator generator;
        std::string result = generator.generate(*root);

        REQUIRE(result == "<div></div>\n");
    }

    SECTION("Generate an element with text") {
        auto root = std::make_unique<RootNode>();
        auto element = std::make_unique<ElementNode>("p");
        element->children.push_back(std::make_unique<TextNode>("hello"));
        root->children.push_back(std::move(element));

        Generator generator;
        std::string result = generator.generate(*root);

        REQUIRE(result == "<p>\n  hello\n</p>\n");
    }

    SECTION("Generate nested elements") {
        auto root = std::make_unique<RootNode>();
        auto html = std::make_unique<ElementNode>("html");
        auto body = std::make_unique<ElementNode>("body");
        body->children.push_back(std::make_unique<ElementNode>("div"));
        html->children.push_back(std::move(body));
        root->children.push_back(std::move(html));

        Generator generator;
        std::string result = generator.generate(*root);

        REQUIRE(result == "<html>\n  <body>\n    <div></div>\n  </body>\n</html>\n");
    }

    SECTION("Generate comments") {
        auto root = std::make_unique<RootNode>();
        root->children.push_back(std::make_unique<CommentNode>("# generator comment", TokenType::GENERATOR_COMMENT));
        root->children.push_back(std::make_unique<CommentNode>("// ignored comment", TokenType::SINGLE_LINE_COMMENT));

        Generator generator;
        std::string result = generator.generate(*root);

        // Note the space added after '#'. The content is "# generator comment".
        REQUIRE(result == "<!-- generator comment -->\n");
    }
}