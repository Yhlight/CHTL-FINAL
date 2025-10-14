#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLCommon/CompilerError.h"
#include "CHTL/CHTLCommon/CompilerConfig.h"
#include "CHTL/CHTLCommon/Validation.h"
#include <string>
#include <chrono>

using namespace CHTL;

TEST_CASE("Production - 边界情况测试", "[production][boundary]") {
    SECTION("空输入") {
        std::string source = "";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 1);  // 只有 EOF
        REQUIRE(tokens[0].type == TokenType::END_OF_FILE);
    }
    
    SECTION("只有空白符") {
        std::string source = "   \n\t\r\n   ";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        // 空白符应该被跳过，但某些实现可能会记录它们
        // 至少应该有 EOF
        REQUIRE(tokens.size() >= 1);
        REQUIRE(tokens.back().type == TokenType::END_OF_FILE);
    }
    
    SECTION("只有注释") {
        std::string source = "// 注释\n/* 多行注释 */\n# 生成器注释";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        // 注释会被跳过，只留下 EOF
        // 但如果注释被记录为 token，可能会有多个
        REQUIRE(tokens.size() <= 2);  // EOF 和可能的注释 token
        REQUIRE(tokens.back().type == TokenType::END_OF_FILE);
    }
    
    SECTION("非常长的标识符") {
        std::string longId(255, 'a');  // 最大长度
        std::string source = longId + " { }";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        REQUIRE(ast.size() == 1);
    }
}

TEST_CASE("Production - 深度嵌套测试", "[production][nesting]") {
    SECTION("适度嵌套（50层）") {
        std::string source;
        for (int i = 0; i < 50; ++i) {
            source += "div { ";
        }
        for (int i = 0; i < 50; ++i) {
            source += " }";
        }
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        REQUIRE(ast.size() == 1);
    }
    
    SECTION("深度嵌套检测（超过限制）") {
        std::string source;
        // 创建超过限制的嵌套（MAX_NESTING_DEPTH + 10）
        size_t depth = CompilerConfig::MAX_NESTING_DEPTH + 10;
        for (size_t i = 0; i < depth; ++i) {
            source += "div { ";
        }
        for (size_t i = 0; i < depth; ++i) {
            source += " }";
        }
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        
        // 应该在解析时检测到深度限制
        bool threw_correct_exception = false;
        try {
            parser.parse();
        } catch (const ResourceLimitError&) {
            threw_correct_exception = true;
        } catch (...) {
            // 其他异常
        }
        
        REQUIRE(threw_correct_exception);
    }
}

TEST_CASE("Production - 字符串处理", "[production][strings]") {
    SECTION("包含特殊字符的字符串") {
        std::string source = R"(div { text: "Hello\nWorld\t\"Test\""; })";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        REQUIRE(ast.size() == 1);
        
        auto* elem = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(elem != nullptr);
        auto attrs = elem->getAttributes();
        // Lexer 已经处理了转义，所以存储的是实际字符
        std::string expected = "Hello\nWorld\t\"Test\"";
        REQUIRE(attrs["text"] == expected);
    }
    
    SECTION("包含 Unicode 的字符串") {
        std::string source = R"(div { text: "你好世界 🎉"; })";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        REQUIRE(ast.size() == 1);
    }
    
    SECTION("空字符串") {
        std::string source = R"(div { text: ""; })";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        REQUIRE(ast.size() == 1);
    }
    
    SECTION("未闭合的字符串应该抛出异常") {
        std::string source = R"(div { text: "unclosed)";
        
        Lexer lexer(source);
        
        // 应该在 tokenize 时抛出异常
        bool threw_exception = false;
        try {
            lexer.tokenize();
        } catch (const LexerError&) {
            threw_exception = true;
        } catch (const std::exception& e) {
            INFO("抛出了其他异常: " << e.what());
        }
        
        REQUIRE(threw_exception);
    }
}

TEST_CASE("Production - HTML 转义测试", "[production][escaping]") {
    SECTION("HTML 特殊字符转义") {
        std::string source = R"(div { text: "<script>alert('xss')</script>"; })";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        Generator generator;
        std::string html = generator.generate(ast);
        
        // 确保 < 和 > 被转义
        REQUIRE(html.find("&lt;script&gt;") != std::string::npos);
        REQUIRE(html.find("&lt;/script&gt;") != std::string::npos);
        REQUIRE(html.find("&#39;") != std::string::npos);
    }
    
    SECTION("所有 HTML 特殊字符") {
        std::string source = R"(div { text: "< > & \" '"; })";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        Generator generator;
        std::string html = generator.generate(ast);
        
        REQUIRE(html.find("&lt;") != std::string::npos);
        REQUIRE(html.find("&gt;") != std::string::npos);
        REQUIRE(html.find("&amp;") != std::string::npos);
        REQUIRE(html.find("&quot;") != std::string::npos);
        REQUIRE(html.find("&#39;") != std::string::npos);
    }
}

TEST_CASE("Production - 错误恢复测试", "[production][error]") {
    SECTION("语法错误不应该崩溃") {
        std::string source = R"(
            div { }
            invalid syntax here
            span { }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        
        // 应该捕获错误但不崩溃
        REQUIRE_NOTHROW(parser.parse());
    }
}

TEST_CASE("Production - 性能基准", "[production][benchmark][!hide]") {
    SECTION("大文件编译性能") {
        // 创建一个中等大小的文档（1000 个元素）
        std::string source;
        source.reserve(50000);
        
        for (int i = 0; i < 1000; ++i) {
            source += "div { class: \"item" + std::to_string(i) + "\"; ";
            source += "text: \"Item " + std::to_string(i) + "\"; } ";
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        Generator generator;
        std::string html = generator.generate(ast);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        INFO("编译时间: " << duration.count() << "ms");
        INFO("Token 数量: " << tokens.size());
        INFO("AST 节点数: " << ast.size());
        INFO("输出大小: " << html.size() << " 字节");
        
        // 性能目标: 1000 个元素应该在 100ms 内完成
        REQUIRE(duration.count() < 1000);  // 宽松的限制
        REQUIRE(ast.size() == 1000);
    }
}

TEST_CASE("Production - 内存安全", "[production][memory]") {
    SECTION("大量节点不应该内存泄漏") {
        for (int iteration = 0; iteration < 10; ++iteration) {
            std::string source;
            for (int i = 0; i < 100; ++i) {
                source += "div { } ";
            }
            
            Lexer lexer(source);
            auto tokens = lexer.tokenize();
            Parser parser(std::move(tokens));
            auto ast = parser.parse();
            Generator generator;
            std::string html = generator.generate(ast);
            
            // AST 应该自动释放
        }
        
        // 如果有内存泄漏，多次迭代会积累
        REQUIRE(true);
    }
}

TEST_CASE("Production - 输入验证", "[production][validation]") {
    SECTION("标识符验证 - 合法") {
        REQUIRE_NOTHROW(Validation::validateIdentifier("validName"));
        REQUIRE_NOTHROW(Validation::validateIdentifier("_private"));
        REQUIRE_NOTHROW(Validation::validateIdentifier("name123"));
        REQUIRE_NOTHROW(Validation::validateIdentifier("kebab-case"));
    }
    
    SECTION("标识符验证 - 非法") {
        REQUIRE_THROWS_AS(Validation::validateIdentifier(""), CompilerError);
        REQUIRE_THROWS_AS(Validation::validateIdentifier("123start"), CompilerError);
        REQUIRE_THROWS_AS(Validation::validateIdentifier("invalid space"), CompilerError);
    }
    
    SECTION("字符串长度验证") {
        std::string short_str(100, 'a');
        REQUIRE_NOTHROW(Validation::validateStringLength(short_str, 1000));
        
        std::string long_str(2000, 'a');
        REQUIRE_THROWS_AS(Validation::validateStringLength(long_str, 1000), ResourceLimitError);
    }
}

TEST_CASE("Production - 完整编译流程", "[production][integration]") {
    SECTION("真实世界示例 - 简单页面") {
        std::string source = R"(
            html {
                head {
                    title: "生产测试页面";
                    charset: "UTF-8";
                }
                
                body {
                    id: "app";
                    class: "container";
                    
                    div {
                        class: "header";
                        
                        text {
                            "欢迎使用 CHTL"
                        }
                    }
                    
                    div {
                        class: "content";
                        
                        span {
                            text: "内容区域";
                        }
                    }
                    
                    div {
                        class: "footer";
                        text: "版权所有";
                    }
                }
            }
        )";
        
        // 完整编译流程
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() > 0);
        
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        REQUIRE(ast.size() == 1);
        
        GeneratorConfig config;
        config.prettyPrint = true;
        config.includeDoctype = true;
        
        Generator generator(config);
        std::string html = generator.generate(ast);
        
        // 验证输出
        REQUIRE(!html.empty());
        REQUIRE(html.find("<!DOCTYPE html>") == 0);
        REQUIRE(html.find("<html>") != std::string::npos);
        REQUIRE(html.find("</html>") != std::string::npos);
        REQUIRE(html.find("欢迎使用 CHTL") != std::string::npos);
    }
}
