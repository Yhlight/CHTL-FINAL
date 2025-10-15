#include "../third-party/catch.hpp"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLGenerator/Generator.h"
#include <string>

using namespace CHTL;

TEST_CASE("Ampersand - Basic with pseudo class", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            width: 100px;
                        }
                        
                        &:hover {
                            width: 120px;
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
    
    // 检查类名
    REQUIRE(output.find("class=\"box\"") != std::string::npos);
    
    // 检查基础样式
    REQUIRE(output.find(".box") != std::string::npos);
    REQUIRE(output.find("width: 100px") != std::string::npos);
    
    // 检查伪类样式
    REQUIRE(output.find(".box:hover") != std::string::npos);
    REQUIRE(output.find("width: 120px") != std::string::npos);
}

TEST_CASE("Ampersand - Pseudo element ::before", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .button {
                            padding: 10px;
                        }
                        
                        &::before {
                            content: "→";
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
    
    // 检查伪元素样式
    REQUIRE(output.find(".button::before") != std::string::npos);
    REQUIRE(output.find("content") != std::string::npos);
}

TEST_CASE("Ampersand - Pseudo element ::after", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .card {
                            width: 200px;
                        }
                        
                        &::after {
                            content: "✓";
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
    
    // 检查伪元素
    REQUIRE(output.find(".card::after") != std::string::npos);
}

TEST_CASE("Ampersand - Multiple pseudo classes", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            width: 100px;
                        }
                        
                        &:hover {
                            width: 120px;
                        }
                        
                        &:active {
                            width: 90px;
                        }
                        
                        &:focus {
                            outline: 2px solid blue;
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
    
    // 检查所有伪类
    REQUIRE(output.find(".box:hover") != std::string::npos);
    REQUIRE(output.find(".box:active") != std::string::npos);
    REQUIRE(output.find(".box:focus") != std::string::npos);
}

TEST_CASE("Ampersand - With ID selector", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        #header {
                            background: #333;
                        }
                        
                        &:hover {
                            background: #000;
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
    
    // 检查 id
    REQUIRE(output.find("id=\"header\"") != std::string::npos);
    
    // 检查基础样式和伪类
    REQUIRE(output.find("#header") != std::string::npos);
    REQUIRE(output.find("#header:hover") != std::string::npos);
}

TEST_CASE("Ampersand - Both pseudo class and element", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
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
                        
                        &::after {
                            content: "<";
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
    
    // 检查伪类和伪元素都存在
    REQUIRE(output.find(".button:hover") != std::string::npos);
    REQUIRE(output.find(".button::before") != std::string::npos);
    REQUIRE(output.find(".button::after") != std::string::npos);
}

TEST_CASE("Ampersand - Nested elements", "[ampersand]") {
    std::string input = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .container {
                            width: 100%;
                        }
                        
                        &:hover {
                            background: #f0f0f0;
                        }
                    }
                    
                    div {
                        style {
                            .item {
                                padding: 10px;
                            }
                            
                            &:hover {
                                padding: 15px;
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
    
    // 检查两个上下文是独立的
    REQUIRE(output.find(".container:hover") != std::string::npos);
    REQUIRE(output.find(".item:hover") != std::string::npos);
    
    // 不应该混淆
    REQUIRE(output.find(".container") != std::string::npos);
    REQUIRE(output.find(".item") != std::string::npos);
}

TEST_CASE("Ampersand - CHTL.md Example", "[ampersand][spec]") {
    // CHTL.md 第 157-172 行的示例
    std::string input = R"(
        div {
            style {
                .box {
                    width: 100px;
                }
                
                &:hover {
                    width: 120px;
                }
            }
        }
    )";
    
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // 不抛出异常即可
    REQUIRE(ast.size() > 0);
}
