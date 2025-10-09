use crate::chtl::evaluator::evaluator::Evaluator;
use crate::chtl::evaluator::object::Object;
use crate::chtl::lexer::lexer::Lexer;
use crate::chtl::loader::Loader;
use crate::chtl::node::ast::*;
use crate::chtl::parser::parser::Parser;
use std::collections::HashMap;

type DocumentMap = HashMap<String, HashMap<String, Expression>>;

pub struct Generator {
    evaluator: Evaluator,
    global_css: String,
    templates: HashMap<String, TemplateDefinitionStatement>,
    loader: Loader,
    document_map: DocumentMap,
}

impl Generator {
    pub fn new() -> Self {
        Generator {
            evaluator: Evaluator::new(),
            global_css: String::new(),
            templates: HashMap::new(),
            loader: Loader::new(),
            document_map: HashMap::new(),
        }
    }

    fn apply_style_template(
        &self,
        template: &TemplateDefinitionStatement,
        context: &mut HashMap<String, Object>,
        inline_style_props: &mut HashMap<String, String>,
    ) {
        for stmt in &template.body {
            match stmt {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        let value_obj = self.evaluator.eval(expr, context, &self.templates, &self.document_map);
                        context.insert(attr.name.value.clone(), value_obj.clone());
                        inline_style_props.insert(attr.name.value.clone(), value_obj.to_string());
                    }
                }
                Statement::UseTemplate(use_stmt) => {
                    if use_stmt.template_type.value == "Style" {
                        if let Some(nested_template) =
                            self.templates.get(&use_stmt.name.value).cloned()
                        {
                            if matches!(nested_template.template_type, TemplateType::Style) {
                                self.apply_style_template(
                                    &nested_template,
                                    context,
                                    inline_style_props,
                                );
                            }
                        }
                    }
                }
                _ => (),
            }
        }
    }

    fn process_imports_and_templates(&mut self, program: &Program) {
        for statement in &program.statements {
            match statement {
                Statement::TemplateDefinition(def) => {
                    self.templates.insert(def.name.value.clone(), def.clone());
                }
                Statement::Import(import_stmt) => {
                    // For now, we only handle CHTL file imports.
                    if let ImportSpecifier::File(ImportFileType::Chtl) = &import_stmt.specifier {
                        let path = match &import_stmt.path {
                            Expression::StringLiteral(s) => s.value.clone(),
                            Expression::UnquotedLiteral(u) => u.value.clone(),
                            _ => {
                                eprintln!("Warning: Unsupported import path expression: {:?}", import_stmt.path);
                                continue;
                            }
                        };

                        match self.loader.load_file_content(&path) {
                            Ok(content) => {
                                let lexer = Lexer::new(&content);
                                let mut parser = Parser::new(lexer);
                                let imported_program = parser.parse_program();
                                if !parser.errors().is_empty() {
                                    eprintln!("Warning: Parsing errors in imported file '{}': {:?}", path, parser.errors());
                                }
                                // Recursively process the imported file.
                                self.process_imports_and_templates(&imported_program);
                            }
                            Err(e) => {
                                eprintln!("Warning: Failed to load imported file '{}': {}", path, e);
                            }
                        }
                    }
                }
                _ => {} // Ignore other statements in this pass.
            }
        }
    }
    fn collect_element_properties(&mut self, statements: &[Statement]) {
        for statement in statements {
            if let Statement::Element(element) = statement {
                let mut element_id = None;
                let mut properties = HashMap::new();

                // First pass to find the ID and collect attributes
                for stmt in &element.body {
                    if let Statement::Attribute(attr) = stmt {
                        if attr.name.value == "id" {
                            if let Some(expr) = &attr.value {
                                // For simplicity, we assume the ID is a string or unquoted literal.
                                match expr {
                                    Expression::StringLiteral(s) => {
                                        element_id = Some(s.value.clone());
                                    }
                                    Expression::UnquotedLiteral(u) => {
                                        element_id = Some(u.value.clone());
                                    }
                                    _ => {}
                                }
                            }
                        }
                        if let Some(value_expr) = &attr.value {
                            properties.insert(attr.name.value.clone(), value_expr.clone());
                        }
                    }
                }

                // Second pass for properties inside style blocks
                for stmt in &element.body {
                    if let Statement::Style(style_block) = stmt {
                        for style_stmt in &style_block.body {
                            if let Statement::Attribute(attr) = style_stmt {
                                if let Some(value_expr) = &attr.value {
                                    properties.insert(attr.name.value.clone(), value_expr.clone());
                                }
                            }
                        }
                    }
                }

                if let Some(id) = element_id {
                    self.document_map.insert(id, properties);
                }

                // Recurse into children
                self.collect_element_properties(&element.body);
            }
        }
    }

    pub fn generate(&mut self, program: &Program) -> String {
        // First pass: recursively process imports and collect templates.
        self.process_imports_and_templates(program);

        // New pass: collect all element properties for cross-element reference.
        self.collect_element_properties(&program.statements);

        // Second pass: generate the actual HTML from the main program.
        let mut html = String::new();
        for statement in &program.statements {
            // Don't generate output for top-level templates or imports.
            if !matches!(statement, Statement::TemplateDefinition(_) | Statement::Import(_)) {
                html.push_str(&self.generate_statement(statement));
            }
        }

        if !self.global_css.is_empty() {
            return format!("<style>{}</style>{}", self.global_css, html);
        }

        html
    }

    fn generate_statement(&mut self, statement: &Statement) -> String {
        match statement {
            Statement::Element(element) => self.generate_element(element),
            Statement::Text(text) => self.generate_text(text),
            Statement::Style(style) => {
                // For a global style block, there are no element attributes to modify.
                // We pass a dummy map and only the global_css will be affected.
                let mut dummy_attrs = std::collections::HashMap::new();
                self.generate_style(style, &mut dummy_attrs);
                String::new()
            }
            Statement::Comment(comment) => self.generate_comment(comment),
            Statement::Attribute(_) => String::new(), // Attributes are handled within elements
            Statement::StyleRule(_) => String::new(), // Style rules are handled within style blocks
            Statement::TemplateDefinition(_) => String::new(), // Handled at a higher level
            Statement::UseTemplate(use_stmt) => {
                if use_stmt.template_type.value == "Element" {
                    if let Some(template) = self.templates.get(&use_stmt.name.value).cloned() {
                        if matches!(template.template_type, TemplateType::Element) {
                            let mut html = String::new();
                            for stmt in &template.body {
                                html.push_str(&self.generate_statement(stmt));
                            }
                            return html;
                        }
                    }
                }
                // Non-element templates are handled within their respective blocks (e.g., style)
                String::new()
            }
            Statement::Import(_) => String::new(),
        }
    }

    fn generate_comment(&self, comment: &CommentStatement) -> String {
        format!("<!--{}-->", comment.value)
    }

    fn generate_element(&mut self, element: &ElementStatement) -> String {
        use std::collections::HashMap;
        let mut attributes: HashMap<String, String> = HashMap::new();
        let mut children = String::new();

        let mut style_statements = Vec::new();
        let mut other_statements = Vec::new();

        for statement in &element.body {
            if let Statement::Style(s) = statement {
                style_statements.push(s);
            } else {
                other_statements.push(statement);
            }
        }

        for statement in &other_statements {
            match statement {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        let context = std::collections::HashMap::new();
                        let value = self.eval_expression_to_string(expr, &context);
                        if attr.name.value == "text" {
                            children.push_str(&value);
                        } else {
                            attributes.insert(attr.name.value.clone(), value);
                        }
                    }
                }
                _ => {
                    children.push_str(&self.generate_statement(statement));
                }
            }
        }

        for style in style_statements {
            self.generate_style(style, &mut attributes);
        }

        let mut attrs_str = String::new();
        let mut sorted_attributes: Vec<_> = attributes.iter().collect();
        sorted_attributes.sort_by_key(|a| a.0);

        for (key, value) in sorted_attributes {
            attrs_str.push_str(&format!(r#" {}="{}""#, key, value));
        }

        format!(
            "<{_name}{_attrs}>{_children}</{_name}>",
            _name = element.name.value,
            _attrs = attrs_str,
            _children = children
        )
    }

    fn generate_style(
        &mut self,
        style: &StyleStatement,
        attributes: &mut std::collections::HashMap<String, String>,
    ) {
        let mut inline_style_props = std::collections::HashMap::new();
        let mut style_rules = Vec::new();
        let mut context = std::collections::HashMap::new();

        // 1. Separate inline properties and style rules
        for statement in &style.body {
            match statement {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        let value_obj = self.evaluator.eval(expr, &context, &self.templates, &self.document_map);
                        context.insert(attr.name.value.clone(), value_obj.clone());
                        inline_style_props.insert(attr.name.value.clone(), value_obj.to_string());
                    }
                }
                Statement::StyleRule(rule) => {
                    style_rules.push(rule);
                }
                Statement::UseTemplate(use_stmt) => {
                    if use_stmt.template_type.value == "Style" {
                        if let Some(template) = self.templates.get(&use_stmt.name.value).cloned() {
                            if matches!(template.template_type, TemplateType::Style) {
                                self.apply_style_template(
                                    &template,
                                    &mut context,
                                    &mut inline_style_props,
                                );
                            }
                        }
                    }
                }
                _ => (),
            }
        }

        // 2. Add classes/IDs from rules to the element's attributes
        for rule in &style_rules {
            if rule.selector.starts_with('.') {
                let class_name = &rule.selector[1..];
                attributes
                    .entry("class".to_string())
                    .and_modify(|e| {
                        if !e.split(' ').any(|c| c == class_name) {
                            e.push(' ');
                            e.push_str(class_name);
                        }
                    })
                    .or_insert_with(|| class_name.to_string());
            } else if rule.selector.starts_with('#') {
                let id_name = &rule.selector[1..];
                attributes
                    .entry("id".to_string())
                    .or_insert_with(|| id_name.to_string());
            }
        }

        // 3. Determine the context selector for '&' replacement (class > id)
        let context_selector = style_rules
            .iter()
            .find(|r| r.selector.starts_with('.'))
            .map(|r| r.selector.clone())
            .or_else(|| {
                style_rules
                    .iter()
                    .find(|r| r.selector.starts_with('#'))
                    .map(|r| r.selector.clone())
            })
            .or_else(|| {
                attributes
                    .get("class")
                    .map(|c| format!(".{}", c.split(' ').next().unwrap()))
            })
            .or_else(|| attributes.get("id").map(|i| format!("#{}", i)))
            .unwrap_or_default();

        // 4. Generate CSS for all rules and add to global_css
        for rule in &style_rules {
            let mut rule_css_body = String::new();
            let mut rule_context = context.clone();
            for prop in &rule.body {
                if let Statement::Attribute(attr_prop) = prop {
                    if let Some(expr) = &attr_prop.value {
                        let value_obj = self.evaluator.eval(
                            expr,
                            &rule_context,
                            &self.templates,
                            &self.document_map,
                        );
                        rule_context.insert(attr_prop.name.value.clone(), value_obj.clone());
                        rule_css_body.push_str(&format!(
                            "{}:{};",
                            attr_prop.name.value,
                            value_obj.to_string()
                        ));
                    }
                }
            }

            let final_selector = if rule.selector.starts_with('&') {
                rule.selector.replacen('&', &context_selector, 1)
            } else {
                rule.selector.clone()
            };

            self.global_css
                .push_str(&format!("{}{{{}}}{}", final_selector, rule_css_body, "\n"));
        }

        // 5. Add inline styles to the 'style' attribute
        if !inline_style_props.is_empty() {
            let mut sorted_props: Vec<_> = inline_style_props.iter().collect();
            sorted_props.sort_by_key(|a| a.0);

            let style_string = sorted_props
                .into_iter()
                .map(|(k, v)| format!("{}:{}", k, v))
                .collect::<Vec<String>>()
                .join(";");

            attributes
                .entry("style".to_string())
                .and_modify(|e| {
                    e.push(';');
                    e.push_str(&style_string);
                })
                .or_insert(style_string);
        }
    }

    fn generate_text(&self, text: &TextStatement) -> String {
        text.value.value.clone()
    }

    fn eval_expression_to_string(
        &self,
        expr: &Expression,
        context: &std::collections::HashMap<String, Object>,
    ) -> String {
        let value_obj = self.evaluator.eval(expr, context, &self.templates, &self.document_map);
        match value_obj {
            Object::Error(e) => {
                eprintln!("Evaluation Error: {}", e);
                "".to_string()
            }
            _ => value_obj.to_string(),
        }
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
        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div class="container" id="box">Hello, CHTL!</div>"#;
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

        let mut generator = Generator::new();
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div class="my-class another-class">This is unquoted text</div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_property_arithmetic() {
        let input = r#"
        div {
            style {
                width: 100px + 50;
                height: 200.5em - 0.5em;
            }
        }
        "#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser errors: {:?}", parser.errors());

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div style="height:200em;width:150px"></div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_style_rules() {
        let input = r#"
        div {
            style {
                color: "blue";
                .my-class {
                    font-weight: "bold";
                }
            }
            text { "Hello" }
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"
        <style>
            .my-class {
                font-weight:bold;
            }
        </style>
        <div class="my-class" style="color:blue">Hello</div>
        "#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_context_deduction() {
        let input = r#"
        div {
            style {
                .box {
                    color: "blue";
                }
                &:hover {
                    color: "red";
                }
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_css = r#"
        .box{color:blue;}
        .box:hover{color:red;}
        "#;
        let expected_html = r#"<div class="box"></div>"#;

        let combined_expected = format!("<style>{}</style>{}", expected_css.replace_whitespace(), expected_html.replace_whitespace());

        assert_eq!(html.replace_whitespace(), combined_expected);
    }

    #[test]
    fn test_generate_with_conditional_property() {
        let input = r#"
        div {
            style {
                width: 100px;
                background-color: width > 50px ? "red" : "blue";
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div style="background-color:red;width:100px"></div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_style_template() {
        let input = r#"
        [Template] @Style DefaultText {
            color: "black";
            line-height: 1.6;
        }

        div {
            style {
                @Style DefaultText;
                font-size: 16px;
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div style="color:black;font-size:16px;line-height:1.6"></div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_element_template() {
        let input = r#"
        [Template] @Element Box {
            div {
                text { "This is a box." }
            }
        }

        body {
            @Element Box;
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<body><div>This is a box.</div></body>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_template_inheritance() {
        let input = r#"
        [Template] @Style Base {
            font-family: "Arial";
        }

        [Template] @Style Derived {
            @Style Base;
            color: "red";
        }

        div {
            style {
                @Style Derived;
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div style="color:red;font-family:Arial"></div>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_import() {
        use std::fs::File;
        use std::io::Write;
        use tempfile::tempdir;

        let dir = tempdir().unwrap();

        // Create the imported file with a template definition
        let template_file_path = dir.path().join("templates.chtl");
        let mut template_file = File::create(&template_file_path).unwrap();
        writeln!(
            template_file,
            r#"[Template] @Element MyCard {{
                div {{
                    class: "card";
                    text: "This is a card from an imported template.";
                }}
            }}"#
        )
        .unwrap();

        // Create the main file that imports and uses the template
        let main_file_content = format!(
            r#"[Import] @Chtl from "{}";
            body {{
                @Element MyCard;
            }}"#,
            template_file_path.to_str().unwrap()
        );

        let lexer = Lexer::new(&main_file_content);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<body><div class="card">This is a card from an imported template.</div></body>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }

    #[test]
    fn test_generate_with_property_reference() {
        let input = r#"
        div {
            id: "box";
            style {
                width: 100px;
            }
        }
        span {
            style {
                height: #box.width;
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

        let mut generator = Generator::new();
        let html = generator.generate(&program);

        let expected_html = r#"<div id="box" style="width:100px"></div><span style="height:100px"></span>"#;
        assert_eq!(html.replace_whitespace(), expected_html.replace_whitespace());
    }
}