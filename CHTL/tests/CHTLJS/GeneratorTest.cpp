#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#define CATCH_CONFIG_MAIN
#include "bridge/ConcreteSaltBridge.h"
#include "catch.hpp"

using namespace CHTLJS;

TEST_CASE("CHTLJS Generator", "[generator][chtljs]") {
    ProgramNode program;
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    Generator generator(bridge);

    SECTION("ScriptLoader") {
        auto node = std::make_unique<ScriptLoaderNode>();
        node->paths.push_back("path/to/script1.js");
        node->paths.push_back("path/to/script2.js");
        program.children.push_back(std::move(node));

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
        REQUIRE(result == expected);
    }

    SECTION("Listen") {
        auto node = std::make_unique<ListenNode>();
        node->target = std::make_unique<EnhancedSelectorNode>(".my-button");
        node->events["click"] = "() => console.log('Clicked!')";
        program.children.push_back(std::move(node));

        std::string result = generator.Generate(program);

        std::string expected = "document.querySelector('.my-button')"
                             ".addEventListener('click', () => console.log('Clicked!'));\n";
        REQUIRE(result == expected);
    }

    SECTION("Delegate") {
        auto node = std::make_unique<DelegateNode>();
        node->parent_target = std::make_unique<EnhancedSelectorNode>("#container");
        node->child_targets.push_back(".child-button");
        node->events["click"] = "function(e) { e.target.style.color = 'red'; }";
        program.children.push_back(std::move(node));

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
        REQUIRE(result == expected);
    }

    SECTION("Animate") {
        auto node = std::make_unique<AnimateNode>();
        node->target = std::make_unique<EnhancedSelectorNode>("#box");
        node->duration = 1000;
        node->easing = "ease-in-out";
        node->begin_state["opacity"] = "0";
        node->when_states.push_back({0.5, {{"opacity", "0.5"}}});
        node->end_state["opacity"] = "1";
        program.children.push_back(std::move(node));

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

        REQUIRE(result == expected);
    }

    SECTION("Vir") {
        auto node = std::make_unique<VirNode>();
        program.children.push_back(std::move(node));
        std::string result = generator.Generate(program);
        REQUIRE(result == "");
    }
}
