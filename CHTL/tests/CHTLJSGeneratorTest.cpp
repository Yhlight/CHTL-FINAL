#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#include "CHTLJS/include/nodes/ProgramNode.h"
#include "include/bridge/ConcreteSaltBridge.h"

TEST_CASE("CHTLJS Generator Tests", "[generator]") {
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    CHTLJS::Generator generator(bridge);
    CHTLJS::ProgramNode program;

    SECTION("AnimateNode") {
        CHTLJS::ProgramNode program;
        auto node = std::make_unique<CHTLJS::AnimateNode>();
        node->target = ".box";
        node->duration = 1000;
        node->easing = "linear";
        node->begin_state["opacity"] = "0";
        node->end_state["opacity"] = "1";
        program.children.push_back(std::move(node));
        std::string result = generator.Generate(program);
        std::string expected =
            "{\n"
            "    const target = document.querySelector('.box');\n"
            "    const keyframes = [\n"
            "        {\"opacity\": \"0\"},\n"
            "        {\"opacity\": \"1\"}\n"
            "    ];\n"
            "    const options = {\n"
            "        duration: 1000,\n"
            "        easing: \"linear\"\n"
            "    };\n"
            "    target.animate(keyframes, options);\n"
            "}\n";
        REQUIRE(result == expected);
    }

    SECTION("DelegateNode") {
        CHTLJS::ProgramNode program;
        auto node = std::make_unique<CHTLJS::DelegateNode>();
        node->targets.push_back(".btn");
        node->events["click"] = "handleButtonClick";
        program.children.push_back(std::move(node));
        std::string result = generator.Generate(program);
        std::string expected =
            "{\n"
            "    const targets = [\".btn\"];\n"
            "    const events = {\"click\": handleButtonClick};\n"
            "    targets.forEach(target => {\n"
            "        document.querySelectorAll(target).forEach(element => {\n"
            "            for (const event in events) {\n"
            "                element.addEventListener(event, events[event]);\n"
            "            }\n"
            "        });\n"
            "    });\n"
            "}\n";
        REQUIRE(result == expected);
    }

    SECTION("ListenNode") {
        CHTLJS::ProgramNode program;
        auto node = std::make_unique<CHTLJS::ListenNode>();
        node->events["click"] = "handleClick";
        node->events["mouseover"] = "handleMouseOver";
        program.children.push_back(std::move(node));
        std::string result = generator.Generate(program);
        std::string expected =
            "{\n"
            "    const events = {\"click\": handleClick, \"mouseover\": handleMouseOver};\n"
            "    for (const event in events) {\n"
            "        this.addEventListener(event, events[event]);\n"
            "    }\n"
            "}\n";
        REQUIRE(result == expected);
    }

    SECTION("ScriptLoaderNode") {
        CHTLJS::ProgramNode program;
        auto node = std::make_unique<CHTLJS::ScriptLoaderNode>();
        node->paths.push_back("path/to/script1.js");
        node->paths.push_back("path/to/script2.js");
        program.children.push_back(std::move(node));
        std::string result = generator.Generate(program);
        std::string expected =
            "{\n"
            "    const scripts = [\"path/to/script1.js\", \"path/to/script2.js\"];\n"
            "    scripts.forEach(path => {\n"
            "        const script = document.createElement('script');\n"
            "        script.src = path;\n"
            "        document.head.appendChild(script);\n"
            "    });\n"
            "}\n";
        REQUIRE(result == expected);
    }

    SECTION("VirNode") {
        CHTLJS::ProgramNode program;
        auto node = std::make_unique<CHTLJS::VirNode>();
        node->name = "myVir";
        node->properties["prop1"] = "\"value1\"";
        node->properties["prop2"] = "123";
        program.children.push_back(std::move(node));
        std::string result = generator.Generate(program);
        std::string expected =
            "{\n"
            "    const myVir = {\n"
            "        \"prop1\": \"value1\",\n"
            "        \"prop2\": 123\n"
            "    };\n"
            "}\n";
        REQUIRE(result == expected);
    }
}
