#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#include "bridge/ConcreteSaltBridge.h"
#include <gtest/gtest.h>

using namespace CHTLJS;

TEST(GeneratorTest, ScriptLoader) {
  ProgramNode program;
  auto node = std::make_unique<ScriptLoaderNode>();
  node->paths.push_back("path/to/script1.js");
  node->paths.push_back("path/to/script2.js");
  program.children.push_back(std::move(node));

  auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
  Generator generator(bridge);
  std::string result = generator.Generate(program);

  std::string expected =
      "(function() {\n"
      "  let script = document.createElement('script');\n"
      "  script.src = 'path/to/script1.js';\n"
      "  script.async = true;\n"
      "  document.head.appendChild(script);\n"
      "  let script = document.createElement('script');\n"
      "  script.src = 'path/to/script2.js';\n"
      "  script.async = true;\n"
      "  document.head.appendChild(script);\n"
      "})();\n";
  ASSERT_EQ(result, expected);
}

TEST(GeneratorTest, Listen) {
  ProgramNode program;
  auto node = std::make_unique<ListenNode>();
  node->target = std::make_unique<EnhancedSelectorNode>(".my-button");
  node->events["click"] = "() => console.log('Clicked!')";
  program.children.push_back(std::move(node));

  auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
  Generator generator(bridge);
  std::string result = generator.Generate(program);

  std::string expected = "document.querySelector('.my-button')"
                       ".addEventListener('click', () => console.log('Clicked!'));\n";
  ASSERT_EQ(result, expected);
}

TEST(GeneratorTest, Delegate) {
  ProgramNode program;
  auto node = std::make_unique<DelegateNode>();
  node->parent_target = std::make_unique<EnhancedSelectorNode>("#container");
  node->child_targets.push_back(".child-button");
  node->events["click"] = "function(e) { e.target.style.color = 'red'; }";
  program.children.push_back(std::move(node));

  auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
  Generator generator(bridge);
  std::string result = generator.Generate(program);

  std::string expected = "{\n"
                       "  const parent = document.querySelector('#container');\n"
                       "  parent.addEventListener('click', (event) => {\n"
                       "    let target = event.target;\n"
                       "    while (target && target !== parent && target !== document) {\n"
                       "      if (target.matches('.child-button')) {\n"
                       "        (function(e) { e.target.style.color = 'red'; }).call(target, event);\n"
                       "        return;\n"
                       "      }\n"
                       "      target = target.parentNode;\n"
                       "    }\n"
                       "  });\n"
                       "}\n";
  ASSERT_EQ(result, expected);
}

TEST(GeneratorTest, Animate) {
  ProgramNode program;
  auto node = std::make_unique<AnimateNode>();
  node->target = std::make_unique<EnhancedSelectorNode>("#box");
  node->duration = 1000;
  node->easing = "ease-in-out";
  node->begin_state["opacity"] = "0";
  node->when_states.push_back({0.5, {{"opacity", "0.5"}}});
  node->end_state["opacity"] = "1";
  program.children.push_back(std::move(node));

  auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
  Generator generator(bridge);
  std::string result = generator.Generate(program);

  std::string expected =
      "{\n"
      "const keyframes = [\n"
      "  { offset: 0, opacity: '0' },\n"
      "  { offset: 0.500000, opacity: '0.5' },\n"
      "  { offset: 1, opacity: '1' },\n"
      "];\n"
      "const options = {\n"
      "  duration: 1000,\n"
      "  easing: 'ease-in-out',\n"
      "};\n"
      "const anim = document.querySelector('#box').animate(keyframes, options);\n"
      "}\n";

  ASSERT_EQ(result, expected);
}

TEST(GeneratorTest, Vir) {
  ProgramNode program;
  auto node = std::make_unique<VirNode>();
  program.children.push_back(std::move(node));

  auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
  Generator generator(bridge);
  std::string result = generator.Generate(program);

  ASSERT_EQ(result, "");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
