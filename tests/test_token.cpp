#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTL/CHTLLexer/Token.h"

using namespace CHTL;

TEST_CASE("Token基础功能测试", "[token]") {
    
    SECTION("创建基本Token") {
        SourceLocation loc("test.chtl", 1, 1);
        Token token(TokenType::IDENTIFIER, "div", loc);
        
        REQUIRE(token.getType() == TokenType::IDENTIFIER);
        REQUIRE(token.getLexeme() == "div");
        REQUIRE(token.getLocation().filename == "test.chtl");
        REQUIRE(token.getLocation().line == 1);
        REQUIRE(token.getLocation().column == 1);
    }
    
    SECTION("Token类型判断") {
        SourceLocation loc("test.chtl", 1, 1);
        
        Token keyword(TokenType::KEYWORD_TEXT, "text", loc);
        REQUIRE(keyword.isKeyword() == true);
        REQUIRE(keyword.isOperator() == false);
        REQUIRE(keyword.isLiteral() == false);
        
        Token op(TokenType::PLUS, "+", loc);
        REQUIRE(op.isKeyword() == false);
        REQUIRE(op.isOperator() == true);
        REQUIRE(op.isLiteral() == false);
        
        Token literal(TokenType::STRING_LITERAL, "\"hello\"", loc);
        REQUIRE(literal.isKeyword() == false);
        REQUIRE(literal.isOperator() == false);
        REQUIRE(literal.isLiteral() == true);
    }
    
    SECTION("Token类型名称") {
        REQUIRE(Token::getTypeName(TokenType::IDENTIFIER) == "IDENTIFIER");
        REQUIRE(Token::getTypeName(TokenType::KEYWORD_TEXT) == "KEYWORD_TEXT");
        REQUIRE(Token::getTypeName(TokenType::LEFT_BRACE) == "LEFT_BRACE");
        REQUIRE(Token::getTypeName(TokenType::PLUS) == "PLUS");
        REQUIRE(Token::getTypeName(TokenType::END_OF_FILE) == "EOF");
    }
    
    SECTION("Token字符串表示") {
        SourceLocation loc("test.chtl", 10, 5);
        Token token(TokenType::IDENTIFIER, "div", loc);
        
        std::string str = token.toString();
        REQUIRE(str.find("IDENTIFIER") != std::string::npos);
        REQUIRE(str.find("div") != std::string::npos);
        REQUIRE(str.find("test.chtl") != std::string::npos);
        REQUIRE(str.find("10") != std::string::npos);
        REQUIRE(str.find("5") != std::string::npos);
    }
    
    SECTION("关键字Token") {
        SourceLocation loc("test.chtl", 1, 1);
        
        Token templateToken(TokenType::KEYWORD_TEMPLATE, "[Template]", loc);
        REQUIRE(templateToken.isKeyword() == true);
        REQUIRE(Token::getTypeName(TokenType::KEYWORD_TEMPLATE) == "KEYWORD_TEMPLATE");
        
        Token styleToken(TokenType::KEYWORD_AT_STYLE, "@Style", loc);
        REQUIRE(styleToken.isKeyword() == true);
        REQUIRE(Token::getTypeName(TokenType::KEYWORD_AT_STYLE) == "KEYWORD_AT_STYLE");
    }
    
    SECTION("运算符Token") {
        SourceLocation loc("test.chtl", 1, 1);
        
        Token doubleStarToken(TokenType::DOUBLE_STAR, "**", loc);
        REQUIRE(doubleStarToken.isOperator() == true);
        REQUIRE(doubleStarToken.getLexeme() == "**");
        
        Token andToken(TokenType::AND, "&&", loc);
        REQUIRE(andToken.isOperator() == true);
        
        Token arrowToken(TokenType::ARROW, "->", loc);
        REQUIRE(arrowToken.isOperator() == true);
    }
    
    SECTION("字面量Token") {
        SourceLocation loc("test.chtl", 1, 1);
        
        Token stringToken(TokenType::STRING_LITERAL, "\"Hello CHTL\"", loc);
        REQUIRE(stringToken.isLiteral() == true);
        
        Token unquotedToken(TokenType::UNQUOTED_LITERAL, "HelloCHTL", loc);
        REQUIRE(unquotedToken.isLiteral() == true);
        
        Token numberToken(TokenType::NUMBER_LITERAL, "100", loc);
        REQUIRE(numberToken.isLiteral() == true);
    }
}

TEST_CASE("SourceLocation测试", "[token][location]") {
    
    SECTION("默认构造函数") {
        SourceLocation loc;
        REQUIRE(loc.filename == "");
        REQUIRE(loc.line == 1);
        REQUIRE(loc.column == 1);
    }
    
    SECTION("参数构造函数") {
        SourceLocation loc("main.chtl", 42, 10);
        REQUIRE(loc.filename == "main.chtl");
        REQUIRE(loc.line == 42);
        REQUIRE(loc.column == 10);
    }
}
