use crate::CHTL::CHTLNode::ast::*;

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
            Statement::Attribute(attribute) => self.generate_attribute(attribute),
        }
    }

    fn generate_element(&self, element: &ElementStatement) -> String {
        let mut attributes = String::new();
        let mut children = String::new();

        for statement in &element.body {
            match statement {
                Statement::Attribute(attr) => {
                    attributes.push_str(&self.generate_attribute(attr));
                }
                _ => {
                    children.push_str(&self.generate_statement(statement));
                }
            }
        }

        format!("<{_name}{_attrs}>{_children}</{_name}>", _name = element.name.value, _attrs = attributes, _children = children)
    }

    fn generate_text(&self, text: &TextStatement) -> String {
        text.value.value.clone()
    }

    fn generate_attribute(&self, attribute: &AttributeStatement) -> String {
        let value = match &attribute.value {
            Expression::Identifier(ident) => ident.value.clone(),
            Expression::StringLiteral(s) => s.value.clone(),
            Expression::NumberLiteral(n) => n.value.to_string(),
        };
        format!(r#" {_name}="{_value}""#, _name = attribute.name.value, _value = value)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::CHTL::CHTLLexer::lexer::Lexer;
    use crate::CHTL::CHTLParser::parser::Parser;

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
}

trait StringExt {
    fn replace_whitespace(&self) -> String;
}

impl StringExt for String {
    fn replace_whitespace(&self) -> String {
        self.split_whitespace().collect()
    }
}

impl StringExt for &str {
    fn replace_whitespace(&self) -> String {
        self.split_whitespace().collect()
    }
}