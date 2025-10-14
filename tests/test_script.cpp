#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/ScriptNode.h"

// 辅助函数：编译 CHTL 源代码到 HTML
static std::string compileToHtml(const std::string& source) {
    CHTL::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    CHTL::Parser parser(tokens);
    auto ast = parser.parse();
    
    CHTL::GeneratorConfig config;
    config.prettyPrint = true;
    
    CHTL::Generator generator(config);
    return generator.generate(ast);
}

TEST_CASE("Script - 基础脚本块", "[script]") {
    SECTION("简单脚本") {
        std::string source = R"(
            button {
                script {
                    console.log("test");
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("<script>") != std::string::npos);
        REQUIRE(html.find("</script>") != std::string::npos);
        REQUIRE(html.find("console") != std::string::npos);
    }
    
    SECTION("空脚本块") {
        std::string source = R"(
            div {
                script {
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // 空脚本块也应该生成 <script> 标签
        REQUIRE(html.find("<script>") != std::string::npos);
        REQUIRE(html.find("</script>") != std::string::npos);
    }
}

TEST_CASE("Script - 与其他元素结合", "[script]") {
    SECTION("script + style") {
        std::string source = R"(
            div {
                id: "container";
                
                style {
                    width: 100px;
                }
                
                script {
                    alert("hi");
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("style=\"width: 100px\"") != std::string::npos);
        REQUIRE(html.find("<script>") != std::string::npos);
        REQUIRE(html.find("alert") != std::string::npos);
    }
    
    SECTION("script + 文本节点") {
        std::string source = R"(
            button {
                text: "Click Me";
                
                script {
                    this.addEventListener("click", callback);
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("Click Me") != std::string::npos);
        REQUIRE(html.find("<script>") != std::string::npos);
        REQUIRE(html.find("addEventListener") != std::string::npos);
    }
}

TEST_CASE("Script - 复杂 JavaScript", "[script]") {
    SECTION("函数定义") {
        std::string source = R"(
            div {
                script {
                    function test() {
                        return true;
                    }
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("function") != std::string::npos);
        REQUIRE(html.find("test") != std::string::npos);
        REQUIRE(html.find("return") != std::string::npos);
    }
    
    SECTION("多行脚本") {
        std::string source = R"(
            div {
                script {
                    var x = 10;
                    var y = 20;
                    console.log(x + y);
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("var x") != std::string::npos);
        REQUIRE(html.find("var y") != std::string::npos);
        REQUIRE(html.find("console") != std::string::npos);
    }
}

TEST_CASE("Script - ScriptNode 类测试", "[script][node]") {
    SECTION("ScriptNode 基本功能") {
        CHTL::ScriptNode node;
        
        REQUIRE(node.getType() == CHTL::NodeType::SCRIPT_BLOCK);
        REQUIRE(node.isEmpty());
        REQUIRE(node.getLineCount() == 0);
        
        node.setContent("console.log('test');");
        
        REQUIRE_FALSE(node.isEmpty());
        REQUIRE(node.getContent() == "console.log('test');");
        REQUIRE(node.getLineCount() == 1);
    }
    
    SECTION("ScriptNode appendLine") {
        CHTL::ScriptNode node;
        
        node.appendLine("var x = 10;");
        node.appendLine("var y = 20;");
        
        REQUIRE_FALSE(node.isEmpty());
        REQUIRE(node.getLineCount() == 2);
        REQUIRE(node.getContent().find("var x") != std::string::npos);
        REQUIRE(node.getContent().find("var y") != std::string::npos);
    }
}

TEST_CASE("Script - CHTL.md 示例", "[script][spec]") {
    SECTION("按钮点击事件") {
        std::string source = R"(
            button {
                id: "myBtn";
                text: "Click";
                
                script {
                    this.addEventListener("click", function() {
                        alert("Clicked!");
                    });
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("<button") != std::string::npos);
        REQUIRE(html.find("id=\"myBtn\"") != std::string::npos);
        REQUIRE(html.find("<script>") != std::string::npos);
        REQUIRE(html.find("addEventListener") != std::string::npos);
    }
}
