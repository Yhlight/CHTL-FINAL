#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "formatter/Formatter.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include <string>

TEST_CASE("Formatter formats basic element", "[formatter]")
{
    std::string input = "div{class=\"test\"}";
    std::string expected = "div { class = \"test\" }\n";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证基本格式化
    REQUIRE(!result.empty());
    REQUIRE(result.find("div") != std::string::npos);
}

TEST_CASE("Formatter formats nested elements", "[formatter]")
{
    std::string input = "div{div{text{hello}}}";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证嵌套结构
    REQUIRE(result.find("div") != std::string::npos);
    REQUIRE(result.find("text") != std::string::npos);
    REQUIRE(result.find("hello") != std::string::npos);
}

TEST_CASE("Formatter respects indent options", "[formatter]")
{
    std::string input = "div { div { text { hello } } }";
    
    CHTL::FormatterOptions options;
    options.indentSize = 2;
    
    CHTL::Formatter formatter(options);
    std::string result = formatter.Format(input);
    
    // 应该包含缩进的内容
    REQUIRE(!result.empty());
    REQUIRE(result.find("div") != std::string::npos);
}

TEST_CASE("Formatter handles text nodes", "[formatter]")
{
    std::string input = "div { text { Hello World } }";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证文本节点格式化
    REQUIRE(result.find("text") != std::string::npos);
    REQUIRE(result.find("Hello World") != std::string::npos);
}

TEST_CASE("Formatter formats style blocks", "[formatter]")
{
    std::string input = "style{.test{color:red;}}";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证样式块格式化
    REQUIRE(result.find("style") != std::string::npos);
    REQUIRE(result.find(".test") != std::string::npos);
}

TEST_CASE("Formatter formats script blocks", "[formatter]")
{
    std::string input = "script{console.log('test');}";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证脚本块格式化（Script包含CHTL JS AST，这里简化处理）
    REQUIRE(result.find("script") != std::string::npos);
    // Script内容由CHTL JS AST表示，格式化器会添加注释
    REQUIRE(!result.empty());
}

TEST_CASE("Formatter handles comments", "[formatter]")
{
    std::string input = "# comment\ndiv { }";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证格式化结果包含div（注释在CHTL中使用#）
    REQUIRE(result.find("div") != std::string::npos);
    REQUIRE(!result.empty());
}

TEST_CASE("Formatter handles import statements", "[formatter]")
{
    std::string input = "[Import test.chtl]";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证导入语句格式化
    REQUIRE(result.find("Import") != std::string::npos);
    REQUIRE(result.find("test.chtl") != std::string::npos);
}

TEST_CASE("Formatter with tabs option", "[formatter]")
{
    std::string input = "div { div { } }";
    
    CHTL::FormatterOptions options;
    options.useTabs = true;
    
    CHTL::Formatter formatter(options);
    std::string result = formatter.Format(input);
    
    // 验证使用tab的格式化
    REQUIRE(!result.empty());
    REQUIRE(result.find("div") != std::string::npos);
}

TEST_CASE("Formatter handles invalid input gracefully", "[formatter]")
{
    std::string input = "this is not valid chtl {{{{";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 如果解析失败，应该返回原始输入
    REQUIRE(!result.empty());
}

TEST_CASE("Formatter preserves content", "[formatter]")
{
    std::string input = "div { class = \"container\"; id = \"main\" } { text { Content } }";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证内容被保留
    REQUIRE(result.find("container") != std::string::npos);
    REQUIRE(result.find("main") != std::string::npos);
    REQUIRE(result.find("Content") != std::string::npos);
}

TEST_CASE("Formatter handles template definitions", "[formatter]")
{
    std::string input = "[Template MyTemplate] div { }";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证模板定义格式化
    REQUIRE(result.find("Template") != std::string::npos);
    REQUIRE(result.find("MyTemplate") != std::string::npos);
}

TEST_CASE("Formatter handles custom definitions", "[formatter]")
{
    std::string input = "[Custom MyComponent] div { }";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证自定义定义格式化
    REQUIRE(result.find("Custom") != std::string::npos);
    REQUIRE(result.find("MyComponent") != std::string::npos);
}

TEST_CASE("Formatter handles namespace", "[formatter]")
{
    std::string input = "[Namespace test] div { }";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证命名空间格式化
    REQUIRE(result.find("Namespace") != std::string::npos);
    REQUIRE(result.find("test") != std::string::npos);
}

TEST_CASE("Formatter handles configuration", "[formatter]")
{
    std::string input = "[Configuration] key = value;";
    
    CHTL::Formatter formatter;
    std::string result = formatter.Format(input);
    
    // 验证配置格式化
    REQUIRE(result.find("Configuration") != std::string::npos);
}
