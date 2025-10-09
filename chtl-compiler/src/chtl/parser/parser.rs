use crate::chtl::lexer::lexer::Lexer;
use crate::chtl::lexer::token::Token;
use crate::chtl::node::ast::*;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };

        while !self.current_token_is(&Token::Eof) {
            if let Some(statement) = self.parse_statement() {
                program.statements.push(statement);
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token.clone() {
            Token::Identifier(name) => {
                if name == "text" && self.peek_token_is(&Token::LBrace) {
                    self.parse_text_statement()
                } else if self.peek_token_is(&Token::LBrace) {
                    self.parse_element_statement()
                } else if self.peek_token_is(&Token::Colon) {
                    self.parse_attribute_statement()
                } else {
                    None
                }
            }
            Token::Style => self.parse_style_statement(),
            Token::GeneratorComment(value) => self.parse_comment_statement(value),
            _ => None,
        }
    }

    fn parse_comment_statement(&mut self, value: String) -> Option<Statement> {
        Some(Statement::Comment(CommentStatement { value }))
    }

    fn parse_style_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::LBrace)?;
        self.next_token();

        let mut body = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                body.push(stmt);
            }
            self.next_token();
        }

        Some(Statement::Style(StyleStatement { body }))
    }

    fn parse_element_statement(&mut self) -> Option<Statement> {
        let name = match self.current_token.clone() {
            Token::Identifier(name) => IdentifierExpression { value: name },
            _ => return None,
        };

        self.expect_peek(&Token::LBrace)?;
        self.next_token();

        let mut body = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                body.push(stmt);
            }
            self.next_token();
        }

        Some(Statement::Element(ElementStatement { name, body }))
    }

    fn parse_attribute_statement(&mut self) -> Option<Statement> {
        let name = match self.current_token.clone() {
            Token::Identifier(name) => IdentifierExpression { value: name },
            _ => return None,
        };

        self.expect_peek(&Token::Colon)?;
        self.next_token();

        let value = self.parse_expression()?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Attribute(AttributeStatement { name, value }))
    }

    fn parse_text_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::LBrace)?;
        self.next_token();

        if let Token::String(value) = self.current_token.clone() {
            let text_value = StringLiteralExpression { value };
            self.next_token();
            if !self.current_token_is(&Token::RBrace) {
                return None;
            }
            return Some(Statement::Text(TextStatement { value: text_value }));
        }

        let mut literal = String::new();
        let mut first = true;
        while self.is_unquoted_literal_token() {
            if !first {
                literal.push(' ');
            }
            let s = match self.current_token.clone() {
                Token::Identifier(s) => s,
                Token::Number(s) => s,
                _ => break,
            };
            literal.push_str(&s);
            self.next_token();
            first = false;
        }

        if !self.current_token_is(&Token::RBrace) {
            return None;
        }

        Some(Statement::Text(TextStatement { value: StringLiteralExpression { value: literal } }))
    }

    fn parse_expression(&mut self) -> Option<Expression> {
        if let Token::String(value) = self.current_token.clone() {
            return Some(Expression::StringLiteral(StringLiteralExpression { value }));
        }

        let mut literal = String::new();
        if self.is_unquoted_literal_token() {
            let s_val = match self.current_token.clone() {
                Token::Identifier(s) => s,
                Token::Number(s) => s,
                _ => unreachable!(),
            };
            literal.push_str(&s_val);
        } else {
            return None;
        }

        while self.is_peek_unquoted_literal_token() {
            self.next_token();
            literal.push(' ');
            let s_val = match self.current_token.clone() {
                Token::Identifier(s) => s,
                Token::Number(s) => s,
                _ => unreachable!(),
            };
            literal.push_str(&s_val);
        }

        Some(Expression::UnquotedLiteral(UnquotedLiteralExpression { value: literal }))
    }

    fn current_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn is_unquoted_literal_token(&self) -> bool {
        matches!(&self.current_token, Token::Identifier(_) | Token::Number(_))
    }

    fn is_peek_unquoted_literal_token(&self) -> bool {
        matches!(&self.peek_token, Token::Identifier(_) | Token::Number(_))
    }

    fn expect_peek(&mut self, t: &Token) -> Option<()> {
        if self.peek_token_is(t) {
            self.next_token();
            Some(())
        } else {
            self.peek_error(t);
            None
        }
    }

    fn peek_error(&mut self, t: &Token) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            t, self.peek_token
        );
        self.errors.push(msg);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::lexer::lexer::Lexer;

    #[test]
    fn test_parse_program() {
        let input = r#"
        div {
            id: "box";
            text { "Hello" }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors.is_empty(), "Parser has errors: {:?}", parser.errors);
        assert_eq!(program.statements.len(), 1);
        let stmt = &program.statements[0];
        if let Statement::Element(element_stmt) = stmt {
            assert_eq!(element_stmt.name.value, "div");
            assert_eq!(element_stmt.body.len(), 2);

            let attr_stmt = &element_stmt.body[0];
            if let Statement::Attribute(attr) = attr_stmt {
                assert_eq!(attr.name.value, "id");
                if let Expression::StringLiteral(s) = &attr.value {
                    assert_eq!(s.value, "box");
                } else {
                    panic!("Expected StringLiteral");
                }
            } else {
                panic!("Expected AttributeStatement");
            }

            let text_stmt = &element_stmt.body[1];
            if let Statement::Text(text) = text_stmt {
                assert_eq!(text.value.value, "Hello");
            } else {
                panic!("Expected TextStatement");
            }
        } else {
            panic!("Expected ElementStatement");
        }
    }
}