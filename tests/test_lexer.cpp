#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"

using namespace CHTL;

TEST_CASE("Lexer - 基础 Token 识别", "[lexer]") {
    SECTION("识别基本分隔符") {
        Lexer lexer("{ } [ ] ( ) ; : , .");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 11); // 10 个 token + EOF
        REQUIRE(tokens[0].type == TokenType::LEFT_BRACE);
        REQUIRE(tokens[1].type == TokenType::RIGHT_BRACE);
        REQUIRE(tokens[2].type == TokenType::LEFT_BRACKET);
        REQUIRE(tokens[3].type == TokenType::RIGHT_BRACKET);
        REQUIRE(tokens[4].type == TokenType::LEFT_PAREN);
        REQUIRE(tokens[5].type == TokenType::RIGHT_PAREN);
        REQUIRE(tokens[6].type == TokenType::SEMICOLON);
        REQUIRE(tokens[7].type == TokenType::COLON);
        REQUIRE(tokens[8].type == TokenType::COMMA);
        REQUIRE(tokens[9].type == TokenType::DOT);
    }
    
    SECTION("识别运算符") {
        Lexer lexer("+ - * / % **");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 7); // 6 个 token + EOF
        REQUIRE(tokens[0].type == TokenType::PLUS);
        REQUIRE(tokens[1].type == TokenType::MINUS);
        REQUIRE(tokens[2].type == TokenType::STAR);
        REQUIRE(tokens[3].type == TokenType::SLASH);
        REQUIRE(tokens[4].type == TokenType::PERCENT);
        REQUIRE(tokens[5].type == TokenType::POWER);
        REQUIRE(tokens[5].value == "**");
    }
}

TEST_CASE("Lexer - 字面量识别", "[lexer]") {
    SECTION("识别字符串字面量 - 双引号") {
        Lexer lexer("\"Hello World\"");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // 1 个 string + EOF
        REQUIRE(tokens[0].type == TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].value == "Hello World");
    }
    
    SECTION("识别字符串字面量 - 单引号") {
        Lexer lexer("'Hello CHTL'");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].value == "Hello CHTL");
    }
    
    SECTION("识别数字") {
        Lexer lexer("123 45.67");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3); // 2 个 number + EOF
        REQUIRE(tokens[0].type == TokenType::NUMBER);
        REQUIRE(tokens[0].value == "123");
        REQUIRE(tokens[1].type == TokenType::NUMBER);
        REQUIRE(tokens[1].value == "45.67");
    }
}

TEST_CASE("Lexer - 关键字识别", "[lexer]") {
    SECTION("识别 text 关键字") {
        Lexer lexer("text");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_TEXT);
        REQUIRE(tokens[0].value == "text");
    }
    
    SECTION("识别 style 关键字") {
        Lexer lexer("style");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_STYLE);
        REQUIRE(tokens[0].value == "style");
    }
    
    SECTION("识别 script 关键字") {
        Lexer lexer("script");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_SCRIPT);
        REQUIRE(tokens[0].value == "script");
    }
}

TEST_CASE("Lexer - 标识符识别", "[lexer]") {
    SECTION("识别简单标识符") {
        Lexer lexer("div span button");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 4); // 3 个 identifier + EOF
        REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[0].value == "div");
        REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[1].value == "span");
        REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[2].value == "button");
    }
    
    SECTION("识别带连字符的标识符") {
        Lexer lexer("background-color font-size");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[0].value == "background-color");
        REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[1].value == "font-size");
    }
}

TEST_CASE("Lexer - 注释识别", "[lexer]") {
    SECTION("识别单行注释") {
        Lexer lexer("div // 这是注释\nspan");
        auto tokens = lexer.tokenize();
        
        // 注释会被跳过
        REQUIRE(tokens.size() == 3); // div, span, EOF
        REQUIRE(tokens[0].value == "div");
        REQUIRE(tokens[1].value == "span");
    }
    
    SECTION("识别多行注释") {
        Lexer lexer("div /* 这是\n多行注释 */ span");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3); // div, span, EOF
        REQUIRE(tokens[0].value == "div");
        REQUIRE(tokens[1].value == "span");
    }
}

TEST_CASE("Lexer - 简单 CHTL 代码", "[lexer]") {
    SECTION("解析简单的 div 元素") {
        std::string code = R"(
            div {
                text: "Hello World";
            }
        )";
        
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() > 0);
        REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[0].value == "div");
        REQUIRE(tokens[1].type == TokenType::LEFT_BRACE);
    }
}

TEST_CASE("Lexer - 行列号跟踪", "[lexer]") {
    SECTION("跟踪行号和列号") {
        std::string code = "div\nspan";
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].line == 1);
        REQUIRE(tokens[1].line == 2);
    }
}

TEST_CASE("Lexer - CHTL 特殊语法块", "[lexer][special]") {
    SECTION("识别 [Template] 关键字") {
        Lexer lexer("[Template]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // [Template] + EOF
        REQUIRE(tokens[0].type == TokenType::KEYWORD_TEMPLATE);
        REQUIRE(tokens[0].value == "[Template]");
    }
    
    SECTION("识别 [Custom] 关键字") {
        Lexer lexer("[Custom]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_CUSTOM);
        REQUIRE(tokens[0].value == "[Custom]");
    }
    
    SECTION("识别 [Import] 关键字") {
        Lexer lexer("[Import]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_IMPORT);
        REQUIRE(tokens[0].value == "[Import]");
    }
    
    SECTION("识别 [Origin] 关键字") {
        Lexer lexer("[Origin]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_ORIGIN);
        REQUIRE(tokens[0].value == "[Origin]");
    }
    
    SECTION("识别 [Namespace] 关键字") {
        Lexer lexer("[Namespace]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_NAMESPACE);
        REQUIRE(tokens[0].value == "[Namespace]");
    }
    
    SECTION("识别 [Configuration] 关键字") {
        Lexer lexer("[Configuration]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_CONFIGURATION);
        REQUIRE(tokens[0].value == "[Configuration]");
    }
    
    SECTION("识别 [Info] 和 [Export] 关键字") {
        Lexer lexer("[Info] [Export]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3); // [Info] + [Export] + EOF
        REQUIRE(tokens[0].type == TokenType::KEYWORD_INFO);
        REQUIRE(tokens[0].value == "[Info]");
        REQUIRE(tokens[1].type == TokenType::KEYWORD_EXPORT);
        REQUIRE(tokens[1].value == "[Export]");
    }
}

TEST_CASE("Lexer - CHTL @ 前缀关键字", "[lexer][special]") {
    SECTION("识别 @Style 关键字") {
        Lexer lexer("@Style");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_AT_STYLE);
        REQUIRE(tokens[0].value == "@Style");
    }
    
    SECTION("识别 @Element 关键字") {
        Lexer lexer("@Element");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_AT_ELEMENT);
        REQUIRE(tokens[0].value == "@Element");
    }
    
    SECTION("识别 @Var 关键字") {
        Lexer lexer("@Var");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_AT_VAR);
        REQUIRE(tokens[0].value == "@Var");
    }
    
    SECTION("识别 @Html, @JavaScript, @Chtl 关键字") {
        Lexer lexer("@Html @JavaScript @Chtl");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 4); // 3 + EOF
        REQUIRE(tokens[0].type == TokenType::KEYWORD_AT_HTML);
        REQUIRE(tokens[1].type == TokenType::KEYWORD_AT_JAVASCRIPT);
        REQUIRE(tokens[2].type == TokenType::KEYWORD_AT_CHTL);
    }
}

TEST_CASE("Lexer - CHTL 其他关键字", "[lexer][keywords]") {
    SECTION("识别 from, as, inherit 等关键字") {
        Lexer lexer("from as inherit delete insert use");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 7); // 6 + EOF
        REQUIRE(tokens[0].type == TokenType::KEYWORD_FROM);
        REQUIRE(tokens[0].value == "from");
        REQUIRE(tokens[1].type == TokenType::KEYWORD_AS);
        REQUIRE(tokens[1].value == "as");
        REQUIRE(tokens[2].type == TokenType::KEYWORD_INHERIT);
        REQUIRE(tokens[2].value == "inherit");
        REQUIRE(tokens[3].type == TokenType::KEYWORD_DELETE);
        REQUIRE(tokens[3].value == "delete");
        REQUIRE(tokens[4].type == TokenType::KEYWORD_INSERT);
        REQUIRE(tokens[4].value == "insert");
        REQUIRE(tokens[5].type == TokenType::KEYWORD_USE);
        REQUIRE(tokens[5].value == "use");
    }
    
    SECTION("识别 except 关键字") {
        Lexer lexer("except");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_EXCEPT);
        REQUIRE(tokens[0].value == "except");
    }
}

TEST_CASE("Lexer - 完整的 CHTL Template 示例", "[lexer][integration]") {
    SECTION("解析 Template 声明") {
        std::string code = R"(
            [Template] @Style DefaultText {
                color: "black";
                line-height: 1.6;
            }
        )";
        
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() > 0);
        REQUIRE(tokens[0].type == TokenType::KEYWORD_TEMPLATE);
        REQUIRE(tokens[1].type == TokenType::KEYWORD_AT_STYLE);
        REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[2].value == "DefaultText");
        REQUIRE(tokens[3].type == TokenType::LEFT_BRACE);
    }
}
