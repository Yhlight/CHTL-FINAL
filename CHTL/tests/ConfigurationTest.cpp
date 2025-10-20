#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Compiler.h"
#include "nodes/ProgramNode.h"
#include "nodes/ElementNode.h"
#include "nodes/StyleNode.h"
#include <fstream>

void write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.close();
}

TEST_CASE("Configuration System End-to-End", "[Compiler]")
{
    remove("test_config.chtl");
    remove("test_main.chtl");

    write_file("test_config.chtl",
        "[Configuration] @Config Spanish\n"
        "{\n"
        "    [Name] { KEYWORD_STYLE = estilo; }\n"
        "}\n"
    );

    write_file("test_main.chtl",
        "[Import] [Configuration] @Config Spanish from \"test_config.chtl\";\n"
        "use @Config Spanish;\n"
        "div { estilo { color: red; } }"
    );

    CHTL::Compiler compiler;
    auto program = compiler.Compile("test_main.chtl");

    auto errors = compiler.GetErrors();
    CAPTURE(errors);
    REQUIRE(errors.empty());
    REQUIRE(program != nullptr);

    REQUIRE(program->children.size() == 1);
    auto* el = dynamic_cast<CHTL::ElementNode*>(program->children[0].get());
    REQUIRE(el != nullptr);
    REQUIRE(el->tag_name == "div");
    REQUIRE(el->children.size() == 1);
    auto* style_node = dynamic_cast<CHTL::StyleNode*>(el->children[0].get());
    REQUIRE(style_node != nullptr);

    remove("test_config.chtl");
    remove("test_main.chtl");
}
