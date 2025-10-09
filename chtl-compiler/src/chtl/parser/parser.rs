use crate::chtl::lexer::lexer::Lexer;
use crate::chtl::lexer::token::Token;
use crate::chtl::node::ast::*;

#[derive(PartialEq, PartialOrd, Debug, Clone, Copy)]
enum Precedence {
    Lowest,
    Sum,     // + or -
    Product, // * or /
    Power,   // **
}

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

        let value = self.parse_expression(Precedence::Lowest)?;

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
        while self.is_unquoted_literal_token(&self.current_token) {
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

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left_exp = self.parse_prefix()?;

        while !self.peek_token_is(&Token::Semicolon) && precedence < self.peek_precedence() {
            self.next_token();
            left_exp = self.parse_infix_expression(left_exp)?;
        }

        Some(left_exp)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match self.current_token.clone() {
            Token::String(s) => Some(Expression::StringLiteral(StringLiteralExpression { value: s })),
            tok if self.is_unquoted_literal_token(&tok) => {
                let mut literal = String::new();
                let mut current_tok = tok;
                loop {
                    let s_val = match current_tok {
                        Token::Identifier(s) => s,
                        Token::Number(s) => s,
                        _ => unreachable!(),
                    };
                    literal.push_str(&s_val);

                    if self.is_peek_unquoted_literal_token() {
                        self.next_token();
                        current_tok = self.current_token.clone();
                        literal.push(' ');
                    } else {
                        break;
                    }
                }
                Some(Expression::UnquotedLiteral(UnquotedLiteralExpression { value: literal }))
            }
            _ => None,
        }
    }

    fn parse_infix_expression(&mut self, left: Expression) -> Option<Expression> {
        let operator = match self.current_token.clone() {
            Token::Plus => "+".to_string(),
            Token::Minus => "-".to_string(),
            Token::Asterisk => "*".to_string(),
            Token::Slash => "/".to_string(),
            Token::Percent => "%".to_string(),
            Token::Power => "**".to_string(),
            _ => return None,
        };

        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Expression::Infix(InfixExpression {
            left: Box::new(left),
            operator,
            right: Box::new(right),
        }))
    }

    fn peek_precedence(&self) -> Precedence {
        Self::token_to_precedence(&self.peek_token)
    }

    fn current_precedence(&self) -> Precedence {
        Self::token_to_precedence(&self.current_token)
    }

    fn token_to_precedence(token: &Token) -> Precedence {
        match token {
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Asterisk | Token::Slash | Token::Percent => Precedence::Product,
            Token::Power => Precedence::Power,
            _ => Precedence::Lowest,
        }
    }

    fn current_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn is_unquoted_literal_token(&self, t: &Token) -> bool {
        matches!(t, Token::Identifier(_) | Token::Number(_))
    }

    fn is_peek_unquoted_literal_token(&self) -> bool {
        self.is_unquoted_literal_token(&self.peek_token)
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

    #[test]
    fn test_infix_expressions() {
        let input = r#"
        style {
            width: 10 + 2 * 3 ** 2;
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors.is_empty(), "Parser errors: {:?}", parser.errors);

        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            let attr_stmt = &style_stmt.body[0];
            if let Statement::Attribute(attr) = attr_stmt {
                assert_eq!(attr.name.value, "width");
                if let Expression::Infix(infix_exp) = &attr.value {
                    // 10 + (2 * (3 ** 2))
                    assert_eq!(infix_exp.operator, "+");

                    if let Expression::UnquotedLiteral(left) = &*infix_exp.left {
                        assert_eq!(left.value, "10");
                    } else {
                        panic!("Expected UnquotedLiteral for left expression");
                    }

                    if let Expression::Infix(right_infix) = &*infix_exp.right {
                        assert_eq!(right_infix.operator, "*");
                        if let Expression::UnquotedLiteral(right_left) = &*right_infix.left {
                             assert_eq!(right_left.value, "2");
                        } else {
                             panic!("Expected UnquotedLiteral for right-left expression");
                        }

                        if let Expression::Infix(power_infix) = &*right_infix.right {
                            assert_eq!(power_infix.operator, "**");
                            if let Expression::UnquotedLiteral(power_left) = &*power_infix.left {
                                assert_eq!(power_left.value, "3");
                            }
                            if let Expression::UnquotedLiteral(power_right) = &*power_infix.right {
                                assert_eq!(power_right.value, "2");
                            }
                        } else {
                            panic!("Expected InfixExpression for power expression");
                        }
                    } else {
                        panic!("Expected InfixExpression for right expression");
                    }
                } else {
                    panic!("Expected InfixExpression");
                }
            } else {
                panic!("Expected AttributeStatement");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }
}