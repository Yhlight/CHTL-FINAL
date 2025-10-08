#include <gtest/gtest.h>
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLNode/Node.h"
#include "CHTLNode/StatementNodes.h"
#include "CHTLNode/ExpressionNodes.h"
#include <memory>

TEST(ParserTest, TestTextNodeBlock) {
    std::string input = "text { \"Hello World\" }";
    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<TextStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    ASSERT_NE(stmt->body, nullptr);
    ASSERT_EQ(stmt->body->statements.size(), 1);

    auto expr_stmt = dynamic_cast<ExpressionStatement*>(stmt->body->statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    auto str_literal = dynamic_cast<StringLiteral*>(expr_stmt->expression.get());
    ASSERT_NE(str_literal, nullptr);
    EXPECT_EQ(str_literal->value, "Hello World");
}

TEST(ParserTest, TestTextNodeAttribute) {
    std::string input = "div { text: \"Hello World\"; }";
    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<ElementStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    ASSERT_NE(stmt->body, nullptr);
    ASSERT_EQ(stmt->body->statements.size(), 1);

    auto attr_stmt = dynamic_cast<AttributeStatement*>(stmt->body->statements[0].get());
    ASSERT_NE(attr_stmt, nullptr);
    EXPECT_EQ(attr_stmt->name->value, "text");

    auto attr_val = dynamic_cast<StringLiteral*>(attr_stmt->value.get());
    ASSERT_NE(attr_val, nullptr);
    EXPECT_EQ(attr_val->value, "Hello World");
}