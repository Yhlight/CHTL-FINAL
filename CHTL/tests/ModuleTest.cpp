#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "loader/Loader.h"
#include "nodes/ProgramNode.h"
#include "nodes/TextNode.h"
#include <fstream>
#include <memory>
#include <cstdlib>

using namespace CHTL;

TEST_CASE("Module packaging and loading", "[module]")
{
    // Create a dummy .chtl file
    std::ofstream ofs("test_module.chtl");
    ofs << "text { \"hello module\" }";
    ofs.close();

    // Package the .chtl file into a .cmod file
    int result = system("python3 ../../package.py test_module.chtl -o test_module.cmod");
    REQUIRE(result == 0);

    // Load the .cmod file
    auto program = Loader::LoadModule(".", "test_module.cmod");

    REQUIRE(program != nullptr);
    REQUIRE(program->GetType() == NodeType::Program);
    auto program_node = static_cast<ProgramNode*>(program.get());
    REQUIRE(program_node->children.size() == 1);
    REQUIRE(program_node->children[0]->GetType() == NodeType::Text);
    REQUIRE(static_cast<TextNode*>(program_node->children[0].get())->value == "hello module");
}
