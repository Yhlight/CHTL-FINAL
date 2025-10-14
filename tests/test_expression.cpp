#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLCommon/CSSValue.h"
#include "CHTL/CHTLParser/CSSExpression.h"

using namespace CHTL;

// 辅助函数：从 CHTL 生成 HTML
static std::string compileToHtml(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    Generator generator;
    return generator.generate(ast);
}

TEST_CASE("CSSValue - 基础构造和访问", "[cssvalue]") {
    SECTION("无单位数字") {
        CSSValue v(100);
        REQUIRE(v.getValue() == 100);
        REQUIRE(v.getUnit() == "");
        REQUIRE(!v.hasUnit());
        REQUIRE(v.toString() == "100");
    }
    
    SECTION("带单位数字") {
        CSSValue v(100, "px");
        REQUIRE(v.getValue() == 100);
        REQUIRE(v.getUnit() == "px");
        REQUIRE(v.hasUnit());
        REQUIRE(v.toString() == "100px");
    }
    
    SECTION("小数格式化") {
        CSSValue v(100.5, "px");
        REQUIRE(v.toString() == "100.5px");
        
        CSSValue v2(100.0, "px");
        REQUIRE(v2.toString() == "100px");  // 去除尾随零
    }
}

TEST_CASE("CSSValue - 算术运算", "[cssvalue]") {
    SECTION("加法 - 同单位") {
        CSSValue v1(100, "px");
        CSSValue v2(50, "px");
        CSSValue result = v1 + v2;
        
        REQUIRE(result.getValue() == 150);
        REQUIRE(result.getUnit() == "px");
        REQUIRE(result.toString() == "150px");
    }
    
    SECTION("减法 - 同单位") {
        CSSValue v1(100, "px");
        CSSValue v2(30, "px");
        CSSValue result = v1 - v2;
        
        REQUIRE(result.toString() == "70px");
    }
    
    SECTION("乘法 - 有单位 * 无单位") {
        CSSValue v1(50, "px");
        CSSValue v2(2);
        CSSValue result = v1 * v2;
        
        REQUIRE(result.toString() == "100px");
    }
    
    SECTION("除法 - 有单位 / 无单位") {
        CSSValue v1(100, "px");
        CSSValue v2(4);
        CSSValue result = v1 / v2;
        
        REQUIRE(result.toString() == "25px");
    }
    
    SECTION("取模") {
        CSSValue v1(100, "px");
        CSSValue v2(30, "px");
        CSSValue result = v1 % v2;
        
        REQUIRE(result.toString() == "10px");
    }
    
    SECTION("幂运算") {
        CSSValue v1(2, "px");
        CSSValue v2(3);
        CSSValue result = v1.power(v2);
        
        REQUIRE(result.toString() == "8px");
    }
}

TEST_CASE("CSSValue - 单位验证", "[cssvalue]") {
    SECTION("不同单位加法 - 应该抛出异常") {
        CSSValue v1(100, "px");
        CSSValue v2(50, "em");
        
        REQUIRE_THROWS_AS(v1 + v2, std::runtime_error);
    }
    
    SECTION("乘法 - 两个都有单位 - 应该抛出异常") {
        CSSValue v1(100, "px");
        CSSValue v2(2, "em");
        
        REQUIRE_THROWS_AS(v1 * v2, std::runtime_error);
    }
    
    SECTION("除以零 - 应该抛出异常") {
        CSSValue v1(100, "px");
        CSSValue v2(0);
        
        REQUIRE_THROWS_AS(v1 / v2, std::runtime_error);
    }
}

TEST_CASE("CSSValue - 左结合规则", "[cssvalue]") {
    SECTION("无单位 + 有单位 → 继承单位") {
        CSSValue v1(100);
        CSSValue v2(50, "px");
        CSSValue result = v1 + v2;
        
        REQUIRE(result.toString() == "150px");
    }
    
    SECTION("有单位 + 无单位 → 保留单位") {
        CSSValue v1(100, "px");
        CSSValue v2(50);
        CSSValue result = v1 + v2;
        
        REQUIRE(result.toString() == "150px");
    }
}

TEST_CASE("Expression - 基础算术运算", "[expression]") {
    SECTION("加法") {
        std::string source = R"(
            div {
                style {
                    width: 100px + 50px;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 150px") != std::string::npos);
    }
    
    SECTION("减法") {
        std::string source = R"(
            div {
                style {
                    width: 100px - 30px;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 70px") != std::string::npos);
    }
    
    SECTION("乘法") {
        std::string source = R"(
            div {
                style {
                    width: 50px * 2;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 100px") != std::string::npos);
    }
    
    SECTION("除法") {
        std::string source = R"(
            div {
                style {
                    width: 100px / 4;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 25px") != std::string::npos);
    }
}

TEST_CASE("Expression - 运算符优先级", "[expression]") {
    SECTION("乘法优先于加法") {
        std::string source = R"(
            div {
                style {
                    width: 100px + 50px * 2;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // 100 + (50 * 2) = 100 + 100 = 200
        REQUIRE(html.find("width: 200px") != std::string::npos);
    }
    
    SECTION("除法优先于减法") {
        std::string source = R"(
            div {
                style {
                    width: 100px - 50px / 2;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // 100 - (50 / 2) = 100 - 25 = 75
        REQUIRE(html.find("width: 75px") != std::string::npos);
    }
}

TEST_CASE("Expression - 括号表达式", "[expression]") {
    SECTION("改变优先级") {
        std::string source = R"(
            div {
                style {
                    width: (100px + 50px) * 2;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // (100 + 50) * 2 = 150 * 2 = 300
        REQUIRE(html.find("width: 300px") != std::string::npos);
    }
    
    SECTION("嵌套括号") {
        std::string source = R"(
            div {
                style {
                    width: ((100px + 50px) * 2) / 3;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // ((100 + 50) * 2) / 3 = 300 / 3 = 100
        REQUIRE(html.find("width: 100px") != std::string::npos);
    }
}

TEST_CASE("Expression - 一元运算符", "[expression]") {
    SECTION("负号") {
        std::string source = R"(
            div {
                style {
                    width: 100px + -50px;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 50px") != std::string::npos);
    }
    
    SECTION("正号") {
        std::string source = R"(
            div {
                style {
                    width: 100px + +50px;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 150px") != std::string::npos);
    }
}

TEST_CASE("Expression - 多种单位", "[expression]") {
    SECTION("em 单位") {
        std::string source = R"(
            div {
                style {
                    width: 10em + 5em;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 15em") != std::string::npos);
    }
    
    SECTION("rem 单位") {
        std::string source = R"(
            div {
                style {
                    width: 2rem * 3;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 6rem") != std::string::npos);
    }
    
    SECTION("百分比") {
        std::string source = R"(
            div {
                style {
                    width: 100% - 25%;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 75%") != std::string::npos);
    }
}

TEST_CASE("Expression - 复杂表达式", "[expression]") {
    SECTION("多重运算") {
        std::string source = R"(
            div {
                style {
                    width: 100px + 50px * 2 - 30px;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // 100 + (50 * 2) - 30 = 100 + 100 - 30 = 170
        REQUIRE(html.find("width: 170px") != std::string::npos);
    }
    
    SECTION("混合括号和优先级") {
        std::string source = R"(
            div {
                style {
                    width: (100px + 50px) * 2 - 50px / 2;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        // (100 + 50) * 2 - 50 / 2 = 150 * 2 - 25 = 300 - 25 = 275
        REQUIRE(html.find("width: 275px") != std::string::npos);
    }
}

TEST_CASE("Expression - 与普通属性共存", "[expression]") {
    SECTION("多个属性，部分带表达式") {
        std::string source = R"(
            div {
                style {
                    width: 100px + 50px;
                    height: 200px;
                    margin: 10px * 2;
                    padding: 5px;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 150px") != std::string::npos);
        REQUIRE(html.find("height: 200px") != std::string::npos);
        REQUIRE(html.find("margin: 20px") != std::string::npos);
        REQUIRE(html.find("padding: 5px") != std::string::npos);
    }
}

TEST_CASE("Expression - CHTL.md 官方示例", "[expression][chtl.md]") {
    SECTION("属性运算示例 - CHTL.md 第177行") {
        std::string source = R"(
            div {
                style {
                    width: 100px + 50px;
                    height: 200px * 2;
                    margin: 50px - 10px;
                    padding: 100px / 4;
                }
            }
        )";
        
        std::string html = compileToHtml(source);
        
        REQUIRE(html.find("width: 150px") != std::string::npos);
        REQUIRE(html.find("height: 400px") != std::string::npos);
        REQUIRE(html.find("margin: 40px") != std::string::npos);
        REQUIRE(html.find("padding: 25px") != std::string::npos);
    }
}
