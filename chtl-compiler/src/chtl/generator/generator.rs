use crate::chtl::node::ast::*;

pub struct Generator {}

impl Generator {
    pub fn new() -> Self {
        Generator {}
    }

    pub fn generate(&self, program: &Program) -> String {
        let mut html = String::new();
        for statement in &program.statements {
            html.push_str(&self.generate_statement(statement));
        }
        html
    }

    fn generate_statement(&self, statement: &Statement) -> String {
        match statement {
            Statement::Element(element) => self.generate_element(element),
            Statement::Text(text) => self.generate_text(text),
            Statement::Style(style) => self.generate_style(style),
            Statement::Comment(comment) => self.generate_comment(comment),
            Statement::Attribute(_) => String::new(), // Attributes are handled within elements
        }
    }

    fn generate_comment(&self, comment: &CommentStatement) -> String {
        format!("<!--{}-->", comment.value)
    }

    fn generate_element(&self, element: &ElementStatement) -> String {
        let mut attributes = String::new();
        let mut children = String::new();
        let mut style_str = String::new();

        for statement in &element.body {
            match statement {
                Statement::Attribute(attr) => {
                    if attr.name.value == "text" {
                        let text_value = match &attr.value {
                            Expression::Identifier(ident) => ident.value.clone(),
                            Expression::StringLiteral(s) => s.value.clone(),
                            Expression::NumberLiteral(n) => n.value.clone(),
                            Expression::UnquotedLiteral(u) => u.value.clone(),
                        };
                        children.push_str(&text_value);
                    } else {
                        attributes.push_str(&self.generate_attribute(attr));
                    }
                }
                Statement::Style(style) => {
                    style_str = self.generate_style(style);
                }
                _ => {
                    children.push_str(&self.generate_statement(statement));
                }
            }
        }

        if !style_str.is_empty() {
             attributes.push_str(&format!(" style=\"{}\"", style_str));
        }

        format!("<{_name}{_attrs}>{_children}</{_name}>",
                _name = element.name.value,
                _attrs = attributes,
                _children = children)
    }

    fn generate_style(&self, style: &StyleStatement) -> String {
        let mut style_props = Vec::new();
        for statement in &style.body {
            if let Statement::Attribute(attr) = statement {
                 let value = match &attr.value {
                    Expression::Identifier(ident) => ident.value.clone(),
                    Expression::StringLiteral(s) => s.value.clone(),
                    Expression::NumberLiteral(n) => n.value.clone(),
                    Expression::UnquotedLiteral(u) => u.value.clone(),
                };
                style_props.push(format!("{}:{}", attr.name.value, value));
            }
        }
        style_props.join(";")
    }

    fn generate_text(&self, text: &TextStatement) -> String {
        text.value.value.clone()
    }

    fn generate_attribute(&self, attribute: &AttributeStatement) -> String {
        let value = match &attribute.value {
            Expression::Identifier(ident) => ident.value.clone(),
            Expression::StringLiteral(s) => s.value.clone(),
            Expression::NumberLiteral(n) => n.value.clone(),
            Expression::UnquotedLiteral(u) => u.value.clone(),
        };
        format!(r#" {_name}="{_value}""#, _name = attribute.name.value, _value = value)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::lexer::lexer::Lexer;
    use crate::chtl::parser::parser::Parser;

    trait StringExt {
        fn replace_whitespace(&self) -> String;
    }

    impl StringExt for String {
        fn replace_whitespace(&self) -> String {
            self.split_whitespace().collect()
        }
    }

    impl<'a> StringExt for &'a str {
        fn replace_whitespace(&self) -> String {
            self.split_whitespace().collect()
        }
    }

    #[test]
    fn test_generate() {
        let input = r#"
        div {
            id: "box";
            class: "container";
            text { "Hello, CHTL!" }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div id="box" class="container">Hello, CHTL!</div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_text_attribute_and_comment() {
        let input = r#"
        # This is a comment that should appear in the HTML
        p {
            text: "This is a paragraph from a text attribute.";
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser errors: {:?}", parser.errors());

        let generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<!-- This is a comment that should appear in the HTML--><p>This is a paragraph from a text attribute.</p>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_unquoted_literals() {
        let input = r#"
        div {
            class: my-class another-class;
            text { This is unquoted text }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser errors: {:?}", parser.errors());

        let generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div class="my-class another-class">This is unquoted text</div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }
}