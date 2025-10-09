use crate::chtl::lexer::lexer::Lexer;
use crate::chtl::lexer::token::Token;
use crate::chtl::node::ast::*;

#[derive(PartialEq, PartialOrd, Debug, Clone, Copy)]
enum Precedence {
    Lowest,
    Conditional, // ?:
    LessGreater, // > or <
    Sum,         // + or -
    Product,     // * or /
    Power,       // **
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
            Token::LBracket => {
                if self.peek_token_is(&Token::Template) {
                    return self.parse_template_definition_statement();
                }
                None
            }
            Token::Identifier(_) => {
                if self.peek_token_is(&Token::LBrace) {
                    self.parse_element_statement()
                } else if self.peek_token_is(&Token::Colon) {
                    self.parse_attribute_statement()
                } else {
                    None
                }
            }
            Token::Text => {
                if self.peek_token_is(&Token::LBrace) {
                    self.parse_text_statement()
                } else if self.peek_token_is(&Token::Colon) {
                    self.parse_attribute_statement()
                } else {
                    self.errors.push(format!(
                        "Unexpected token after 'text' keyword: {:?}",
                        self.peek_token
                    ));
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
            let stmt = match self.current_token.clone() {
                Token::Dot | Token::Hash | Token::Ampersand => self.parse_style_rule_statement(),
                Token::At => self.parse_use_template_statement(),
                Token::Identifier(_) => {
                    if self.peek_token_is(&Token::LBrace) {
                        self.parse_style_rule_statement()
                    } else if self.peek_token_is(&Token::Colon) {
                        self.parse_attribute_statement()
                    } else {
                        self.errors.push(format!(
                            "Unexpected token in style block: {:?}",
                            self.peek_token
                        ));
                        None
                    }
                }
                _ => {
                    self.errors.push(format!(
                        "Unexpected token in style block: {:?}",
                        self.current_token
                    ));
                    None
                }
            };

            if let Some(s) = stmt {
                body.push(s);
            }
            self.next_token();
        }

        Some(Statement::Style(StyleStatement { body }))
    }

    fn parse_style_rule_statement(&mut self) -> Option<Statement> {
        let mut selector = String::new();
        let mut prev_token_was_identifier = false;

        while !self.current_token_is(&Token::LBrace) {
            if self.current_token_is(&Token::Eof) {
                self.errors
                    .push("Unexpected EOF while parsing style rule selector".to_string());
                return None;
            }

            let (part, is_ident) =
                if let Some(res) = self.token_to_string_for_selector(&self.current_token) {
                    res
                } else {
                    self.errors.push(format!(
                        "Unexpected token in selector: {:?}",
                        self.current_token
                    ));
                    return None;
                };

            if prev_token_was_identifier && is_ident {
                selector.push(' ');
            }

            selector.push_str(&part);
            prev_token_was_identifier = is_ident;
            self.next_token();
        }

        self.next_token(); // consume LBrace

        let mut body = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(stmt) = self.parse_attribute_statement() {
                body.push(stmt);
            }
            self.next_token();
        }

        Some(Statement::StyleRule(StyleRuleStatement { selector, body }))
    }

    fn parse_conditional_expression(&mut self, condition: Expression) -> Option<Expression> {
        self.next_token(); // consume '?', current_token is now first token of consequence
        let consequence = self.parse_expression(Precedence::Conditional)?; // parse consequence

        let alternative = if self.peek_token_is(&Token::Colon) {
            self.next_token(); // consume last token of consequence, current_token is now ':'
            self.next_token(); // consume ':', current_token is now first token of alternative
            Some(self.parse_expression(Precedence::Conditional)?)
        } else {
            None
        };

        Some(Expression::Conditional(ConditionalExpression {
            condition: Box::new(condition),
            consequence: Box::new(consequence),
            alternative: alternative.map(Box::new),
        }))
    }

    fn parse_template_definition_statement(&mut self) -> Option<Statement> {
        // Current token is `[`, peek is `Template`
        self.expect_peek(&Token::Template)?; // Consume `[`, current is `Template`
        self.expect_peek(&Token::RBracket)?; // Consume `Template`, current is `]`
        self.expect_peek(&Token::At)?; // Consume `]`, current is `@`
        self.next_token(); // Consume `@`, current is type keyword

        let template_type = match self.current_token.clone() {
            Token::Style => TemplateType::Style,
            Token::Element => TemplateType::Element,
            Token::Var => TemplateType::Var,
            _ => {
                self.errors.push(format!(
                    "Expected template type (Style, Element, Var), got {:?}",
                    self.current_token
                ));
                return None;
            }
        };

        self.next_token(); // Consume type keyword, current is template name

        let name = if let Token::Identifier(n) = self.current_token.clone() {
            IdentifierExpression { value: n }
        } else {
            self.errors.push(format!(
                "Expected template name identifier, got {:?}",
                self.current_token
            ));
            return None;
        };

        self.expect_peek(&Token::LBrace)?; // Consume name, current is `{`
        self.next_token(); // Consume `{`

        let mut body = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                body.push(stmt);
            }
            self.next_token();
        }

        Some(Statement::TemplateDefinition(TemplateDefinitionStatement {
            name,
            template_type,
            body,
        }))
    }

    fn parse_use_template_statement(&mut self) -> Option<Statement> {
        // Current token is `@`
        self.next_token(); // Consume `@`, current is type keyword (e.g. Style)

        let template_type_str = match self.current_token.clone() {
            Token::Style => "Style",
            Token::Element => "Element",
            Token::Var => "Var",
            _ => {
                self.errors.push(format!(
                    "Expected template type keyword after @, got {:?}",
                    self.current_token
                ));
                return None;
            }
        };
        let template_type = IdentifierExpression {
            value: template_type_str.to_string(),
        };

        self.next_token(); // Consume type keyword, current is template name

        let name = if let Token::Identifier(n) = self.current_token.clone() {
            IdentifierExpression { value: n }
        } else {
            self.errors.push(format!(
                "Expected template name identifier, got {:?}",
                self.current_token
            ));
            return None;
        };

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::UseTemplate(UseTemplateStatement {
            name,
            template_type,
        }))
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
            Token::Text => IdentifierExpression { value: "text".to_string() },
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
            if let Some(s) = self.token_to_string_for_unquoted_literal(&self.current_token) {
                literal.push_str(&s);
            } else {
                break;
            }
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
            left_exp = if self.current_token_is(&Token::Question) {
                self.parse_conditional_expression(left_exp)?
            } else {
                self.parse_infix_expression(left_exp)?
            };
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
                    if let Some(s_val) = self.token_to_string_for_unquoted_literal(&current_tok) {
                        literal.push_str(&s_val);
                    } else {
                        break;
                    }

                    if self.is_peek_unquoted_literal_token() {
                        let is_current_number = matches!(current_tok, Token::Number(_));
                        let is_peek_identifier = matches!(self.peek_token, Token::Identifier(_));

                        self.next_token();
                        current_tok = self.current_token.clone();

                        if !(is_current_number && is_peek_identifier) {
                            literal.push(' ');
                        }
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
            Token::Gt => ">".to_string(),
            Token::Lt => "<".to_string(),
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
            Token::Gt | Token::Lt => Precedence::LessGreater,
            Token::Question => Precedence::Conditional,
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
        self.token_to_string_for_unquoted_literal(t).is_some()
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

    fn token_to_string_for_selector(&self, token: &Token) -> Option<(String, bool)> {
        match token.clone() {
            Token::Identifier(s) => Some((s, true)),
            Token::Dot => Some((".".to_string(), false)),
            Token::Hash => Some(("#".to_string(), false)),
            Token::Ampersand => Some(("&".to_string(), false)),
            Token::Colon => Some((":".to_string(), false)),
            Token::Before => Some(("before".to_string(), true)),
            _ => None,
        }
    }

    fn token_to_string_for_unquoted_literal(&self, token: &Token) -> Option<String> {
        match token.clone() {
            Token::Identifier(s) => Some(s),
            Token::Number(s) => Some(s),
            Token::Text => Some("text".to_string()),
            Token::Style => Some("style".to_string()),
            Token::Script => Some("script".to_string()),
            Token::Template => Some("template".to_string()),
            Token::Custom => Some("custom".to_string()),
            Token::Origin => Some("origin".to_string()),
            Token::Import => Some("import".to_string()),
            Token::Namespace => Some("namespace".to_string()),
            Token::Configuration => Some("configuration".to_string()),
            Token::Use => Some("use".to_string()),
            Token::If => Some("if".to_string()),
            Token::Else => Some("else".to_string()),
            Token::Except => Some("except".to_string()),
            Token::Inherit => Some("inherit".to_string()),
            Token::Delete => Some("delete".to_string()),
            Token::Insert => Some("insert".to_string()),
            Token::After => Some("after".to_string()),
            Token::Before => Some("before".to_string()),
            Token::Replace => Some("replace".to_string()),
            Token::AtTop => Some("at top".to_string()),
            Token::AtBottom => Some("at bottom".to_string()),
            Token::From => Some("from".to_string()),
            Token::As => Some("as".to_string()),
            _ => None,
        }
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

    #[test]
    fn test_conditional_expression() {
        let input = r#"
        style {
            background-color: width > 50px ? "red" : "blue";
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            let attr_stmt = &style_stmt.body[0];
            if let Statement::Attribute(attr) = attr_stmt {
                if let Expression::Conditional(cond_expr) = &attr.value {
                    // Check condition
                    if let Expression::Infix(infix) = &*cond_expr.condition {
                        assert_eq!(infix.operator, ">");
                    } else {
                        panic!("Expected InfixExpression for condition");
                    }

                    // Check consequence
                    if let Expression::StringLiteral(s) = &*cond_expr.consequence {
                        assert_eq!(s.value, "red");
                    } else {
                        panic!("Expected StringLiteral for consequence");
                    }

                    // Check alternative
                    if let Some(alt) = &cond_expr.alternative {
                        if let Expression::StringLiteral(s) = &**alt {
                            assert_eq!(s.value, "blue");
                        } else {
                            panic!("Expected StringLiteral for alternative");
                        }
                    } else {
                        panic!("Expected an alternative expression");
                    }
                } else {
                    panic!("Expected ConditionalExpression");
                }
            } else {
                panic!("Expected AttributeStatement");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }

    #[test]
    fn test_context_deduction_in_style() {
        let input = r#"
        style {
            .box {
                color: "blue";
            }
            &:hover {
                color: "red";
            }
            &::before {
                content: "";
            }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        assert_eq!(program.statements.len(), 1);
        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            assert_eq!(style_stmt.body.len(), 3);

            // Test &:hover
            let rule2 = &style_stmt.body[1];
            if let Statement::StyleRule(rule) = rule2 {
                assert_eq!(rule.selector, "&:hover");
            } else {
                panic!("Expected StyleRuleStatement for &:hover");
            }

            // Test &::before
            let rule3 = &style_stmt.body[2];
            if let Statement::StyleRule(rule) = rule3 {
                assert_eq!(rule.selector, "&::before");
            } else {
                panic!("Expected StyleRuleStatement for &::before");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }

    #[test]
    fn test_style_rule_statement() {
        let input = r#"
        style {
            .my-class {
                color: "red";
            }
            #my-id {
                font-size: 16px;
            }
            p {
                margin: 0;
            }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors.is_empty(),
            "Parser has errors: {:?}",
            parser.errors
        );

        assert_eq!(program.statements.len(), 1);
        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            assert_eq!(style_stmt.body.len(), 3);

            // Test .my-class
            let rule1 = &style_stmt.body[0];
            if let Statement::StyleRule(rule) = rule1 {
                assert_eq!(rule.selector, ".my-class");
                assert_eq!(rule.body.len(), 1);
                if let Statement::Attribute(attr) = &rule.body[0] {
                    assert_eq!(attr.name.value, "color");
                    if let Expression::StringLiteral(s) = &attr.value {
                        assert_eq!(s.value, "red");
                    } else {
                        panic!("Expected StringLiteral for color value");
                    }
                } else {
                    panic!("Expected AttributeStatement");
                }
            } else {
                panic!("Expected StyleRuleStatement for .my-class");
            }

            // Test #my-id
            let rule2 = &style_stmt.body[1];
            if let Statement::StyleRule(rule) = rule2 {
                assert_eq!(rule.selector, "#my-id");
                assert_eq!(rule.body.len(), 1);
                if let Statement::Attribute(attr) = &rule.body[0] {
                    assert_eq!(attr.name.value, "font-size");
                    if let Expression::UnquotedLiteral(s) = &attr.value {
                        assert_eq!(s.value, "16px");
                    } else {
                        panic!("Expected UnquotedLiteral for font-size value");
                    }
                } else {
                    panic!("Expected AttributeStatement");
                }
            } else {
                panic!("Expected StyleRuleStatement for #my-id");
            }

            // Test p
            let rule3 = &style_stmt.body[2];
            if let Statement::StyleRule(rule) = rule3 {
                assert_eq!(rule.selector, "p");
                assert_eq!(rule.body.len(), 1);
                if let Statement::Attribute(attr) = &rule.body[0] {
                    assert_eq!(attr.name.value, "margin");
                    if let Expression::UnquotedLiteral(s) = &attr.value {
                        assert_eq!(s.value, "0");
                    } else {
                        panic!("Expected UnquotedLiteral for margin value");
                    }
                } else {
                    panic!("Expected AttributeStatement");
                }
            } else {
                panic!("Expected StyleRuleStatement for p");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }

    #[test]
    fn test_template_parsing() {
        let input = r#"
        [Template] @Style DefaultText {
            color: "black";
            line-height: 1.6;
        }

        div {
            style {
                @Style DefaultText;
            }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        // Test TemplateDefinitionStatement
        let template_def_stmt = &program.statements[0];
        if let Statement::TemplateDefinition(def) = template_def_stmt {
            assert_eq!(def.name.value, "DefaultText");
            assert!(matches!(def.template_type, TemplateType::Style));
            assert_eq!(def.body.len(), 2);
        } else {
            panic!("Expected TemplateDefinitionStatement");
        }

        // Test UseTemplateStatement inside style block
        let div_stmt = &program.statements[1];
        if let Statement::Element(element) = div_stmt {
            let style_block = &element.body[0];
            if let Statement::Style(style) = style_block {
                let use_template_stmt = &style.body[0];
                if let Statement::UseTemplate(use_stmt) = use_template_stmt {
                    assert_eq!(use_stmt.name.value, "DefaultText");
                    assert_eq!(use_stmt.template_type.value, "Style");
                } else {
                    panic!("Expected UseTemplateStatement");
                }
            } else {
                panic!("Expected StyleStatement");
            }
        } else {
            panic!("Expected ElementStatement");
        }
    }
}