use crate::CHTL::CHTLLexer::lexer::Lexer;
use crate::CHTL::CHTLLexer::token::Token;
use crate::CHTL::CHTLNode::ast::*;

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
        // Read two tokens, so current_token and peek_token are both set
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

        while self.current_token != Token::Eof {
            match self.parse_statement() {
                Some(statement) => program.statements.push(statement),
                None => {}
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match &self.current_token {
            Token::Identifier(_) => {
                if self.peek_token_is(&Token::LBrace) {
                    self.parse_element_statement()
                } else if self.peek_token_is(&Token::Colon) {
                    self.parse_attribute_statement()
                }
                else {
                    None
                }
            }
            Token::Text => self.parse_text_statement(),
            _ => None,
        }
    }

    fn parse_element_statement(&mut self) -> Option<Statement> {
        let name = match self.current_token.clone() {
            Token::Identifier(name) => IdentifierExpression {
                token: self.current_token.clone(),
                value: name,
            },
            _ => return None,
        };

        if !self.expect_peek(Token::LBrace) {
            return None;
        }

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
            Token::Identifier(name) => IdentifierExpression {
                token: self.current_token.clone(),
                value: name,
            },
            _ => return None,
        };

        if !self.expect_peek(Token::Colon) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression()?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Attribute(AttributeStatement { name, value }))
    }

    fn parse_text_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::LBrace) {
            return None;
        }

        self.next_token();

        let value = match self.current_token.clone() {
            Token::String(value) => StringLiteralExpression {
                token: self.current_token.clone(),
                value,
            },
            _ => return None,
        };

        if !self.expect_peek(Token::RBrace) {
            return None;
        }

        Some(Statement::Text(TextStatement { value }))
    }

    fn parse_expression(&mut self) -> Option<Expression> {
        match &self.current_token {
            Token::Identifier(value) => Some(Expression::Identifier(IdentifierExpression {
                token: self.current_token.clone(),
                value: value.clone(),
            })),
            Token::String(value) => Some(Expression::StringLiteral(StringLiteralExpression {
                token: self.current_token.clone(),
                value: value.clone(),
            })),
            Token::Number(value) => Some(Expression::NumberLiteral(NumberLiteralExpression {
                token: self.current_token.clone(),
                value: *value,
            })),
            _ => None,
        }
    }

    fn current_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn expect_peek(&mut self, t: Token) -> bool {
        if self.peek_token_is(&t) {
            self.next_token();
            true
        } else {
            self.peek_error(&t);
            false
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
    use crate::CHTL::CHTLLexer::lexer::Lexer;

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