// Lexer测试文件
// CATCH_CONFIG_MAIN在test_token.cpp中定义
#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include <sstream>

using namespace CHTL;

TEST_CASE("Lexer基础功能测试", "[lexer]") {
    
    SECTION("空输入") {
        std::string input = "";
        Lexer lexer(input, "test.chtl");
        
        Token token = lexer.nextToken();
        REQUIRE(token.getType() == TokenType::END_OF_FILE);
    }
    
    SECTION("仅空白字符") {
        std::string input = "   \n\t  \n  ";
        Lexer lexer(input, "test.chtl");
        
        Token token = lexer.nextToken();
        REQUIRE(token.getType() == TokenType::END_OF_FILE);
    }
}

TEST_CASE("标识符识别测试", "[lexer][identifier]") {
    
    SECTION("简单标识符") {
        std::string input = "div";
        Lexer lexer(input, "test.chtl");
        
        Token token = lexer.nextToken();
        REQUIRE(token.getType() == TokenType::IDENTIFIER);
        REQUIRE(token.getLexeme() == "div");
    }
    
    SECTION("多个标识符") {
        std::string input = "div span button";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLexeme() == "div");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "span");
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getLexeme() == "button");
        
        Token eof = lexer.nextToken();
        REQUIRE(eof.getType() == TokenType::END_OF_FILE);
    }
    
    SECTION("带下划线和数字的标识符") {
        std::string input = "my_element element123 _private";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLexeme() == "my_element");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "element123");
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getLexeme() == "_private");
    }
}

TEST_CASE("关键字识别测试", "[lexer][keyword]") {
    
    SECTION("基础块关键字") {
        std::string input = "text style script";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::KEYWORD_TEXT);
        REQUIRE(token1.getLexeme() == "text");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getType() == TokenType::KEYWORD_STYLE);
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getType() == TokenType::KEYWORD_SCRIPT);
    }
    
    SECTION("模板关键字") {
        std::string input = "[Template] [Custom] [Import]";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::KEYWORD_TEMPLATE);
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getType() == TokenType::KEYWORD_CUSTOM);
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getType() == TokenType::KEYWORD_IMPORT);
    }
    
    SECTION("类型标识关键字") {
        std::string input = "@Style @Element @Var";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::KEYWORD_AT_STYLE);
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getType() == TokenType::KEYWORD_AT_ELEMENT);
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getType() == TokenType::KEYWORD_AT_VAR);
    }
    
    SECTION("操作符关键字") {
        std::string input = "inherit delete insert from as";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::KEYWORD_INHERIT);
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getType() == TokenType::KEYWORD_DELETE);
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getType() == TokenType::KEYWORD_INSERT);
        
        Token token4 = lexer.nextToken();
        REQUIRE(token4.getType() == TokenType::KEYWORD_FROM);
        
        Token token5 = lexer.nextToken();
        REQUIRE(token5.getType() == TokenType::KEYWORD_AS);
    }
}

TEST_CASE("字符串字面量识别测试", "[lexer][string]") {
    
    SECTION("双引号字符串") {
        std::string input = R"("Hello, CHTL!")";
        Lexer lexer(input, "test.chtl");
        
        Token token = lexer.nextToken();
        REQUIRE(token.getType() == TokenType::STRING_LITERAL);
        REQUIRE(token.getLexeme() == "\"Hello, CHTL!\"");
    }
    
    SECTION("单引号字符串") {
        std::string input = "'Hello, CHTL!'";
        Lexer lexer(input, "test.chtl");
        
        Token token = lexer.nextToken();
        REQUIRE(token.getType() == TokenType::STRING_LITERAL);
        REQUIRE(token.getLexeme() == "'Hello, CHTL!'");
    }
    
    SECTION("空字符串") {
        std::string input = R"("" '')";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::STRING_LITERAL);
        REQUIRE(token1.getLexeme() == "\"\"");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getType() == TokenType::STRING_LITERAL);
        REQUIRE(token2.getLexeme() == "''");
    }
    
    SECTION("包含空格的字符串") {
        std::string input = R"("Hello World" 'Good Bye')";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLexeme() == "\"Hello World\"");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "'Good Bye'");
    }
}

TEST_CASE("数字字面量识别测试", "[lexer][number]") {
    
    SECTION("整数") {
        std::string input = "123 456 0 999";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::NUMBER_LITERAL);
        REQUIRE(token1.getLexeme() == "123");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "456");
    }
    
    SECTION("浮点数") {
        std::string input = "3.14 0.5 100.0";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::NUMBER_LITERAL);
        REQUIRE(token1.getLexeme() == "3.14");
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "0.5");
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getLexeme() == "100.0");
    }
}

TEST_CASE("符号识别测试", "[lexer][symbol]") {
    
    SECTION("括号") {
        std::string input = "{ } [ ] ( )";
        Lexer lexer(input, "test.chtl");
        
        REQUIRE(lexer.nextToken().getType() == TokenType::LEFT_BRACE);
        REQUIRE(lexer.nextToken().getType() == TokenType::RIGHT_BRACE);
        REQUIRE(lexer.nextToken().getType() == TokenType::LEFT_BRACKET);
        REQUIRE(lexer.nextToken().getType() == TokenType::RIGHT_BRACKET);
        REQUIRE(lexer.nextToken().getType() == TokenType::LEFT_PAREN);
        REQUIRE(lexer.nextToken().getType() == TokenType::RIGHT_PAREN);
    }
    
    SECTION("标点符号") {
        std::string input = ": ; , . # @";
        Lexer lexer(input, "test.chtl");
        
        REQUIRE(lexer.nextToken().getType() == TokenType::COLON);
        REQUIRE(lexer.nextToken().getType() == TokenType::SEMICOLON);
        REQUIRE(lexer.nextToken().getType() == TokenType::COMMA);
        REQUIRE(lexer.nextToken().getType() == TokenType::DOT);
        REQUIRE(lexer.nextToken().getType() == TokenType::HASH);
        REQUIRE(lexer.nextToken().getType() == TokenType::AT);
    }
}

TEST_CASE("运算符识别测试", "[lexer][operator]") {
    
    SECTION("基础运算符") {
        std::string input = "+ - * / % =";
        Lexer lexer(input, "test.chtl");
        
        REQUIRE(lexer.nextToken().getType() == TokenType::PLUS);
        REQUIRE(lexer.nextToken().getType() == TokenType::MINUS);
        REQUIRE(lexer.nextToken().getType() == TokenType::STAR);
        REQUIRE(lexer.nextToken().getType() == TokenType::SLASH);
        REQUIRE(lexer.nextToken().getType() == TokenType::PERCENT);
        REQUIRE(lexer.nextToken().getType() == TokenType::EQUAL);
    }
    
    SECTION("双字符运算符") {
        std::string input = "** -> && || == != >= <=";
        Lexer lexer(input, "test.chtl");
        
        REQUIRE(lexer.nextToken().getType() == TokenType::DOUBLE_STAR);
        REQUIRE(lexer.nextToken().getType() == TokenType::ARROW);
        REQUIRE(lexer.nextToken().getType() == TokenType::AND);
        REQUIRE(lexer.nextToken().getType() == TokenType::OR);
        REQUIRE(lexer.nextToken().getType() == TokenType::EQUAL_EQUAL);
        REQUIRE(lexer.nextToken().getType() == TokenType::NOT_EQUAL);
        REQUIRE(lexer.nextToken().getType() == TokenType::GREATER_EQUAL);
        REQUIRE(lexer.nextToken().getType() == TokenType::LESS_EQUAL);
    }
    
    SECTION("比较运算符") {
        std::string input = "> <";
        Lexer lexer(input, "test.chtl");
        
        REQUIRE(lexer.nextToken().getType() == TokenType::GREATER);
        REQUIRE(lexer.nextToken().getType() == TokenType::LESS);
    }
}

TEST_CASE("注释处理测试", "[lexer][comment]") {
    
    SECTION("行注释") {
        std::string input = "div // 这是注释\nspan";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLexeme() == "div");
        
        // 行注释应该被跳过
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "span");
    }
    
    SECTION("块注释") {
        std::string input = "div /* 这是\n多行\n注释 */ span";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLexeme() == "div");
        
        // 块注释应该被跳过
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "span");
    }
    
    SECTION("生成器注释") {
        std::string input = "# 这是生成器注释\ndiv";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getType() == TokenType::COMMENT_GENERATOR);
        REQUIRE(token1.getLexeme().find("这是生成器注释") != std::string::npos);
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLexeme() == "div");
    }
}

TEST_CASE("完整CHTL代码测试", "[lexer][integration]") {
    
    SECTION("简单div结构") {
        std::string input = R"(
div
{
    id: box;
    class: container;
}
)";
        Lexer lexer(input, "test.chtl");
        
        REQUIRE(lexer.nextToken().getLexeme() == "div");
        REQUIRE(lexer.nextToken().getType() == TokenType::LEFT_BRACE);
        REQUIRE(lexer.nextToken().getLexeme() == "id");
        REQUIRE(lexer.nextToken().getType() == TokenType::COLON);
        REQUIRE(lexer.nextToken().getLexeme() == "box");
        REQUIRE(lexer.nextToken().getType() == TokenType::SEMICOLON);
        REQUIRE(lexer.nextToken().getLexeme() == "class");
        REQUIRE(lexer.nextToken().getType() == TokenType::COLON);
        REQUIRE(lexer.nextToken().getLexeme() == "container");
        REQUIRE(lexer.nextToken().getType() == TokenType::SEMICOLON);
        REQUIRE(lexer.nextToken().getType() == TokenType::RIGHT_BRACE);
        REQUIRE(lexer.nextToken().getType() == TokenType::END_OF_FILE);
    }
}

TEST_CASE("位置追踪测试", "[lexer][location]") {
    
    SECTION("单行位置") {
        std::string input = "div span";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLocation().line == 1);
        REQUIRE(token1.getLocation().column == 1);
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLocation().line == 1);
        REQUIRE(token2.getLocation().column == 5);
    }
    
    SECTION("多行位置") {
        std::string input = "div\nspan\nbutton";
        Lexer lexer(input, "test.chtl");
        
        Token token1 = lexer.nextToken();
        REQUIRE(token1.getLocation().line == 1);
        
        Token token2 = lexer.nextToken();
        REQUIRE(token2.getLocation().line == 2);
        
        Token token3 = lexer.nextToken();
        REQUIRE(token3.getLocation().line == 3);
    }
}
