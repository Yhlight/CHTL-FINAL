#include <gtest/gtest.h>
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLNode/Node.h"
#include "CHTLNode/StatementNodes.h"
#include "CHTLNode/ExpressionNodes.h"
#include <memory>

TEST(ParserTest, TestElementStatement) {
    std::string input = "div { id: \"box\"; }";
    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<ElementStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->tagName->value, "div");

    ASSERT_NE(stmt->body, nullptr);
    ASSERT_EQ(stmt->body->statements.size(), 1);

    auto attr_stmt = dynamic_cast<AttributeStatement*>(stmt->body->statements[0].get());
    ASSERT_NE(attr_stmt, nullptr);
    EXPECT_EQ(attr_stmt->name->value, "id");

    auto attr_val = dynamic_cast<StringLiteral*>(attr_stmt->value.get());
    ASSERT_NE(attr_val, nullptr);
    EXPECT_EQ(attr_val->value, "box");
}