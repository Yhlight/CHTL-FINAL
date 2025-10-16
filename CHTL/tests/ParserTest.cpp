#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Parser.h"
#include "AstNode.h"
#include "Lexer.h"
#include <vector>
#include <string>

// Helper function to check for parser errors
void checkParserErrors(const CHTL::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }

    std::string error_message = "Parser has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("Test parsing a simple empty element", "[parser]")
{
    std::string input = "div {}";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* element_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(element_node != nullptr);
    REQUIRE(element_node->tag_name == "div");
    REQUIRE(element_node->attributes.empty());
    REQUIRE(element_node->children.empty());
}

TEST_CASE("Test parsing style block with identifier property", "[parser]")
{
    std::string input = R"(style { color: red; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->children.size() == 1);

    auto* prop = dynamic_cast<CHTL::StyleProperty*>(style_node->children[0].get());
    REQUIRE(prop != nullptr);
    REQUIRE(prop->name == "color");
    auto* value_node = dynamic_cast<CHTL::Identifier*>(prop->value.get());
    REQUIRE(value_node != nullptr);
    REQUIRE(value_node->value == "red");
}

TEST_CASE("Test parsing a top-level text block with quotes", "[parser]")
{
    std::string input = R"(
        text {
            "Hello Block"
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* text_node = dynamic_cast<CHTL::TextNode*>(program->children[0].get());
    REQUIRE(text_node != nullptr);
    REQUIRE(text_node->value == "Hello Block");
}

TEST_CASE("Test parsing a top-level text block with unquoted literal", "[parser]")
{
    std::string input = R"(
        text {
            This is an unquoted literal.
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* text_node_unquoted = dynamic_cast<CHTL::TextNode*>(program->children[0].get());
    REQUIRE(text_node_unquoted != nullptr);
    REQUIRE(text_node_unquoted->value == "This is an unquoted literal.");
}

TEST_CASE("Test parsing text as an attribute-like property", "[parser]")
{
    std::string input = R"(
        div {
            text: "Hello World";
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* element_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(element_node != nullptr);
    REQUIRE(element_node->tag_name == "div");

    // The text property should not be in the attributes list
    REQUIRE(element_node->attributes.empty());

    // It should be a TextNode child
    REQUIRE(element_node->children.size() == 1);
    auto* text_node = dynamic_cast<CHTL::TextNode*>(element_node->children[0].get());
    REQUIRE(text_node != nullptr);
    REQUIRE(text_node->value == "Hello World");
}

TEST_CASE("Test parsing a simple TemplateUsage in a style block", "[parser]")
{
    std::string input = R"(
        div {
            style {
                @Style MyTemplate;
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* div = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(div->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->children.size() == 1);

    auto* usage = dynamic_cast<CHTL::TemplateUsageNode*>(style_node->children[0].get());
    REQUIRE(usage != nullptr);
    REQUIRE(usage->type == "@Style");
    REQUIRE(usage->name == "MyTemplate");
}

TEST_CASE("Test parsing a CustomUsage with delete specialization", "[parser]")
{
    std::string input = R"(
        div {
            style {
                @Style MyCustom {
                    delete color;
                }
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* div = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(div->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->children.size() == 1);

    auto* usage = dynamic_cast<CHTL::CustomUsageNode*>(style_node->children[0].get());
    REQUIRE(usage != nullptr);
    REQUIRE(usage->type == "@Style");
    REQUIRE(usage->name == "MyCustom");
    REQUIRE(usage->specializations.size() == 1);

    auto* del_spec = dynamic_cast<CHTL::DeleteSpecializationNode*>(usage->specializations[0].get());
    REQUIRE(del_spec != nullptr);
    REQUIRE(del_spec->property_name == "color");
}

TEST_CASE("Test parsing an import statement", "[parser]")
{
    std::string input = R"([Import] @Chtl from "../tests/resources/imported_template.chtl";)";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    // The import node itself is not added to the children, the definitions are merged directly.
    // Let's verify the template map, which should now be in the global namespace.
    REQUIRE(program->templates.count(CHTL::GLOBAL_NAMESPACE) == 1);
    const auto& global_ns = program->templates.at(CHTL::GLOBAL_NAMESPACE);
    REQUIRE(global_ns.size() == 1);
    REQUIRE(global_ns.count("ImportedStyle") == 1);

    const auto* tmpl = global_ns.at("ImportedStyle");
    REQUIRE(tmpl->name == "ImportedStyle");
    REQUIRE(tmpl->properties.size() == 2);
}

TEST_CASE("Test parsing a namespace block", "[parser]")
{
    std::string input = R"(
        [Namespace] MySpace {
            [Template] @Style MyTemplate {
                color: red;
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1); // The namespace node

    auto* ns_node = dynamic_cast<CHTL::NamespaceNode*>(program->children[0].get());
    REQUIRE(ns_node != nullptr);
    REQUIRE(ns_node->name == "MySpace");

    // Check that the template was stored in the correct namespace in the program's map
    REQUIRE(program->templates.size() == 1);
    REQUIRE(program->templates.count("MySpace") == 1);
    REQUIRE(program->templates.at("MySpace").size() == 1);
    REQUIRE(program->templates.at("MySpace").count("MyTemplate") == 1);

    const auto* tmpl = program->templates.at("MySpace").at("MyTemplate");
    REQUIRE(tmpl->name == "MyTemplate");
}

TEST_CASE("Test parsing style block with arithmetic expressions", "[parser]")
{
    std::string input = R"(style { width: 100 + 50 * 2; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
    REQUIRE(style_node != nullptr);
    REQUIRE(style_node->children.size() == 1);

    auto* prop = dynamic_cast<CHTL::StyleProperty*>(style_node->children[0].get());
    REQUIRE(prop != nullptr);
    REQUIRE(prop->name == "width");

    // Check for expression: (100 + (50 * 2))
    auto* expr = dynamic_cast<CHTL::InfixExpression*>(prop->value.get());
    REQUIRE(expr != nullptr);
    REQUIRE(expr->op == "+");

    auto* left_num = dynamic_cast<CHTL::NumberLiteral*>(expr->left.get());
    REQUIRE(left_num != nullptr);
    REQUIRE(left_num->value == 100.0);

    auto* right_infix_expr = dynamic_cast<CHTL::InfixExpression*>(expr->right.get());
    REQUIRE(right_infix_expr != nullptr);
    REQUIRE(right_infix_expr->op == "*");

    auto* right_left_num = dynamic_cast<CHTL::NumberLiteral*>(right_infix_expr->left.get());
    REQUIRE(right_left_num != nullptr);
    REQUIRE(right_left_num->value == 50.0);

    auto* right_right_num = dynamic_cast<CHTL::NumberLiteral*>(right_infix_expr->right.get());
    REQUIRE(right_right_num != nullptr);
    REQUIRE(right_right_num->value == 2.0);
}

TEST_CASE("Test parsing a generator comment", "[parser]")
{
    std::string input = R"(# this is a comment)";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* comment_node = dynamic_cast<CHTL::CommentNode*>(program->children[0].get());
    REQUIRE(comment_node != nullptr);
    REQUIRE(comment_node->value == "this is a comment");
}

TEST_CASE("Test parsing a simple Custom Style definition", "[parser]")
{
    std::string input = R"(
        [Custom] @Style MyCustomStyle {
            color: red;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* custom_def_node = dynamic_cast<CHTL::CustomDefinitionNode*>(program->children[0].get());
    REQUIRE(custom_def_node != nullptr);
    REQUIRE(custom_def_node->type == "@Style");
    REQUIRE(custom_def_node->name == "MyCustomStyle");
    REQUIRE(custom_def_node->children.size() == 1);

    auto* prop = dynamic_cast<CHTL::StyleProperty*>(custom_def_node->children[0].get());
    REQUIRE(prop != nullptr);
    REQUIRE(prop->name == "color");

    auto* value_node = dynamic_cast<CHTL::Identifier*>(prop->value.get());
    REQUIRE(value_node != nullptr);
    REQUIRE(value_node->value == "red");
}
