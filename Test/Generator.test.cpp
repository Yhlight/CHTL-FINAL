#include "catch.hpp"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/RootNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/StylePropertyNode.h"
#include "CHTL/CHTLNode/NumericLiteralExprNode.h"
#include "CHTL/CHTLNode/IdentifierExprNode.h"
#include "CHTL/CHTLNode/BinaryOpExprNode.h"
#include "CHTL/CHTLNode/TemplateDeclarationNode.h"
#include "CHTL/CHTLNode/TemplateUsageNode.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to make tests insensitive to whitespace changes
std::string normalize_whitespace(const std::string& s) {
    std::string result;
    result.reserve(s.length());
    for (char c : s) {
        if (!std::isspace(c)) {
            result += c;
        }
    }
    return result;
}

TEST_CASE("Generator produces correct HTML", "[generator]") {
    CHTLContext context;

    SECTION("Generate a simple element") {
        auto root = std::make_unique<RootNode>();
        root->children.push_back(std::make_unique<ElementNode>("div"));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<div></div>"));
    }

    SECTION("Generate an element with text") {
        auto root = std::make_unique<RootNode>();
        auto element = std::make_unique<ElementNode>("p");
        element->children.push_back(std::make_unique<TextNode>("hello"));
        root->children.push_back(std::move(element));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<p>hello</p>"));
    }

    SECTION("Generate nested elements") {
        auto root = std::make_unique<RootNode>();
        auto html = std::make_unique<ElementNode>("html");
        auto body = std::make_unique<ElementNode>("body");
        body->children.push_back(std::make_unique<ElementNode>("div"));
        html->children.push_back(std::move(body));
        root->children.push_back(std::move(html));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<html><body><div></div></body></html>"));
    }

    SECTION("Generate comments") {
        auto root = std::make_unique<RootNode>();
        root->children.push_back(std::make_unique<CommentNode>("# generator comment", TokenType::GENERATOR_COMMENT));
        root->children.push_back(std::make_unique<CommentNode>("// ignored comment", TokenType::SINGLE_LINE_COMMENT));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<!-- generator comment -->"));
    }

    SECTION("Generate an element with attributes") {
        auto root = std::make_unique<RootNode>();
        auto element = std::make_unique<ElementNode>("div");
        element->attributes["id"] = "main";
        element->attributes["class"] = "container";
        root->children.push_back(std::move(element));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<div class=\"container\" id=\"main\"></div>"));
    }

    SECTION("Generate an element with inline styles") {
        auto root = std::make_unique<RootNode>();
        auto element = std::make_unique<ElementNode>("div");
        auto styleNode = std::make_unique<StyleNode>();
        styleNode->properties.push_back(std::make_unique<StylePropertyNode>("color", std::make_unique<IdentifierExprNode>("red")));
        styleNode->properties.push_back(std::make_unique<StylePropertyNode>("font-size", std::make_unique<IdentifierExprNode>("16px")));
        element->style = std::move(styleNode);
        root->children.push_back(std::move(element));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<div style=\"color: red; font-size: 16px;\"></div>"));
    }

    SECTION("Generate a global stylesheet from style rules") {
        auto root = std::make_unique<RootNode>();
        auto html = std::make_unique<ElementNode>("html");
        html->children.push_back(std::make_unique<ElementNode>("head"));
        auto body = std::make_unique<ElementNode>("body");
        auto div = std::make_unique<ElementNode>("div");
        auto styleNode = std::make_unique<StyleNode>();
        auto rule = std::make_unique<StyleRuleNode>(".box");
        rule->properties.push_back(std::make_unique<StylePropertyNode>("color", std::make_unique<IdentifierExprNode>("red")));
        styleNode->rules.push_back(std::move(rule));
        div->style = std::move(styleNode);
        body->children.push_back(std::move(div));
        html->children.push_back(std::move(body));
        root->children.push_back(std::move(html));
        Generator generator;
        std::string result = generator.generate(*root, context);
        std::string expected = "<html><head><style>.box { color: red; }</style></head><body><div class=\"box\"></div></body></html>";
        REQUIRE(normalize_whitespace(result) == normalize_whitespace(expected));
    }

    SECTION("Automatically generate class and id attributes from style rules") {
        auto root = std::make_unique<RootNode>();
        auto element = std::make_unique<ElementNode>("div");
        element->attributes["class"] = "existing-class";
        auto styleNode = std::make_unique<StyleNode>();
        auto classRule = std::make_unique<StyleRuleNode>(".box");
        styleNode->rules.push_back(std::move(classRule));
        auto idRule = std::make_unique<StyleRuleNode>("#main-content");
        styleNode->rules.push_back(std::move(idRule));
        element->style = std::move(styleNode);
        root->children.push_back(std::move(element));
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result).find(normalize_whitespace("<div class=\"box existing-class\" id=\"main-content\">")) != std::string::npos);
    }

    SECTION("Resolve the contextual selector '&' in style rules") {
        auto root = std::make_unique<RootNode>();
        auto html = std::make_unique<ElementNode>("html");
        html->children.push_back(std::make_unique<ElementNode>("head"));
        auto body = std::make_unique<ElementNode>("body");
        auto button = std::make_unique<ElementNode>("button");
        auto styleNode = std::make_unique<StyleNode>();
        auto mainRule = std::make_unique<StyleRuleNode>(".btn");
        styleNode->rules.push_back(std::move(mainRule));
        auto hoverRule = std::make_unique<StyleRuleNode>("&:hover");
        hoverRule->properties.push_back(std::make_unique<StylePropertyNode>("color", std::make_unique<IdentifierExprNode>("red")));
        styleNode->rules.push_back(std::move(hoverRule));
        button->style = std::move(styleNode);
        body->children.push_back(std::move(button));
        html->children.push_back(std::move(body));
        root->children.push_back(std::move(html));
        Generator generator;
        std::string result = generator.generate(*root, context);
        std::string expected_style = ".btn{} .btn:hover{color:red;}";
        REQUIRE(normalize_whitespace(result).find(normalize_whitespace(expected_style)) != std::string::npos);
        REQUIRE(normalize_whitespace(result).find(normalize_whitespace("<button class=\"btn\">")) != std::string::npos);
    }

    SECTION("Expand a style template") {
        auto tpl = std::make_unique<TemplateDeclarationNode>(TemplateType::STYLE, "DefaultText");
        tpl->body.push_back(std::make_unique<StylePropertyNode>("color", std::make_unique<IdentifierExprNode>("blue")));
        tpl->body.push_back(std::make_unique<StylePropertyNode>("font-size", std::make_unique<NumericLiteralExprNode>(12, "pt")));
        context.styleTemplates["DefaultText"] = std::move(tpl);

        auto root = std::make_unique<RootNode>();
        auto div = std::make_unique<ElementNode>("div");
        auto styleNode = std::make_unique<StyleNode>();
        styleNode->templateUsages.push_back(std::make_unique<TemplateUsageNode>(TemplateType::STYLE, "DefaultText"));
        div->style = std::move(styleNode);
        root->children.push_back(std::move(div));

        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<div style=\"color: blue; font-size: 12pt;\"></div>"));
    }

    SECTION("Expand an element template") {
        // 1. Define the template in the context
        auto tpl = std::make_unique<TemplateDeclarationNode>(TemplateType::ELEMENT, "Box");
        auto tplDiv = std::make_unique<ElementNode>("div");
        tplDiv->children.push_back(std::make_unique<TextNode>("hello from template"));
        tpl->body.push_back(std::move(tplDiv));
        context.elementTemplates["Box"] = std::move(tpl);

        // 2. Create an AST that uses the template
        auto root = std::make_unique<RootNode>();
        auto body = std::make_unique<ElementNode>("body");
        body->children.push_back(std::make_unique<TemplateUsageNode>(TemplateType::ELEMENT, "Box"));
        root->children.push_back(std::move(body));

        // 3. Generate and assert
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<body><div>hello from template</div></body>"));
    }

    SECTION("Expand inherited style templates") {
        // 1. Define the base template
        auto baseTpl = std::make_unique<TemplateDeclarationNode>(TemplateType::STYLE, "BaseStyle");
        baseTpl->body.push_back(std::make_unique<StylePropertyNode>("color", std::make_unique<IdentifierExprNode>("red")));
        context.styleTemplates["BaseStyle"] = std::move(baseTpl);

        // 2. Define the derived template that uses the base template
        auto derivedTpl = std::make_unique<TemplateDeclarationNode>(TemplateType::STYLE, "DerivedStyle");
        derivedTpl->body.push_back(std::make_unique<TemplateUsageNode>(TemplateType::STYLE, "BaseStyle"));
        derivedTpl->body.push_back(std::make_unique<StylePropertyNode>("font-weight", std::make_unique<IdentifierExprNode>("bold")));
        context.styleTemplates["DerivedStyle"] = std::move(derivedTpl);

        // 3. Create an AST that uses the derived template
        auto root = std::make_unique<RootNode>();
        auto div = std::make_unique<ElementNode>("div");
        auto styleNode = std::make_unique<StyleNode>();
        styleNode->templateUsages.push_back(std::make_unique<TemplateUsageNode>(TemplateType::STYLE, "DerivedStyle"));
        div->style = std::move(styleNode);
        root->children.push_back(std::move(div));

        // 4. Generate and assert
        Generator generator;
        std::string result = generator.generate(*root, context);
        REQUIRE(normalize_whitespace(result) == normalize_whitespace("<div style=\"color: red; font-weight: bold;\"></div>"));
    }
}