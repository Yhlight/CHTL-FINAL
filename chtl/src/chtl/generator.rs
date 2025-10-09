use crate::chtl::ast::{
    Element, ElementTemplateDefinition, Node, Origin, Program, Script, Statement, StyleItem,
    StyleTemplateDefinition, Text,
};
use std::collections::HashMap;

pub struct Generator<'a> {
    style_templates: HashMap<String, &'a StyleTemplateDefinition>,
    element_templates: HashMap<String, &'a ElementTemplateDefinition>,
}

impl<'a> Generator<'a> {
    pub fn new() -> Self {
        Generator {
            style_templates: HashMap::new(),
            element_templates: HashMap::new(),
        }
    }

    fn build_template_map(&mut self, program: &'a Program) {
        for statement in &program.body {
            match statement {
                Statement::StyleTemplateDefinition(template) => {
                    self.style_templates
                        .insert(template.name.clone(), template);
                }
                Statement::ElementTemplateDefinition(template) => {
                    self.element_templates
                        .insert(template.name.clone(), template);
                }
                _ => {}
            }
        }
    }

    pub fn generate(&mut self, node: &'a Node) -> String {
        match node {
            Node::Program(program) => {
                self.build_template_map(program);
                self.generate_program(program)
            }
            _ => "".to_string(),
        }
    }

    fn generate_program(&self, program: &Program) -> String {
        let mut output = String::new();
        for statement in &program.body {
            output.push_str(&self.generate_statement(statement));
        }
        output
    }

    fn generate_statement(&self, statement: &Statement) -> String {
        match statement {
            Statement::Element(element) => self.generate_element(element),
            Statement::Text(text) => self.generate_text(text),
            Statement::StyleTemplateDefinition(_) => "".to_string(),
            Statement::ElementTemplateDefinition(_) => "".to_string(),
            Statement::Style(_) => "".to_string(),
            Statement::StyleTemplateUsage(_) => "".to_string(),
            Statement::ElementTemplateUsage(name) => {
                if let Some(template) = self.element_templates.get(name) {
                    template
                        .body
                        .iter()
                        .map(|stmt| self.generate_statement(stmt))
                        .collect()
                } else {
                    "".to_string() // Or some error indication
                }
            }
            Statement::Script(script) => self.generate_script(script),
            Statement::GeneratorComment(comment) => format!("<!-- {} -->", comment),
            Statement::Origin(origin) => self.generate_origin(origin),
        }
    }

    fn generate_origin(&self, origin: &Origin) -> String {
        origin.content.clone()
    }

    fn generate_script(&self, script: &Script) -> String {
        format!("<script>{}</script>", script.content)
    }

    fn generate_element(&self, element: &Element) -> String {
        let mut output = String::new();
        output.push_str("<");
        output.push_str(&element.tag);

        for (key, value) in &element.attributes {
            output.push_str(&format!(" {}=\"{}\"", key, value));
        }

        let mut style_properties: Vec<String> = Vec::new();

        let style_items: Vec<&StyleItem> = element
            .body
            .iter()
            .filter_map(|stmt| {
                if let Statement::Style(style) = stmt {
                    Some(&style.items)
                } else {
                    None
                }
            })
            .flatten()
            .collect();

        for item in style_items {
            match item {
                StyleItem::Property((key, value)) => {
                    style_properties.push(format!("{}: {};", key, value));
                }
                StyleItem::TemplateUsage(name) => {
                    if let Some(template) = self.style_templates.get(name) {
                        for (key, value) in &template.properties {
                            style_properties.push(format!("{}: {};", key, value));
                        }
                    }
                }
            }
        }

        if !style_properties.is_empty() {
            output.push_str(" style=\"");
            output.push_str(&style_properties.join(" "));
            output.push_str("\"");
        }

        output.push_str(">");

        for statement in &element.body {
            // Don't render style blocks as content, they are processed for the style attribute
            if let Statement::Style(_) = statement {
            } else {
                output.push_str(&self.generate_statement(statement));
            }
        }
        output.push_str(&format!("</{}>", element.tag));
        output
    }

    fn generate_text(&self, text: &Text) -> String {
        text.value.clone()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::ast::{Element, Node, Origin, Program, Script, Statement, Style, StyleItem, StyleTemplateDefinition, Text};

    #[test]
    fn test_generate_html_with_attributes() {
        let program = Program {
            body: vec![Statement::Element(Element {
                tag: "div".to_string(),
                attributes: vec![("id".to_string(), "main".to_string())],
                body: vec![Statement::Text(Text {
                    value: "Hello".to_string(),
                })],
            })],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<div id=\"main\">Hello</div>");
    }

    #[test]
    fn test_generate_html_with_style_block() {
        let program = Program {
            body: vec![Statement::Element(Element {
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
                        value: "Styled".to_string(),
                    }),
                ],
            })],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<div style=\"color: red; font-size: 16px;\">Styled</div>");
    }

    #[test]
    fn test_generate_html_with_script_block() {
        let program = Program {
            body: vec![Statement::Element(Element {
                tag: "div".to_string(),
                attributes: vec![],
                body: vec![
                    Statement::Script(Script {
                        content: "console.log('hello');".to_string(),
                    }),
                ],
            })],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<div><script>console.log('hello');</script></div>");
    }

    #[test]
    fn test_generate_html_with_generator_comment() {
        let program = Program {
            body: vec![Statement::GeneratorComment(
                "This is a comment".to_string()
            )],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<!-- This is a comment -->");
    }

    #[test]
    fn test_generate_html_with_origin_block() {
        let program = Program {
            body: vec![Statement::Origin(Origin {
                origin_type: "Html".to_string(),
                content: "<div><p>Raw HTML</p></div>".to_string(),
            })],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<div><p>Raw HTML</p></div>");
    }

    #[test]
    fn test_generate_html_with_style_template() {
        let program = Program {
            body: vec![
                Statement::StyleTemplateDefinition(StyleTemplateDefinition {
                    name: "MyTemplate".to_string(),
                    properties: vec![
                        ("color".to_string(), "red".to_string()),
                    ],
                }),
                Statement::Element(Element {
                    tag: "div".to_string(),
                    attributes: vec![],
                    body: vec![
                        Statement::Style(Style {
                            items: vec![
                                StyleItem::TemplateUsage("MyTemplate".to_string()),
                            ],
                        }),
                    ],
                }),
            ],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<div style=\"color: red;\"></div>");
    }

    #[test]
    fn test_generate_html_with_element_template() {
        let program = Program {
            body: vec![
                Statement::ElementTemplateDefinition(ElementTemplateDefinition {
                    name: "MyBox".to_string(),
                    body: vec![Statement::Element(Element {
                        tag: "p".to_string(),
                        attributes: vec![],
                        body: vec![Statement::Text(Text {
                            value: "Template content".to_string(),
                        })],
                    })],
                }),
                Statement::Element(Element {
                    tag: "div".to_string(),
                    attributes: vec![],
                    body: vec![Statement::ElementTemplateUsage("MyBox".to_string())],
                }),
            ],
        };

        let node = Node::Program(program);
        let mut generator = Generator::new();
        let html = generator.generate(&node);

        assert_eq!(html, "<div><p>Template content</p></div>");
    }
}