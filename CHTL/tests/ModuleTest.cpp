#include "gtest/gtest.h"
#include "loader/Loader.h"
#include "nodes/ProgramNode.h"
#include "nodes/TextNode.h"
#include <fstream>
#include <memory>

using namespace CHTL;

TEST(ModuleTest, PackAndLoad)
{
    // Create a dummy .chtl file
    std::ofstream ofs("test_module.chtl");
    ofs << "text { \"hello module\" }";
    ofs.close();

    // Package the .chtl file into a .cmod file
    system("python3 ../../package.py test_module.chtl -o test_module.cmod");

    // Load the .cmod file
    auto program = Loader::LoadModule(".", "test_module.cmod");

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->GetType(), NodeType::Program);
    auto program_node = static_cast<ProgramNode*>(program.get());
    ASSERT_EQ(program_node->children.size(), 1);
    ASSERT_EQ(program_node->children[0]->GetType(), NodeType::Text);
    ASSERT_EQ(static_cast<TextNode*>(program_node->children[0].get())->value, "hello module");
}
