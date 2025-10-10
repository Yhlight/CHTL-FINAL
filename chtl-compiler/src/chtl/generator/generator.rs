use crate::chtl::evaluator::evaluator::Evaluator;
use crate::chtl::evaluator::object::Object;
use crate::chtl::loader::Loader;
use crate::chtl::node::ast::{
    CommentStatement, ElementStatement, Expression, IfStatement, ImportFileType, ImportSpecifier,
    Program, Statement, StyleStatement, TemplateDefinitionStatement, TemplateType, TextStatement,
};
use crate::chtl::lexer::lexer::Lexer;
use crate::chtl::parser::parser::Parser;
use std::collections::HashMap;

pub struct Generator {
    pub evaluator: Evaluator,
    pub global_css: String,
    pub templates: HashMap<String, HashMap<String, TemplateDefinitionStatement>>,
    pub loader: Loader,
    pub document_map: HashMap<String, HashMap<String, Expression>>,
    pub current_namespace: String,
}

impl Generator {
    pub fn new() -> Self {
        Generator {
            evaluator: Evaluator::new(),
            global_css: String::new(),
            templates: HashMap::new(),
            loader: Loader::new(),
            document_map: HashMap::new(),
            current_namespace: "::default".to_string(),
        }
    }

    fn apply_style_template(
        &mut self,
        template: &TemplateDefinitionStatement,
        template_namespace: &str,
        context: &mut HashMap<String, Object>,
        inline_style_props: &mut HashMap<String, String>,
        specialization_body: &Option<Vec<Statement>>,
    ) {
        let mut applied_properties = HashMap::new();

        // First, apply properties from the base template
        for stmt in &template.body {
            match stmt {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        let all_templates: HashMap<_, _> = self
                            .templates
                            .values()
                            .flat_map(|m| m.iter())
                            .map(|(k, v)| (k.clone(), v.clone()))
                            .collect();
                        let value_obj = self.evaluator.eval(
                            expr,
                            context,
                            &all_templates,
                            &self.document_map,
                        );
                        applied_properties.insert(attr.name.value.clone(), value_obj);
                    }
                }
                Statement::UseTemplate(use_stmt) => {
                    if use_stmt.template_type.value == "Style" {
                        // Nested template usage. Resolve it within the parent's namespace
                        // unless a 'from' clause is specified.
                        let namespace_key =
                            use_stmt.from.as_ref().map_or(template_namespace, |f| &f.value);
                        if let Some(templates_in_ns) = self.templates.get(namespace_key) {
                            if let Some(nested_template) =
                                templates_in_ns.get(&use_stmt.name.value).cloned()
                            {
                                if matches!(nested_template.template_type, TemplateType::Style) {
                                    // Pass the namespace of the resolved template, and &None for specialization body.
                                    self.apply_style_template(
                                        &nested_template,
                                        namespace_key,
                                        context,
                                        inline_style_props,
                                        &None,
                                    );
                                }
                            }
                        }
                    }
                }
                _ => (),
            }
        }

        // Apply specialized properties and deletions from the call site
        if let Some(body) = specialization_body {
            for stmt in body {
                match stmt {
                    Statement::Attribute(attr) => {
                        if let Some(expr) = &attr.value {
                            let all_templates: HashMap<_, _> = self
                                .templates
                                .values()
                                .flat_map(|m| m.iter())
                                .map(|(k, v)| (k.clone(), v.clone()))
                                .collect();
                            let value_obj = self.evaluator.eval(
                                expr,
                                context,
                                &all_templates,
                                &self.document_map,
                            );
                            applied_properties.insert(attr.name.value.clone(), value_obj);
                        }
                    }
                    Statement::Delete(del_stmt) => {
                        for target in &del_stmt.targets {
                            if let Expression::Identifier(ident) = target {
                                applied_properties.remove(&ident.value);
                            } else if let Expression::UnquotedLiteral(lit) = target {
                                // Handle cases like `delete @Style Base`
                                if lit.value.starts_with("@Style") {
                                    let template_name = lit.value.split_whitespace().last().unwrap();
                                    // This is a simplification. A more robust solution would
                                    // track which properties came from which template.
                                    // For now, we search all namespaces.
                                    for templates_in_ns in self.templates.values() {
                                        if let Some(template_to_delete) =
                                            templates_in_ns.get(template_name)
                                        {
                                            for t_stmt in &template_to_delete.body {
                                                if let Statement::Attribute(t_attr) = t_stmt {
                                                    applied_properties.remove(&t_attr.name.value);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    _ => {}
                }
            }
        }

        // Finally, add all processed properties to the inline style map
        for (key, value) in applied_properties {
            context.insert(key.clone(), value.clone());
            inline_style_props.insert(key, value.to_string());
        }
    }

    fn process_imports_and_templates(&mut self, program: &Program, file_path_for_namespace: &str) {
        // Determine the namespace for the current file.
        // It's either defined by a [Namespace] directive or defaults to the filename.
        let mut namespace_for_this_file = std::path::Path::new(file_path_for_namespace)
            .file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("::default")
            .to_string();

        for statement in &program.statements {
            if let Statement::Namespace(ns) = statement {
                namespace_for_this_file = ns.name.value.clone();
                break; // First namespace directive wins
            }
        }

        for statement in &program.statements {
            match statement {
                Statement::TemplateDefinition(def) => {
                    self.templates
                        .entry(namespace_for_this_file.clone())
                        .or_default()
                        .insert(def.name.value.clone(), def.clone());
                }
                Statement::Import(import_stmt) => {
                    if let ImportSpecifier::File(ImportFileType::Chtl) = &import_stmt.specifier {
                        let path = match &import_stmt.path {
                            Expression::StringLiteral(s) => s.value.clone(),
                            Expression::UnquotedLiteral(u) => u.value.clone(),
                            _ => {
                                eprintln!(
                                    "Warning: Unsupported import path expression: {:?}",
                                    import_stmt.path
                                );
                                continue;
                            }
                        };

                        if let Ok(content) = self.loader.load_file_content(&path) {
                            let lexer = Lexer::new(&content);
                            let mut parser = Parser::new(lexer);
                            let imported_program = parser.parse_program();
                            if !parser.errors().is_empty() {
                                eprintln!(
                                    "Warning: Parsing errors in imported file '{}': {:?}",
                                    path,
                                    parser.errors()
                                );
                            }
                            // Recursively process the imported file, passing its path
                            self.process_imports_and_templates(&imported_program, &path);
                        }
                    }
                }
                _ => {}
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
        self.process_imports_and_templates(program, "main.chtl");

        // New pass: collect all element properties for cross-element reference.
        self.collect_element_properties(&program.statements);

        // Determine the main namespace for the generation pass.
        self.current_namespace = "::default".to_string(); // Reset before check
        for statement in &program.statements {
            if let Statement::Namespace(ns) = statement {
                self.current_namespace = ns.name.value.clone();
                break;
            }
        }

        // Second pass: generate the actual HTML from the main program.
        let mut html = String::new();
        for statement in &program.statements {
            // Don't generate output for top-level templates or imports.
            if !matches!(
                statement,
                Statement::TemplateDefinition(_) | Statement::Import(_) | Statement::Namespace(_)
            ) {
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
            Statement::UseTemplate(use_stmt) => {
                if use_stmt.template_type.value == "Element" {
                    let namespace_key =
                        use_stmt.from.as_ref().map_or(&self.current_namespace, |f| &f.value);
                    if let Some(templates_in_ns) = self.templates.get(namespace_key) {
                        if let Some(template) = templates_in_ns.get(&use_stmt.name.value).cloned()
                        {
                            if matches!(template.template_type, TemplateType::Element) {
                                let mut html = String::new();
                                for stmt in &template.body {
                                    html.push_str(&self.generate_statement(stmt));
                                }
                                return html;
                            }
                        }
                    }
                }
                String::new()
            }
            _ => String::new(),
        }
    }

    fn generate_comment(&self, comment: &CommentStatement) -> String {
        format!("<!--{}-->", comment.value)
    }

    fn generate_element(&mut self, element: &ElementStatement) -> String {
        use std::collections::HashMap;
        let mut attributes: HashMap<String, String> = HashMap::new();
        let mut children = String::new();

        // 1. Separate statements into different categories
        let mut style_statements: Vec<StyleStatement> = Vec::new();
        let mut if_statements: Vec<IfStatement> = Vec::new();
        let mut other_statements: Vec<Statement> = Vec::new();

        for statement in &element.body {
            match statement {
                Statement::Style(s) => style_statements.push(s.clone()),
                Statement::If(i) => if_statements.push(i.clone()),
                _ => other_statements.push(statement.clone()),
            }
        }

        // 2. Build a context for evaluating `if` conditions from non-conditional statements.
        let mut context = HashMap::new();
        let mut temp_prop_stmts: Vec<Statement> = other_statements.iter().cloned().collect();
        temp_prop_stmts.extend(style_statements.iter().flat_map(|s| s.body.clone()));

        for stmt in &temp_prop_stmts {
            if let Statement::Attribute(attr) = stmt {
                if let Some(expr) = &attr.value {
                    let value_obj = self.eval_expression_to_object(expr, &mut context);
                    context.insert(attr.name.value.clone(), value_obj);
                }
            }
        }

        // 3. Evaluate `if` statement chains and add their body statements to the appropriate lists.
        for if_stmt in if_statements {
            let statements_to_add = self.evaluate_if_chain(&if_stmt, &mut context);
            for stmt in statements_to_add {
                match stmt {
                    Statement::Style(s) => style_statements.push(s),
                    _ => other_statements.push(stmt),
                }
            }
        }

        // 4. Process all "other" statements (including those from applied `if`s)
        for statement in &other_statements {
            match statement {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        // Use the full context now for evaluation
                        let value = self.eval_expression_to_string(expr, &mut context);
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

        // 5. Process style statements (including those from applied `if`s)
        // Merge all style statements into a single one to ensure deterministic property order.
        let mut merged_style_body = Vec::new();
        for style in style_statements {
            merged_style_body.extend(style.body);
        }
        let merged_style_statement = StyleStatement {
            body: merged_style_body,
        };
        self.generate_style(&merged_style_statement, &mut attributes);

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
                        let all_templates: HashMap<_, _> = self
                            .templates
                            .values()
                            .flat_map(|m| m.iter())
                            .map(|(k, v)| (k.clone(), v.clone()))
                            .collect();
                        let value_obj = self.evaluator.eval(
                            expr,
                            &mut context,
                            &all_templates,
                            &self.document_map,
                        );
                        context.insert(attr.name.value.clone(), value_obj.clone());
                        inline_style_props.insert(attr.name.value.clone(), value_obj.to_string());
                    }
                }
                Statement::StyleRule(rule) => {
                    style_rules.push(rule);
                }
                Statement::UseTemplate(use_stmt) => {
                    if use_stmt.template_type.value == "Style" {
                        let namespace_key = use_stmt
                            .from
                            .as_ref()
                            .map_or(self.current_namespace.clone(), |f| f.value.clone());
                        if let Some(templates_in_ns) = self.templates.get(&namespace_key) {
                            if let Some(template) =
                                templates_in_ns.get(&use_stmt.name.value).cloned()
                            {
                                if matches!(template.template_type, TemplateType::Style) {
                                    self.apply_style_template(
                                        &template,
                                        &namespace_key,
                                        &mut context,
                                        &mut inline_style_props,
                                        &use_stmt.body,
                                    );
                                }
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
                        let all_templates: HashMap<_, _> = self
                            .templates
                            .values()
                            .flat_map(|m| m.iter())
                            .map(|(k, v)| (k.clone(), v.clone()))
                            .collect();
                        let value_obj = self.evaluator.eval(
                            expr,
                            &mut rule_context,
                            &all_templates,
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
        &mut self,
        expr: &Expression,
        context: &mut std::collections::HashMap<String, Object>,
    ) -> String {
        let value_obj = self.eval_expression_to_object(expr, context);
        match value_obj {
            Object::Error(e) => {
                eprintln!("Evaluation Error: {}", e);
                "".to_string()
            }
            _ => value_obj.to_string(),
        }
    }

    fn eval_expression_to_object(
        &mut self,
        expr: &Expression,
        context: &mut std::collections::HashMap<String, Object>,
    ) -> Object {
        let all_templates: HashMap<_, _> = self
            .templates
            .values()
            .flat_map(|m| m.iter())
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect();
        self.evaluator
            .eval(expr, context, &all_templates, &self.document_map)
    }

    fn is_truthy(&self, object: &Object) -> bool {
        match object {
            Object::Boolean(b) => *b,
            Object::Number(n, _) => *n != 0.0,
            Object::String(s) => !s.is_empty(),
            _ => false, // Errors, etc. are not truthy
        }
    }

    fn evaluate_if_chain(
        &mut self,
        if_stmt: &IfStatement,
        context: &mut HashMap<String, Object>,
    ) -> Vec<Statement> {
        let condition_result = self.eval_expression_to_object(&if_stmt.condition, context);

        if self.is_truthy(&condition_result) {
            return if_stmt.consequence.clone();
        } else if let Some(alternative) = &if_stmt.alternative {
            match &**alternative {
                Statement::If(next_if_stmt) => {
                    return self.evaluate_if_chain(next_if_stmt, context);
                }
                Statement::Else(else_stmt) => {
                    return else_stmt.consequence.clone();
                }
                _ => {
                    return vec![];
                }
            }
        }
        vec![]
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::lexer::lexer::Lexer;
    use tempfile::Builder;

    fn generate_html(input: &str) -> String {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );
        let mut generator = Generator::new();
        generator.generate(&program)
    }

    #[test]
    fn test_simple_if_true() {
        let input = r#"
        div {
            width: 100;
            if {
                condition: width > 50;
                height: 200;
            }
        }
        "#;
        let html = generate_html(input);
        assert!(html.contains(r#" height="200""#));
        assert!(html.contains(r#" width="100""#));
    }

    #[test]
    fn test_simple_if_false() {
        let input = r#"
        div {
            width: 40;
            if {
                condition: width > 50;
                height: 200;
            }
        }
        "#;
        let html = generate_html(input);
        assert!(!html.contains(r#"height"#));
        assert!(html.contains(r#" width="40""#));
    }

    #[test]
    fn test_if_else_statement() {
        let input = r#"
        div {
            width: 40;
            if {
                condition: width > 50;
                height: 200;
            } else {
                height: 100;
            }
        }
        "#;
        let html = generate_html(input);
        assert!(html.contains(r#" height="100""#));
        assert!(!html.contains(r#"height="200""#));
    }

    #[test]
    fn test_if_else_if_else_statement() {
        let input = r#"
        div {
            width: 80;
            if {
                condition: width > 100;
                height: 300;
            } else if {
                condition: width > 70;
                height: 200;
            } else {
                height: 100;
            }
        }
        "#;
        let html = generate_html(input);
        assert!(html.contains(r#" height="200""#));
        assert!(!html.contains(r#"height="300""#));
        assert!(!html.contains(r#"height="100""#));
    }

    #[test]
    fn test_namespace_template_resolution() {
        // Create a temporary directory for our test files
        let dir = Builder::new().prefix("chtl_test").tempdir().unwrap();
        let lib_path = dir.path().join("lib.chtl");
        let main_path = dir.path().join("main.chtl");

        // Create the library file with a namespaced template
        let lib_content = r#"
            [Namespace] my_lib;

            [Template] @Element MyBox {
                div {
                    class: "box";
                }
            }
        "#;
        std::fs::write(&lib_path, lib_content).unwrap();

        // Create the main file that imports and uses the template
        let main_content = r#"
            [Import] @Chtl from "./lib.chtl";

            body {
                @Element MyBox from my_lib;
            }
        "#;
        std::fs::write(&main_path, main_content).unwrap();

        // Set up the loader to know where the main file is
        let mut loader = Loader::new();
        loader.set_current_file_path(main_path.to_str().unwrap());

        // Parse the main file
        let lexer = Lexer::new(main_content);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );

        // Generate the HTML
        let mut generator = Generator::new();
        generator.loader = loader; // Use our configured loader
        let html = generator.generate(&program);

        // Verify the output
        let expected_html = r#"<body><div class="box"></div></body>"#;
        assert_eq!(html.trim(), expected_html.trim());
    }

    #[test]
    fn test_implicit_namespace_resolution() {
        // A template is used without a `from` clause.
        // It should resolve to the template defined in the same file's namespace.
        let input = r#"
            [Namespace] my_ui;

            [Template] @Element Button {
                button {
                    class: "btn";
                }
            }

            body {
                @Element Button; // Implicitly from `my_ui`
            }
        "#;

        // Parse the file
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );

        // Generate the HTML
        let mut generator = Generator::new();
        let html = generator.generate(&program);

        // Verify the output
        let expected_html = r#"<body><button class="btn"></button></body>"#;
        assert_eq!(html.trim(), expected_html.trim());
    }
}