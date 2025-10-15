#include "../third-party/catch.hpp"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLGenerator/Generator.h"
#include "../src/CHTL/CHTLCommon/GlobalStyleCollector.h"
#include <string>

using namespace CHTL;

TEST_CASE("Auto Class - Basic class selector", "[auto-class]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            width: 100px;
                        }
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查类名是否自动添加
    REQUIRE(output.find("class=\"box\"") != std::string::npos);
    
    // 检查全局样式是否生成
    REQUIRE(output.find("<style>") != std::string::npos);
    REQUIRE(output.find(".box") != std::string::npos);
    REQUIRE(output.find("width: 100px") != std::string::npos);
}

TEST_CASE("Auto Class - Multiple class selectors", "[auto-class]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .container {
                            width: 100px;
                        }
                        
                        .wrapper {
                            height: 200px;
                        }
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查两个类名都被添加
    REQUIRE(output.find("class=\"container wrapper\"") != std::string::npos);
    
    // 检查两个样式规则都存在
    REQUIRE(output.find(".container") != std::string::npos);
    REQUIRE(output.find(".wrapper") != std::string::npos);
}

TEST_CASE("Auto Class - With inline styles", "[auto-class]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            width: 100px;
                        }
                        
                        height: 200px;
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查类名
    REQUIRE(output.find("class=\"box\"") != std::string::npos);
    
    // 检查内联样式
    REQUIRE(output.find("style=\"height: 200px\"") != std::string::npos);
    
    // 检查全局样式
    REQUIRE(output.find(".box") != std::string::npos);
    REQUIRE(output.find("width: 100px") != std::string::npos);
}

TEST_CASE("Auto Class - Nested elements", "[auto-class]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .container {
                            width: 100px;
                        }
                    }
                    
                    div {
                        style {
                            .box {
                                height: 50px;
                            }
                        }
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查两个元素的类名
    REQUIRE(output.find("class=\"container\"") != std::string::npos);
    REQUIRE(output.find("class=\"box\"") != std::string::npos);
    
    // 检查两个样式规则
    REQUIRE(output.find(".container") != std::string::npos);
    REQUIRE(output.find(".box") != std::string::npos);
}

TEST_CASE("Auto Class - With expressions", "[auto-class]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            width: 100px + 50px;
                            height: 200px * 2;
                        }
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查表达式被求值
    REQUIRE(output.find("width: 150px") != std::string::npos);
    REQUIRE(output.find("height: 400px") != std::string::npos);
}

TEST_CASE("Auto Class - Color codes", "[auto-class]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            color: #ff0000;
                            background-color: #3498db;
                        }
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查颜色代码格式正确（没有空格）
    REQUIRE(output.find("#ff0000") != std::string::npos);
    REQUIRE(output.find("#3498db") != std::string::npos);
    REQUIRE(output.find("# ") == std::string::npos); // 不应该有 "# " 这样的空格
}

TEST_CASE("Auto Class - Style injection in head", "[auto-class]") {
    std::string input = R"(
        html {
            head {
                title: "Test";
            }
            body {
                div {
                    style {
                        .box {
                            width: 100px;
                        }
                    }
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    Generator generator;
    std::string output = generator.generate(ast);
    
    // 检查 <style> 标签在 <head> 内
    size_t headPos = output.find("<head");
    size_t stylePos = output.find("<style>");
    size_t headEndPos = output.find("</head>");
    
    REQUIRE(headPos != std::string::npos);
    REQUIRE(stylePos != std::string::npos);
    REQUIRE(headEndPos != std::string::npos);
    REQUIRE(headPos < stylePos);
    REQUIRE(stylePos < headEndPos);
}

TEST_CASE("Auto Class - Global style collector", "[auto-class]") {
    // 清空收集器
    GlobalStyleCollector::getInstance().clear();
    
    // 添加样式规则
    std::map<std::string, std::string> props1;
    props1["width"] = "100px";
    props1["height"] = "200px";
    GlobalStyleCollector::getInstance().addRule(".box", props1);
    
    std::map<std::string, std::string> props2;
    props2["color"] = "red";
    GlobalStyleCollector::getInstance().addRule(".text", props2);
    
    // 检查规则数量
    REQUIRE(GlobalStyleCollector::getInstance().getRuleCount() == 2);
    REQUIRE(GlobalStyleCollector::getInstance().hasRules());
    
    // 生成 CSS
    std::string css = GlobalStyleCollector::getInstance().generateCSS(true, 0);
    
    REQUIRE(css.find(".box") != std::string::npos);
    REQUIRE(css.find("width: 100px") != std::string::npos);
    REQUIRE(css.find(".text") != std::string::npos);
    REQUIRE(css.find("color: red") != std::string::npos);
    
    // 清空
    GlobalStyleCollector::getInstance().clear();
    REQUIRE(GlobalStyleCollector::getInstance().getRuleCount() == 0);
    REQUIRE_FALSE(GlobalStyleCollector::getInstance().hasRules());
}
