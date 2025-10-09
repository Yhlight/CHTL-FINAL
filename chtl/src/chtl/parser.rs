use crate::chtl::ast::{Element, ElementTemplateDefinition, Origin, Program, Script, Statement, Style, StyleItem, StyleTemplateDefinition, Text};
use crate::chtl::lexer::token::Token;
use crate::chtl::lexer::Lexer;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::EOF,
            peek_token: Token::EOF,
            errors: Vec::new(),
        };
        parser.next_token();
        parser.next_token();
        parser
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program { body: Vec::new() };

        while self.current_token != Token::EOF {
            match self.parse_statement() {
                Some(statement) => program.body.push(statement),
                None => {}
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match &self.current_token {
            Token::Identifier(_) => self.parse_element_statement(),
            Token::Text => self.parse_text_statement(),
            Token::Style => self.parse_style_statement(),
            Token::Script => self.parse_script_statement(),
            Token::GeneratorComment(comment) => Some(Statement::GeneratorComment(comment.clone())),
            Token::LBracket => {
                match self.peek_token {
                    Token::Origin => self.parse_origin_statement(),
                    Token::Template => self.parse_template_statement(),
                    _ => {
                        self.errors.push(format!("Unexpected token after '[': {:?}", self.peek_token));
                        None
                    }
                }
            },
            Token::At => {
                match self.peek_token.clone() {
                    Token::Element => {
                        self.next_token(); // consume @, current is Element
                        self.next_token(); // consume Element, current is template name
                        let name = if let Token::Identifier(name) = self.current_token.clone() {
                            name
                        } else {
                            self.errors.push(format!("Expected element template name, got {:?}", self.current_token));
                            return None;
                        };
                        if !self.expect_peek(Token::Semicolon) { return None; }
                        Some(Statement::ElementTemplateUsage(name))
                    },
                    _ => {
                        self.errors.push(format!("Unsupported @-rule: {:?}", self.peek_token));
                        None
                    }
                }
            },
            _ => None,
        }
    }

    fn parse_element_statement(&mut self) -> Option<Statement> {
        let tag = if let Token::Identifier(tag) = &self.current_token {
            tag.clone()
        } else {
            return None;
        };

        if !self.expect_peek(Token::LBrace) {
            return None;
        }

        self.next_token(); // Consume LBrace

        let mut attributes = Vec::new();
        let mut body = Vec::new();

        while self.current_token != Token::RBrace && self.current_token != Token::EOF {
            if let Token::Identifier(_) = &self.current_token {
                if self.peek_token == Token::Colon {
                    if let Some(attr) = self.parse_attribute() {
                        attributes.push(attr);
                    }
                } else if let Some(statement) = self.parse_statement() {
                    body.push(statement);
                }
            } else if let Some(statement) = self.parse_statement() {
                body.push(statement);
            }
            self.next_token();
        }

        if self.current_token != Token::RBrace {
             self.errors.push(format!("Expected RBrace, got {:?}", self.current_token));
             return None;
        }

        Some(Statement::Element(Element { tag, attributes, body }))
    }

    fn parse_script_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::LBrace) {
            return None;
        }

        let content = if let Token::String(s) = self.peek_token.clone() {
            s
        } else {
            self.errors.push(format!("Expected String for script content, got {:?}", self.peek_token));
            return None;
        };
        self.next_token(); // Consume string

        if !self.expect_peek(Token::RBrace) {
            return None;
        }

        Some(Statement::Script(Script { content }))
    }

    fn parse_origin_statement(&mut self) -> Option<Statement> {
        // Current token is LBracket
        if !self.expect_peek(Token::Origin) { return None; } // current: Origin, peek: ]
        if !self.expect_peek(Token::RBracket) { return None; } // current: ], peek: @
        if !self.expect_peek(Token::At) { return None; } // current: @, peek: Html

        self.next_token(); // current: Html, peek: {

        let origin_type = if let Token::Identifier(t) = self.current_token.clone() {
            t
        } else {
            self.errors.push(format!("Expected origin type identifier, got {:?}", self.current_token));
            return None;
        };

        if self.peek_token != Token::LBrace {
            self.errors.push(format!("Expected LBrace, got {:?}", self.peek_token));
            return None;
        }

        // Manually advance current_token to LBrace without advancing peek_token
        self.current_token = self.peek_token.clone();

        // The lexer is positioned right after producing the LBrace token.
        // Its internal `ch` is at the start of the raw content.
        let content = self.lexer.read_raw_block_content();

        // After reading the raw block, the lexer is positioned after the final `}`.
        // We need to resynchronize the parser's token stream.
        self.current_token = self.lexer.next_token();
        self.peek_token = self.lexer.next_token();

        Some(Statement::Origin(Origin { origin_type, content }))
    }

    fn parse_template_statement(&mut self) -> Option<Statement> {
        // Assumes current token is LBracket
        if !self.expect_peek(Token::Template) { return None; }
        if !self.expect_peek(Token::RBracket) { return None; }
        if !self.expect_peek(Token::At) { return None; }

        // current token is now At, peek is the type (Style, Element, etc.)
        match self.peek_token.clone() {
            Token::Style => {
                self.next_token(); // consume At, current is Style
                self.next_token(); // consume Style, current is template name

                let name = if let Token::Identifier(name) = self.current_token.clone() {
                    name
                } else {
                    self.errors.push(format!("Expected template name, got {:?}", self.current_token));
                    return None;
                };

                if !self.expect_peek(Token::LBrace) { return None; }
                self.next_token(); // Consume LBrace

                let mut properties = Vec::new();
                while self.current_token != Token::RBrace && self.current_token != Token::EOF {
                    if let Some(prop) = self.parse_attribute() {
                        properties.push(prop);
                    }
                    self.next_token();
                }

                if self.current_token != Token::RBrace {
                    self.errors.push(format!("Expected RBrace, got {:?}", self.current_token));
                    return None;
                }

                Some(Statement::StyleTemplateDefinition(StyleTemplateDefinition { name, properties }))
            },
             Token::Element => {
                self.next_token(); // consume At
                self.next_token(); // consume Element

                let name = if let Token::Identifier(name) = self.current_token.clone() {
                    name
                } else {
                    self.errors.push(format!("Expected element template name, got {:?}", self.current_token));
                    return None;
                };

                if !self.expect_peek(Token::LBrace) { return None; }
                self.next_token(); // consume LBrace

                let mut body = Vec::new();
                while self.current_token != Token::RBrace && self.current_token != Token::EOF {
                    if let Some(statement) = self.parse_statement() {
                        body.push(statement);
                    }
                    self.next_token();
                }

                if self.current_token != Token::RBrace {
                    self.errors.push(format!("Expected RBrace for element template, got {:?}", self.current_token));
                    return None;
                }

                Some(Statement::ElementTemplateDefinition(ElementTemplateDefinition { name, body }))
            },
            _ => {
                self.errors.push(format!("Unsupported template type: {:?}", self.peek_token));
                None
            }
        }
    }

    fn parse_style_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::LBrace) {
            return None;
        }

        let mut items = Vec::new();
        while self.peek_token != Token::RBrace && self.peek_token != Token::EOF {
            self.next_token(); // Consume token for the start of the item

            match self.current_token.clone() {
                Token::Identifier(_) => {
                    if let Some(prop) = self.parse_attribute() {
                        items.push(StyleItem::Property(prop));
                    } else {
                        return None; // Error in attribute parsing
                    }
                }
                Token::At => {
                    if !self.expect_peek(Token::Style) { return None; } // current is Style
                    self.next_token(); // current is template name
                    let name = if let Token::Identifier(name) = self.current_token.clone() {
                        name
                    } else {
                        self.errors.push(format!("Expected template name, got {:?}", self.current_token));
                        return None;
                    };
                    if !self.expect_peek(Token::Semicolon) { return None; }
                    items.push(StyleItem::TemplateUsage(name));
                }
                _ => { /* Ignore other tokens */ }
            }
        }

        if !self.expect_peek(Token::RBrace) {
            self.errors.push(format!("Expected closing RBrace for style block, got {:?}", self.peek_token));
            return None;
        }

        Some(Statement::Style(Style { items }))
    }

    fn parse_text_statement(&mut self) -> Option<Statement> {
        // current_token is Token::Text
        if !self.expect_peek(Token::LBrace) { // current_token is now LBrace
            return None;
        }

        // current_token is LBrace. The string value is in peek_token.
        let value = if let Token::String(s) = self.peek_token.clone() {
            s
        } else {
            self.errors.push(format!("Expected String, got {:?}", self.peek_token));
            return None;
        };
        self.next_token(); // Consume string, current_token is now the string.

        if !self.expect_peek(Token::RBrace) { // current_token is now RBrace
            return None;
        }

        Some(Statement::Text(Text { value }))
    }

    fn parse_attribute(&mut self) -> Option<(String, String)> {
        let name = if let Token::Identifier(name) = &self.current_token {
            name.clone()
        } else {
            self.errors.push("Expected attribute name.".to_string());
            return None;
        };

        if !self.expect_peek(Token::Colon) {
            return None;
        }

        self.next_token();

        let value = match &self.current_token {
            Token::String(s) => s.clone(),
            Token::Identifier(s) => s.clone(),
            _ => {
                self.errors.push(format!("Expected attribute value, got {:?}", self.current_token));
                return None;
            }
        };

        if !self.expect_peek(Token::Semicolon) {
            return None;
        }

        Some((name, value))
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if self.peek_token == token {
            self.next_token();
            true
        } else {
            self.errors.push(format!(
                "Expected next token to be {:?}, got {:?} instead",
                token, self.peek_token
            ));
            false
        }
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::ast::{Element, Statement, Text};
    use crate::chtl::lexer::Lexer;

    #[test]
    fn test_parse_program_with_attributes() {
        let input = r#"
            div {
                id: "main";
                text { "Hello" }
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::Element(Element {
            tag: "div".to_string(),
            attributes: vec![("id".to_string(), "main".to_string())],
            body: vec![Statement::Text(Text {
                value: "Hello".to_string(),
            })],
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_with_style_block() {
        let input = r#"
            div {
                style {
                    color: "red";
                    font-size: "16px";
                }
                text { "Styled text" }
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::Element(Element {
            tag: "div".to_string(),
            attributes: vec![],
            body: vec![
                Statement::Style(Style {
                    items: vec![
                        StyleItem::Property(("color".to_string(), "red".to_string())),
                        StyleItem::Property(("font-size".to_string(), "16px".to_string())),
                    ],
                }),
                Statement::Text(Text {
                    value: "Styled text".to_string(),
                }),
            ],
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_with_script_block() {
        let input = r#"
            div {
                script { "console.log('hello');" }
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::Element(Element {
            tag: "div".to_string(),
            attributes: vec![],
            body: vec![
                Statement::Script(Script {
                    content: "console.log('hello');".to_string(),
                }),
            ],
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_generator_comment() {
        let input = r#"
            # This is a generator comment
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::GeneratorComment("This is a generator comment".to_string());

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_origin_block() {
        let input = r#"
            [Origin] @Html { <p>Raw HTML</p> }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::Origin(Origin {
            origin_type: "Html".to_string(),
            content: " <p>Raw HTML</p> ".to_string(),
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_style_template_definition() {
        let input = r#"
            [Template] @Style MyTemplate {
                color: "blue";
                font-weight: "bold";
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::StyleTemplateDefinition(StyleTemplateDefinition {
            name: "MyTemplate".to_string(),
            properties: vec![
                ("color".to_string(), "blue".to_string()),
                ("font-weight".to_string(), "bold".to_string()),
            ],
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_style_template_usage() {
        let input = r#"
            div {
                style {
                    @Style MyTemplate;
                }
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::Element(Element {
            tag: "div".to_string(),
            attributes: vec![],
            body: vec![
                Statement::Style(Style {
                    items: vec![
                        StyleItem::TemplateUsage("MyTemplate".to_string())
                    ],
                }),
            ],
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_element_template_definition() {
        let input = r#"
            [Template] @Element MyBox {
                div {
                    text { "I am in a box." }
                }
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::ElementTemplateDefinition(ElementTemplateDefinition {
            name: "MyBox".to_string(),
            body: vec![
                Statement::Element(Element {
                    tag: "div".to_string(),
                    attributes: vec![],
                    body: vec![
                        Statement::Text(Text { value: "I am in a box.".to_string() })
                    ],
                })
            ],
        });

        assert_eq!(program.body[0], expected_statement);
    }

    #[test]
    fn test_parse_element_template_usage() {
        let input = r#"
            body {
                @Element MyBox;
            }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser has errors: {:?}", parser.errors());
        assert_eq!(program.body.len(), 1);

        let expected_statement = Statement::Element(Element {
            tag: "body".to_string(),
            attributes: vec![],
            body: vec![
                Statement::ElementTemplateUsage("MyBox".to_string()),
            ],
        });

        assert_eq!(program.body[0], expected_statement);
    }
}