#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"

using namespace CHTL;

// 辅助函数：从 CHTL 源代码生成 HTML
std::string generateHtml(const std::string& source, GeneratorConfig config = GeneratorConfig{}) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    Generator generator(config);
    return generator.generate(ast);
}

// 辅助函数：去除字符串中的空白字符（用于比较）
std::string stripWhitespace(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!std::isspace(c)) {
            result += c;
        }
    }
    return result;
}

TEST_CASE("Generator - 简单元素生成", "[generator]") {
    SECTION("生成空的 div 元素") {
        std::string source = "div { }";
        std::string html = generateHtml(source);
        
        REQUIRE(stripWhitespace(html) == "<div/>");
    }
    
    SECTION("生成带标签名的元素") {
        std::string source = "span { }";
        std::string html = generateHtml(source);
        
        REQUIRE(stripWhitespace(html) == "<span/>");
    }
    
    SECTION("生成多个元素") {
        std::string source = R"(
            div { }
            span { }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(stripWhitespace(html) == "<div/><span/>");
    }
}

TEST_CASE("Generator - 元素属性生成", "[generator]") {
    SECTION("生成单个属性") {
        std::string source = R"(
            div {
                id: "container";
            }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(stripWhitespace(html) == "<divid=\"container\"/>");
    }
    
    SECTION("生成多个属性") {
        std::string source = R"(
            div {
                id: "main";
                class: "container";
            }
        )";
        std::string html = generateHtml(source);
        
        // 属性顺序可能不同，检查包含关系
        REQUIRE(html.find("id=\"main\"") != std::string::npos);
        REQUIRE(html.find("class=\"container\"") != std::string::npos);
    }
    
    SECTION("HTML 转义特殊字符") {
        std::string source = R"(
            div {
                data-value: "<script>";
            }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(html.find("&lt;script&gt;") != std::string::npos);
    }
}

TEST_CASE("Generator - 文本节点生成", "[generator]") {
    SECTION("生成简单文本") {
        std::string source = R"(
            div {
                text {
                    "Hello World"
                }
            }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(html.find("Hello World") != std::string::npos);
        REQUIRE(html.find("<div>") != std::string::npos);
        REQUIRE(html.find("</div>") != std::string::npos);
    }
    
    SECTION("转义文本中的 HTML 字符") {
        std::string source = R"(
            div {
                text {
                    "<tag> & 'quote'"
                }
            }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(html.find("&lt;tag&gt;") != std::string::npos);
        REQUIRE(html.find("&amp;") != std::string::npos);
        REQUIRE(html.find("&#39;") != std::string::npos);
    }
}

TEST_CASE("Generator - 嵌套元素生成", "[generator]") {
    SECTION("生成简单嵌套") {
        std::string source = R"(
            div {
                span { }
            }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(html.find("<div>") != std::string::npos);
        REQUIRE(html.find("<span />") != std::string::npos);
        REQUIRE(html.find("</div>") != std::string::npos);
    }
    
    SECTION("生成多层嵌套") {
        std::string source = R"(
            html {
                body {
                    div { }
                }
            }
        )";
        std::string html = generateHtml(source);
        
        REQUIRE(html.find("<html>") != std::string::npos);
        REQUIRE(html.find("<body>") != std::string::npos);
        REQUIRE(html.find("<div />") != std::string::npos);
        REQUIRE(html.find("</body>") != std::string::npos);
        REQUIRE(html.find("</html>") != std::string::npos);
    }
}

TEST_CASE("Generator - 格式化选项", "[generator]") {
    SECTION("紧凑模式（无格式化）") {
        GeneratorConfig config;
        config.prettyPrint = false;
        
        std::string source = R"(
            div {
                span { }
            }
        )";
        std::string html = generateHtml(source, config);
        
        // 不应该包含多余的空白
        REQUIRE(html.find("\n") == std::string::npos);
    }
    
    SECTION("格式化模式（默认）") {
        GeneratorConfig config;
        config.prettyPrint = true;
        
        std::string source = R"(
            div {
                span { }
            }
        )";
        std::string html = generateHtml(source, config);
        
        // 应该包含换行符
        REQUIRE(html.find("\n") != std::string::npos);
    }
}

TEST_CASE("Generator - 自闭合标签", "[generator]") {
    SECTION("HTML5 自闭合标签") {
        std::string source = R"(
            div {
                img { }
                br { }
                input { }
            }
        )";
        std::string html = generateHtml(source);
        
        // 这些标签应该是自闭合的
        REQUIRE(html.find("<img />") != std::string::npos);
        REQUIRE(html.find("<br />") != std::string::npos);
        REQUIRE(html.find("<input />") != std::string::npos);
        
        // 不应该有结束标签
        REQUIRE(html.find("</img>") == std::string::npos);
        REQUIRE(html.find("</br>") == std::string::npos);
        REQUIRE(html.find("</input>") == std::string::npos);
    }
}

TEST_CASE("Generator - 完整示例", "[generator][integration]") {
    SECTION("生成完整的 HTML 页面") {
        std::string source = R"(
            html {
                head {
                    title: "Test Page";
                }
                
                body {
                    id: "main";
                    class: "container";
                    
                    div {
                        class: "header";
                        
                        text {
                            "Welcome to CHTL"
                        }
                    }
                    
                    div {
                        class: "content";
                        
                        span {
                            text: "Hello World";
                        }
                    }
                }
            }
        )";
        
        std::string html = generateHtml(source);
        
        // 验证结构
        REQUIRE(html.find("<html>") != std::string::npos);
        REQUIRE(html.find("<head") != std::string::npos);
        REQUIRE(html.find("title=\"Test Page\"") != std::string::npos);
        REQUIRE(html.find("<body") != std::string::npos);
        REQUIRE(html.find("id=\"main\"") != std::string::npos);
        REQUIRE(html.find("class=\"container\"") != std::string::npos);
        REQUIRE(html.find("Welcome to CHTL") != std::string::npos);
        REQUIRE(html.find("text=\"Hello World\"") != std::string::npos);
        REQUIRE(html.find("</html>") != std::string::npos);
    }
}

TEST_CASE("Generator - DOCTYPE 选项", "[generator]") {
    SECTION("包含 DOCTYPE") {
        GeneratorConfig config;
        config.includeDoctype = true;
        
        std::string source = "html { }";
        std::string html = generateHtml(source, config);
        
        REQUIRE(html.find("<!DOCTYPE html>") == 0); // 应该在开头
    }
    
    SECTION("不包含 DOCTYPE（默认）") {
        GeneratorConfig config;
        config.includeDoctype = false;
        
        std::string source = "html { }";
        std::string html = generateHtml(source, config);
        
        REQUIRE(html.find("<!DOCTYPE html>") == std::string::npos);
    }
}
