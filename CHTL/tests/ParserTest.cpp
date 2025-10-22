#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "AstNode.h"
#include "lexer/Lexer.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
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

TEST_CASE("Test parsing a type import statement", "[parser][import]")
{
    std::string input = R"([Import] [Template] @Style from "my-styles.chtl";)";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    // We expect an error at this stage
    // checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* import_node = dynamic_cast<CHTL::ImportNode*>(program->children[0].get());
    REQUIRE(import_node != nullptr);

    REQUIRE(import_node->import_scope == "[Template]");
    REQUIRE(import_node->specific_type == "@Style");
    REQUIRE(import_node->path == "my-styles.chtl");
    REQUIRE(import_node->imported_name.empty());
    REQUIRE(import_node->alias.empty());
    REQUIRE(import_node->type.empty());
}

TEST_CASE("Test parsing a Style Template with inherit keyword", "[parser][template]")
{
    std::string input = R"(
        [Template] @Style MyInheritedStyle inherit BaseStyle1, BaseStyle2 {
            font-size: 16px;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* template_def_node = dynamic_cast<CHTL::TemplateDefinitionNode*>(program->children[0].get());
    REQUIRE(template_def_node != nullptr);
    REQUIRE(template_def_node->type == "@Style");
    REQUIRE(template_def_node->name == "MyInheritedStyle");

    REQUIRE(template_def_node->inherited_templates.size() == 2);
    REQUIRE(template_def_node->inherited_templates[0] == "BaseStyle1");
    REQUIRE(template_def_node->inherited_templates[1] == "BaseStyle2");

    REQUIRE(template_def_node->properties.size() == 1);
    auto* prop = dynamic_cast<CHTL::StyleProperty*>(template_def_node->properties[0].get());
    REQUIRE(prop != nullptr);
    REQUIRE(prop->name == "font-size");
}

TEST_CASE("Test parsing a Var Template definition", "[parser][template]")
{
    std::string input = R"(
        [Template] @Var ThemeColors {
            primary: "#ff0000";
            secondary: blue;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* template_def_node = dynamic_cast<CHTL::TemplateDefinitionNode*>(program->children[0].get());
    REQUIRE(template_def_node != nullptr);
    REQUIRE(template_def_node->type == "@Var");
    REQUIRE(template_def_node->name == "ThemeColors");
    REQUIRE(template_def_node->variables.size() == 2);

    // Check first variable
    auto it = template_def_node->variables.find("primary");
    REQUIRE(it != template_def_node->variables.end());
    auto* value1 = dynamic_cast<CHTL::StringLiteral*>(it->second.get());
    REQUIRE(value1 != nullptr);
    REQUIRE(value1->value == "#ff0000");

    // Check second variable
    it = template_def_node->variables.find("secondary");
    REQUIRE(it != template_def_node->variables.end());
    auto* value2 = dynamic_cast<CHTL::Identifier*>(it->second.get());
    REQUIRE(value2 != nullptr);
    REQUIRE(value2->value == "blue");
}

TEST_CASE("Test parsing an Element Template definition", "[parser][template]")
{
    std::string input = R"(
        [Template] @Element MyBox {
            div {
                text: "This is a box.";
            }
            span {}
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* template_def_node = dynamic_cast<CHTL::TemplateDefinitionNode*>(program->children[0].get());
    REQUIRE(template_def_node != nullptr);
    REQUIRE(template_def_node->type == "@Element");
    REQUIRE(template_def_node->name == "MyBox");
    REQUIRE(template_def_node->body.size() == 2);

    // Check first child (div)
    auto* div_node = dynamic_cast<CHTL::ElementNode*>(template_def_node->body[0].get());
    REQUIRE(div_node != nullptr);
    REQUIRE(div_node->tag_name == "div");
    REQUIRE(div_node->children.size() == 1);

    auto* text_node = dynamic_cast<CHTL::TextNode*>(div_node->children[0].get());
    REQUIRE(text_node != nullptr);
    REQUIRE(text_node->value == "This is a box.");

    // Check second child (span)
    auto* span_node = dynamic_cast<CHTL::ElementNode*>(template_def_node->body[1].get());
    REQUIRE(span_node != nullptr);
    REQUIRE(span_node->tag_name == "span");
    REQUIRE(span_node->children.empty());
}

TEST_CASE("Test parsing a Custom Style with valueless properties", "[parser][custom]")
{
    std::string input = R"(
        [Custom] @Style ValuelessText {
            color,
            font-size;
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
    REQUIRE(custom_def_node->name == "ValuelessText");
    REQUIRE(custom_def_node->children.size() == 2);

    // Check first property (color)
    auto* prop1 = dynamic_cast<CHTL::StyleProperty*>(custom_def_node->children[0].get());
    REQUIRE(prop1 != nullptr);
    REQUIRE(prop1->name == "color");
    REQUIRE(prop1->value == nullptr);

    // Check second property (font-size)
    auto* prop2 = dynamic_cast<CHTL::StyleProperty*>(custom_def_node->children[1].get());
    REQUIRE(prop2 != nullptr);
    REQUIRE(prop2->name == "font-size");
    REQUIRE(prop2->value == nullptr);
}

TEST_CASE("Test parsing a style property using a variable", "[parser][expression]")
{
    std::string input = R"(
        style {
            color: ThemeColors(primary);
        }
    )";
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

    auto* var_access = dynamic_cast<CHTL::VariableAccessNode*>(prop->value.get());
    REQUIRE(var_access != nullptr);
    REQUIRE(var_access->template_name == "ThemeColors");
    REQUIRE(var_access->variable_name == "primary");
}

TEST_CASE("Test parsing attribute access expression in a style property", "[parser][expression]")
{
    std::string input = R"(
        style {
            width: .box.width + 10;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    auto* style_node = dynamic_cast<CHTL::StyleNode*>(program->children[0].get());
    REQUIRE(style_node != nullptr);
    auto* prop = dynamic_cast<CHTL::StyleProperty*>(style_node->children[0].get());
    REQUIRE(prop != nullptr);
    REQUIRE(prop->name == "width");

    // Check for expression: (.box.width + 10)
    auto* expr = dynamic_cast<CHTL::InfixExpression*>(prop->value.get());
    REQUIRE(expr != nullptr);
    REQUIRE(expr->op == "+");

    // Check left side: .box.width
    auto* attr_access = dynamic_cast<CHTL::AttributeAccessExpression*>(expr->left.get());
    REQUIRE(attr_access != nullptr);
    REQUIRE(attr_access->selector == ".box");
    REQUIRE(attr_access->attribute_name == "width");

    // Check right side: 10
    auto* right_num = dynamic_cast<CHTL::NumberLiteral*>(expr->right.get());
    REQUIRE(right_num != nullptr);
    REQUIRE(right_num->value == 10.0);
}

TEST_CASE("Test parsing a script block", "[parser][script]")
{
    std::string input = R"(
        div {
            script {
                let my_element = {{ .my-div }};
            }
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
    REQUIRE(element_node->children.size() == 1);

    auto* script_node = dynamic_cast<CHTL::ScriptNode*>(element_node->children[0].get());
    REQUIRE(script_node != nullptr);

    // Verify that the CHTL JS AST was correctly generated
    REQUIRE(script_node->js_ast != nullptr);
    REQUIRE(script_node->js_ast->children.size() == 3);

    auto* raw1 = dynamic_cast<CHTLJS::RawJSNode*>(script_node->js_ast->children[0].get());
    REQUIRE(raw1 != nullptr);
    REQUIRE(raw1->content == "\n                let my_element = ");

    auto* selector_node = dynamic_cast<CHTLJS::EnhancedSelectorNode*>(script_node->js_ast->children[1].get());
    REQUIRE(selector_node != nullptr);
    REQUIRE(selector_node->selector == ".my-div");

    auto* raw2 = dynamic_cast<CHTLJS::RawJSNode*>(script_node->js_ast->children[2].get());
    REQUIRE(raw2 != nullptr);
    REQUIRE(raw2->content == ";\n            ");
}

TEST_CASE("Test parsing an if block", "[parser][if]")
{
    std::string input = R"(
        div {
            if {
                condition: 1 > 0;
                display: none;
                color: "red";
            }
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
    REQUIRE(element_node->children.size() == 1);

    auto* if_node = dynamic_cast<CHTL::IfNode*>(element_node->children[0].get());
    REQUIRE(if_node != nullptr);

    // Check condition
    auto* cond_expr = dynamic_cast<CHTL::InfixExpression*>(if_node->condition.get());
    REQUIRE(cond_expr != nullptr);
    REQUIRE(cond_expr->op == ">");

    // Check consequence
    REQUIRE(if_node->consequence.size() == 2);
    auto* prop1_node = dynamic_cast<CHTL::StyleProperty*>(if_node->consequence[0].get());
    REQUIRE(prop1_node != nullptr);
    REQUIRE(prop1_node->name == "display");
    auto* prop1_val = dynamic_cast<CHTL::Identifier*>(prop1_node->value.get());
    REQUIRE(prop1_val->value == "none");

    auto* prop2_node = dynamic_cast<CHTL::StyleProperty*>(if_node->consequence[1].get());
    REQUIRE(prop2_node != nullptr);
    REQUIRE(prop2_node->name == "color");
    auto* prop2_val = dynamic_cast<CHTL::StringLiteral*>(prop2_node->value.get());
    REQUIRE(prop2_val->value == "red");
}

TEST_CASE("Test parsing an if-else if-else chain", "[parser][if]")
{
    std::string input = R"(
        div {
            if { condition: 1 > 2; color: "red"; }
            else if { condition: 1 > 1; color: "green"; }
            else { color: "blue"; }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    auto* element_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(element_node != nullptr);
    REQUIRE(element_node->children.size() == 1);

    // Check if-node
    auto* if_node = dynamic_cast<CHTL::IfNode*>(element_node->children[0].get());
    REQUIRE(if_node != nullptr);
    REQUIRE(if_node->consequence.size() == 1);
    REQUIRE(if_node->alternative != nullptr);

    // Check else-if-node
    auto* else_if_node = dynamic_cast<CHTL::IfNode*>(if_node->alternative.get());
    REQUIRE(else_if_node != nullptr);
    REQUIRE(else_if_node->consequence.size() == 1);
    REQUIRE(else_if_node->alternative != nullptr);

    // Check else-node
    auto* else_node = dynamic_cast<CHTL::ElseNode*>(else_if_node->alternative.get());
    REQUIRE(else_node != nullptr);
    REQUIRE(else_node->consequence.size() == 1);
    auto* prop_node = dynamic_cast<CHTL::StyleProperty*>(else_node->consequence[0].get());
    REQUIRE(prop_node != nullptr);
    REQUIRE(prop_node->name == "color");
}

TEST_CASE("Test parsing conditional rendering of an element", "[parser][if]")
{
    std::string input = R"(
        body {
            if {
                condition: 1 > 0;
                div {
                    text: "This should be rendered";
                }
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);
    REQUIRE(program != nullptr);
    auto* body = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(body != nullptr);
    auto* if_node = dynamic_cast<CHTL::IfNode*>(body->children[0].get());
    REQUIRE(if_node != nullptr);
    REQUIRE(if_node->consequence.size() == 1);
    auto* div_node = dynamic_cast<CHTL::ElementNode*>(if_node->consequence[0].get());
    REQUIRE(div_node != nullptr);
    REQUIRE(div_node->tag_name == "div");
}

TEST_CASE("Test parsing a CustomUsage with insert specialization", "[parser]")
{
    SECTION("insert after selector")
    {
        std::string input = R"(
            @Element MyCustomElement {
                insert after div[0] {
                    span {}
                }
            }
        )";
        CHTL::Lexer l(input);
        CHTL::Parser p(l);
        // We need to parse this within a context that allows @Element usage, like a style block or element body
        // Let's wrap it in a dummy element
        std::string wrapped_input = "body { " + input + " }";
        CHTL::Lexer l_wrapped(wrapped_input);
        CHTL::Parser p_wrapped(l_wrapped);
        auto program = p_wrapped.ParseProgram();

        checkParserErrors(p_wrapped);

        REQUIRE(program != nullptr);
        auto* body = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        REQUIRE(body != nullptr);
        auto* usage = dynamic_cast<CHTL::CustomUsageNode*>(body->children[0].get());
        REQUIRE(usage != nullptr);
        REQUIRE(usage->type == "@Element");
        REQUIRE(usage->name == "MyCustomElement");
        REQUIRE(usage->specializations.size() == 1);

        auto* insert_spec = dynamic_cast<CHTL::InsertSpecializationNode*>(usage->specializations[0].get());
        REQUIRE(insert_spec != nullptr);
        REQUIRE(insert_spec->position == "after");
        REQUIRE(insert_spec->target_selector == "div[0]");
        REQUIRE(insert_spec->content.size() == 1);
        auto* inserted_content = dynamic_cast<CHTL::ElementNode*>(insert_spec->content[0].get());
        REQUIRE(inserted_content != nullptr);
        REQUIRE(inserted_content->tag_name == "span");
    }

    SECTION("insert at top")
    {
        std::string input = R"(
            @Element MyCustomElement {
                insert at top {
                    p { text: "Header"; }
                }
            }
        )";
        std::string wrapped_input = "body { " + input + " }";
        CHTL::Lexer l(wrapped_input);
        CHTL::Parser p(l);
        auto program = p.ParseProgram();
        checkParserErrors(p);

        auto* body = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
        auto* usage = dynamic_cast<CHTL::CustomUsageNode*>(body->children[0].get());
        auto* insert_spec = dynamic_cast<CHTL::InsertSpecializationNode*>(usage->specializations[0].get());

        REQUIRE(insert_spec != nullptr);
        REQUIRE(insert_spec->position == "at top");
        REQUIRE(insert_spec->target_selector.empty());
        REQUIRE(insert_spec->content.size() == 1);
        auto* p_node = dynamic_cast<CHTL::ElementNode*>(insert_spec->content[0].get());
        REQUIRE(p_node != nullptr);
        REQUIRE(p_node->tag_name == "p");
    }
    }

TEST_CASE("Test parsing an except statement", "[parser][except]")
{
    std::string input = "div { except span, a; }";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);
    REQUIRE(program != nullptr);
    auto* div = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);
    auto* except_node = dynamic_cast<CHTL::ExceptNode*>(div->children[0].get());
    REQUIRE(except_node != nullptr);
    REQUIRE(except_node->constraints.size() == 2);
    REQUIRE(except_node->constraints[0].path.size() == 1);
    REQUIRE(except_node->constraints[0].path[0] == "span");
    REQUIRE(except_node->constraints[1].path.size() == 1);
    REQUIRE(except_node->constraints[1].path[0] == "a");
}

TEST_CASE("Test parsing an except statement with type constraints", "[parser][except]")
{
    std::string input = "div { except @Html, [Custom] @Element Box; }";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);
    REQUIRE(program != nullptr);
    auto* div = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(div != nullptr);
    REQUIRE(div->children.size() == 1);
    auto* except_node = dynamic_cast<CHTL::ExceptNode*>(div->children[0].get());
    REQUIRE(except_node != nullptr);
    REQUIRE(except_node->constraints.size() == 2);
    REQUIRE(except_node->constraints[0].is_type_constraint);
    REQUIRE(except_node->constraints[0].path.size() == 1);
    REQUIRE(except_node->constraints[0].path[0] == "@Html");
    REQUIRE(!except_node->constraints[1].is_type_constraint);
    REQUIRE(except_node->constraints[1].path.size() == 3);
    REQUIRE(except_node->constraints[1].path[0] == "[Custom]");
    REQUIRE(except_node->constraints[1].path[1] == "@Element");
    REQUIRE(except_node->constraints[1].path[2] == "Box");
}

// Helper to read a file for test setup
static std::string readTestFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        FAIL("Could not open test file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST_CASE("Parser handles [Import] statements", "[parser][import]")
{
    // The test executable runs from `CHTL/build/tests/`.
    // The test resources are in `CHTL/tests/resources/`.
    // So, the relative path to the resources directory is `../tests/resources/`.

    SECTION("Successfully imports and parses a namespaced template")
    {
        // We pretend the main file is in `CHTL/tests/`
        std::string main_file_path = "../tests/main.chtl";
        // And it imports a file from its sub-directory 'resources'
        std::string main_file_content = R"([Import] @Chtl from "./resources/namespaced_template.chtl";)";

        // Clear the static set of parsed files before this test section
        CHTL::Parser::ResetParsedFiles();

        CHTL::Lexer l(main_file_content);
        CHTL::Parser p(l, main_file_path);
        auto program = p.ParseProgram();

        checkParserErrors(p);

        REQUIRE(program != nullptr);
        REQUIRE(program->templates.count("TestSpace") == 1);
        const auto& test_space = program->templates.at("TestSpace");
        REQUIRE(test_space.count("TestStyle") == 1);
        const auto* tmpl = test_space.at("TestStyle");
        REQUIRE(tmpl->name == "TestStyle");
    }

    SECTION("Gracefully handles circular dependencies")
    {
        std::string main_file_path = "../tests/resources/integration_A.chtl";
        std::string main_file_content = readTestFile(main_file_path);

        // Clear the static set of parsed files before this test section
        CHTL::Parser::ResetParsedFiles();

        CHTL::Lexer l(main_file_content);
        CHTL::Parser p(l, main_file_path);
        auto program = p.ParseProgram();

        checkParserErrors(p);

        REQUIRE(program != nullptr);
        REQUIRE(program->templates.count(CHTL::GLOBAL_NAMESPACE) == 1);
        const auto& global_ns = program->templates.at(CHTL::GLOBAL_NAMESPACE);
        REQUIRE(global_ns.count("StyleA") == 1);
        REQUIRE(global_ns.count("StyleB") == 1);
    }

    SECTION("Reports an error for a non-existent file")
    {
        std::string main_file_path = "../tests/main.chtl";
        std::string main_file_content = R"([Import] @Chtl from "./resources/non_existent_file.chtl";)";

        // Clear the static set of parsed files before this test section
        CHTL::Parser::ResetParsedFiles();

        CHTL::Lexer l(main_file_content);
        CHTL::Parser p(l, main_file_path);
        auto program = p.ParseProgram();

        const auto& errors = p.GetErrors();
        REQUIRE(!errors.empty());
        REQUIRE(errors[0].find("Could not import file") != std::string::npos);
        REQUIRE(errors[0].find("non_existent_file.chtl") != std::string::npos);
    }
}

TEST_CASE("Test parsing a simple Configuration block", "[parser]")
{
    std::string input = R"(
        [Configuration] {
            DEBUG_MODE = true;
            INDEX_INITIAL_COUNT = 1;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* config_node = dynamic_cast<CHTL::ConfigurationNode*>(program->children[0].get());
    REQUIRE(config_node != nullptr);
    REQUIRE(config_node->name.empty());
    REQUIRE(config_node->settings.size() == 2);
    REQUIRE(config_node->settings.count("DEBUG_MODE") == 1);
    REQUIRE(config_node->settings.at("DEBUG_MODE") == "true");
    REQUIRE(config_node->settings.count("INDEX_INITIAL_COUNT") == 1);
    REQUIRE(config_node->settings.at("INDEX_INITIAL_COUNT") == "1");
}

TEST_CASE("Test parsing a named Configuration block", "[parser][config]")
{
    std::string input = R"(
        [Configuration] @Config Basic {
            INDEX_INITIAL_COUNT = 0;
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);
    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);
    auto* config_node = dynamic_cast<CHTL::ConfigurationNode*>(program->children[0].get());
    REQUIRE(config_node != nullptr);
    REQUIRE(config_node->name == "Basic");
    REQUIRE(config_node->settings.size() == 1);
    REQUIRE(config_node->settings.count("INDEX_INITIAL_COUNT") == 1);
    REQUIRE(config_node->settings.at("INDEX_INITIAL_COUNT") == "0");
}

TEST_CASE("Test parsing Configuration with a Name block", "[parser]")
{
    std::string input = R"(
        [Configuration] {
            [Name] {
                KEYWORD_TEXT = text_element;
            }
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* config_node = dynamic_cast<CHTL::ConfigurationNode*>(program->children[0].get());
    REQUIRE(config_node != nullptr);
    REQUIRE(config_node->settings.empty());
    REQUIRE(config_node->name_config != nullptr);

    auto* name_config_node = config_node->name_config.get();
    REQUIRE(name_config_node->settings.size() == 1);
    REQUIRE(name_config_node->settings.count("KEYWORD_TEXT") == 1);
    REQUIRE(name_config_node->settings.at("KEYWORD_TEXT") == "text_element");
}

TEST_CASE("Test parsing an empty Configuration block", "[parser][config]")
{
    std::string input = R"([Configuration] {})";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);
    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);
    auto* config_node = dynamic_cast<CHTL::ConfigurationNode*>(program->children[0].get());
    REQUIRE(config_node != nullptr);
    REQUIRE(config_node->name.empty());
    REQUIRE(config_node->settings.empty());
    REQUIRE(config_node->name_config == nullptr);
}

TEST_CASE("Test parsing a simple use statement", "[parser]")
{
    std::string input = "use html5;";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* use_node = dynamic_cast<CHTL::UseNode*>(program->children[0].get());
    REQUIRE(use_node != nullptr);
    REQUIRE(use_node->path.size() == 1);
    REQUIRE(use_node->path[0] == "html5");
}

TEST_CASE("Test parsing a complex use statement for config", "[parser]")
{
    std::string input = R"(
        [Configuration] @Config Basic {}
        use @Config Basic;
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 2); // Expect both the config and use nodes

    // The first child should be the ConfigurationNode
    auto* config_node = dynamic_cast<CHTL::ConfigurationNode*>(program->children[0].get());
    REQUIRE(config_node != nullptr);
    REQUIRE(config_node->name == "Basic");

    // The second child should be the UseNode
    auto* use_node = dynamic_cast<CHTL::UseNode*>(program->children[1].get());
    REQUIRE(use_node != nullptr);
    REQUIRE(use_node->path.size() == 2);
    REQUIRE(use_node->path[0] == "@Config");
    REQUIRE(use_node->path[1] == "Basic");
}

TEST_CASE("Test parsing a use statement for config with full name", "[parser][use]")
{
    std::string input = "use [Configuration] @Config Basic;";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();
    checkParserErrors(p);
    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);
    auto* use_node = dynamic_cast<CHTL::UseNode*>(program->children[0].get());
    REQUIRE(use_node != nullptr);
    REQUIRE(use_node->path.size() == 3);
    REQUIRE(use_node->path[0] == "[Configuration]");
    REQUIRE(use_node->path[1] == "@Config");
    REQUIRE(use_node->path[2] == "Basic");
}

TEST_CASE("Test parsing a simple Origin block", "[parser]")
{
    std::string input = R"(
        [Origin] @Html {
            <div class="raw-html"></div>
        }
    )";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* origin_node = dynamic_cast<CHTL::OriginNode*>(program->children[0].get());
    REQUIRE(origin_node != nullptr);
    REQUIRE(origin_node->type == "@Html");
    REQUIRE(origin_node->name.empty()); // No name in this case
    // Note: The content includes the surrounding whitespace from the input string.
    // A more robust parser might trim this, but for now we test the raw capture.
    REQUIRE(origin_node->content == "\n            <div class=\"raw-html\"></div>\n        ");
}

TEST_CASE("Test parsing a named Origin block", "[parser]")
{
    std::string input = R"([Origin] @JavaScript myScript { let x = 1; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* origin_node = dynamic_cast<CHTL::OriginNode*>(program->children[0].get());
    REQUIRE(origin_node != nullptr);
    REQUIRE(origin_node->type == "@JavaScript");
    REQUIRE(origin_node->name == "myScript");
    REQUIRE(origin_node->content == " let x = 1; ");
}

TEST_CASE("Test parsing a conditional expression in a style property", "[parser]")
{
    std::string input = R"(
        style {
            background-color: width > 50 ? "red" : "blue";
        }
    )";
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
    REQUIRE(prop->name == "background-color");

    auto* cond_expr = dynamic_cast<CHTL::ConditionalExpression*>(prop->value.get());
    REQUIRE(cond_expr != nullptr);

    // Check the condition: width > 50
    auto* condition = dynamic_cast<CHTL::InfixExpression*>(cond_expr->condition.get());
    REQUIRE(condition != nullptr);
    REQUIRE(condition->op == ">");
    auto* cond_left = dynamic_cast<CHTL::Identifier*>(condition->left.get());
    REQUIRE(cond_left != nullptr);
    REQUIRE(cond_left->value == "width");
    auto* cond_right = dynamic_cast<CHTL::NumberLiteral*>(condition->right.get());
    REQUIRE(cond_right != nullptr);
    REQUIRE(cond_right->value == 50.0);

    // Check the consequence: "red"
    auto* consequence = dynamic_cast<CHTL::StringLiteral*>(cond_expr->consequence.get());
    REQUIRE(consequence != nullptr);
    REQUIRE(consequence->value == "red");

    // Check the alternative: "blue"
    auto* alternative = dynamic_cast<CHTL::StringLiteral*>(cond_expr->alternative.get());
    REQUIRE(alternative != nullptr);
    REQUIRE(alternative->value == "blue");
}

TEST_CASE("Test parsing attribute with equals sign (CE Equality)", "[parser]")
{
    std::string input = R"(div { id = "my-id"; })";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    auto program = p.ParseProgram();

    checkParserErrors(p);

    REQUIRE(program != nullptr);
    REQUIRE(program->children.size() == 1);

    auto* element_node = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(element_node != nullptr);
    REQUIRE(element_node->tag_name == "div");
    REQUIRE(element_node->attributes.size() == 1);
    REQUIRE(element_node->attributes[0].name == "id");
    REQUIRE(element_node->attributes[0].value == "my-id");
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

TEST_CASE("Test parsing a precise import statement", "[parser][import]")
{
    // We are creating a dummy file for the parser to "load"
    // The actual file loading is mocked out or handled by the Loader, but the parser
    // needs to correctly interpret the import statement itself.
    std::string input = R"([Import] [Template] @Style ImportedStyle from "../tests/resources/imported_template.chtl" as MyStyle;)";
    CHTL::Lexer l(input);
    CHTL::Parser p(l);

    // We call parseStatement instead of ParseProgram because import is a statement.
    // Also, we don't want to trigger the actual file loading logic in ParseProgram for this unit test.
    // Let's parse the whole program but verify the parsed node.
    auto program = p.ParseProgram();
    checkParserErrors(p);

    REQUIRE(program != nullptr);
    // The import node is now a child of the program node before being processed
    REQUIRE(program->children.size() == 1);

    auto* import_node = dynamic_cast<CHTL::ImportNode*>(program->children[0].get());
    REQUIRE(import_node != nullptr);

    REQUIRE(import_node->import_scope == "[Template]");
    REQUIRE(import_node->specific_type == "@Style");
    REQUIRE(import_node->imported_name == "ImportedStyle");
    REQUIRE(import_node->path == "../tests/resources/imported_template.chtl");
    REQUIRE(import_node->alias == "MyStyle");
    REQUIRE(import_node->type.empty()); // 통配 import should be empty
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
