#include "../third-party/catch.hpp"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLGenerator/Generator.h"
#include <string>

using namespace CHTL;

TEST_CASE("Auto Head - Create head when missing", "[auto-head]") {
    std::string input = R"(
        html {
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
    
    // 检查 <head> 被自动创建
    REQUIRE(output.find("<head>") != std::string::npos);
    REQUIRE(output.find("</head>") != std::string::npos);
    
    // 检查全局样式在 <head> 中
    REQUIRE(output.find("<style>") != std::string::npos);
    REQUIRE(output.find(".box") != std::string::npos);
    
    // 检查 <head> 在 <body> 之前
    size_t headPos = output.find("<head>");
    size_t bodyPos = output.find("<body>");
    REQUIRE(headPos < bodyPos);
}

TEST_CASE("Auto Head - Preserve existing head", "[auto-head]") {
    std::string input = R"(
        html {
            head {
                title: "测试";
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
    
    // 检查 <head> 存在
    REQUIRE(output.find("<head") != std::string::npos);
    
    // 检查 title 属性保留
    REQUIRE(output.find("title=\"测试\"") != std::string::npos);
    
    // 检查样式被注入
    REQUIRE(output.find("<style>") != std::string::npos);
}

TEST_CASE("Auto Head - Multiple style rules", "[auto-head]") {
    std::string input = R"(
        html {
            body {
                div {
                    style {
                        .container {
                            width: 1200px;
                        }
                    }
                }
                div {
                    style {
                        .box {
                            padding: 20px;
                        }
                    }
                }
                div {
                    style {
                        .card {
                            margin: 10px;
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
    
    // 检查 <head> 被创建
    REQUIRE(output.find("<head>") != std::string::npos);
    
    // 检查所有样式规则都在
    REQUIRE(output.find(".container") != std::string::npos);
    REQUIRE(output.find(".box") != std::string::npos);
    REQUIRE(output.find(".card") != std::string::npos);
}

TEST_CASE("Auto Head - With pseudo classes", "[auto-head]") {
    std::string input = R"(
        html {
            body {
                div {
                    style {
                        .button {
                            padding: 10px;
                        }
                        
                        &:hover {
                            padding: 12px;
                        }
                        
                        &::before {
                            content: ">";
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
    
    // 检查 <head> 被创建
    REQUIRE(output.find("<head>") != std::string::npos);
    
    // 检查所有样式规则
    REQUIRE(output.find(".button") != std::string::npos);
    REQUIRE(output.find(".button:hover") != std::string::npos);
    REQUIRE(output.find(".button::before") != std::string::npos);
}

TEST_CASE("Auto Head - No html element", "[auto-head]") {
    std::string input = R"(
        div {
            style {
                .box {
                    width: 100px;
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
    
    // 没有 <html>，无法创建 <head>
    REQUIRE(output.find("<head>") == std::string::npos);
    
    // 但元素仍然有类名
    REQUIRE(output.find("class=\"box\"") != std::string::npos);
}

TEST_CASE("Auto Head - With DOCTYPE", "[auto-head]") {
    std::string input = R"(
        html {
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
    
    GeneratorConfig config;
    config.includeDoctype = true;
    Generator generator(config);
    std::string output = generator.generate(ast);
    
    // 检查 DOCTYPE
    REQUIRE(output.find("<!DOCTYPE html>") != std::string::npos);
    
    // 检查 <head> 被创建
    REQUIRE(output.find("<head>") != std::string::npos);
    
    // 检查样式
    REQUIRE(output.find(".box") != std::string::npos);
}
