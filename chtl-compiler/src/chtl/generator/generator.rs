use crate::chtl::config_manager::ConfigManager;
use crate::chtl::evaluator::evaluator::Evaluator;
use crate::chtl::evaluator::object::Object;
use crate::chtl::loader::Loader;
use crate::chtl::node::ast::{
    CommentStatement, ElementStatement, ExportStatement, Expression, IfStatement, ImportFileType,
    ImportItemCategory, ImportSpecifier, InfoStatement, Program, Statement, StyleStatement,
    TemplateDefinitionStatement, TemplateType, TextStatement, UseStatement, UseTarget,
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
    pub module_info: HashMap<String, InfoStatement>,
    pub module_exports: HashMap<String, ExportStatement>,
    use_html5: bool,
}

impl Generator {
    pub fn new(initial_file_path: Option<&str>) -> Self {
        let mut loader = Loader::new();
        if let Some(path) = initial_file_path {
            loader.set_current_file_path(path);
        }

        Generator {
            evaluator: Evaluator::new(),
            global_css: String::new(),
            templates: HashMap::new(),
            loader,
            document_map: HashMap::new(),
            current_namespace: "::default".to_string(),
            module_info: HashMap::new(),
            module_exports: HashMap::new(),
            use_html5: false,
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
                    if use_stmt.template_type.value == "style" {
                        // Nested template usage. Resolve it within the parent's namespace
                        // unless a 'from' clause is specified.
                        let namespace_key =
                            use_stmt.from.as_ref().map_or(template_namespace, |f| &f.value);

                        if !self.is_symbol_exported(namespace_key, &use_stmt.name.value, "style") {
                            eprintln!(
                                "Warning: Attempt to use non-exported template Style '{}' from namespace '{}'",
                                use_stmt.name.value, namespace_key
                            );
                            continue;
                        }

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

    fn process_imports_and_templates(&mut self, program: &Program, file_path_for_namespace: &std::path::Path) {
        // Determine the namespace for the current file.
        // It's either defined by a [Namespace] directive or defaults to the filename.
        let mut namespace_for_this_file = file_path_for_namespace
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
                Statement::Info(info) => {
                    self.module_info
                        .insert(namespace_for_this_file.clone(), info.clone());
                }
                Statement::Export(export) => {
                    self.module_exports
                        .insert(namespace_for_this_file.clone(), export.clone());
                }
                Statement::Import(import_stmt) => {
                    let path_str = match &import_stmt.path {
                        Expression::StringLiteral(s) => s.value.clone(),
                        Expression::UnquotedLiteral(u) => u.value.clone(),
                        _ => {
                            eprintln!("Warning: Unsupported import path expression: {:?}", import_stmt.path);
                            continue;
                        }
                    };

                    match self.loader.resolve_path(&path_str) {
                        Ok(resolved_path) => {
                            match std::fs::read_to_string(&resolved_path) {
                                Ok(content) => {
                                    let config = ConfigManager::new();
                                    let lexer = Lexer::new(&content, &config);
                                    let mut parser = Parser::new(lexer);
                                    let imported_program = parser.parse_program();

                                    self.process_imports_and_templates(&imported_program, &resolved_path);

                                    if let ImportSpecifier::Item(item_spec) = &import_stmt.specifier {
                                        let mut imported_ns_name = resolved_path
                                            .file_stem()
                                            .and_then(|s| s.to_str())
                                            .unwrap_or("::default")
                                            .to_string();
                                        for stmt in &imported_program.statements {
                                            if let Statement::Namespace(ns) = stmt {
                                                imported_ns_name = ns.name.value.clone();
                                                break;
                                            }
                                        }

                                        if let Some(source_templates) = self.templates.get(&imported_ns_name).cloned() {
                                            for (template_name, template_def) in source_templates {
                                                let mut matches = true;
                                                if let Some(spec_name) = &item_spec.name {
                                                    if template_name != spec_name.value {
                                                        matches = false;
                                                    }
                                                }
                                                if let Some(spec_type) = &item_spec.item_type {
                                                    let type_str = match template_def.template_type {
                                                        TemplateType::Style => "style",
                                                        TemplateType::Element => "element",
                                                        TemplateType::Var => "var",
                                                    };
                                                    if !type_str.eq_ignore_ascii_case(&spec_type.value) {
                                                        matches = false;
                                                    }
                                                }
                                                if let Some(spec_category) = &item_spec.category {
                                                    let is_custom = matches!(spec_category, ImportItemCategory::Custom);
                                                    if template_def.is_custom != is_custom {
                                                        matches = false;
                                                    }
                                                }

                                                if matches {
                                                    let type_str = match template_def.template_type {
                                                        TemplateType::Style => "style",
                                                        TemplateType::Element => "element",
                                                        TemplateType::Var => "var",
                                                    };
                                                    if self.is_symbol_exported(&imported_ns_name, &template_name, type_str) {
                                                        let mut def_to_add = template_def.clone();
                                                        if let Some(alias) = &import_stmt.alias {
                                                            if item_spec.name.is_some() {
                                                                def_to_add.name.value = alias.value.clone();
                                                            }
                                                        }
                                                        self.templates
                                                            .entry(namespace_for_this_file.clone())
                                                            .or_default()
                                                            .insert(def_to_add.name.value.clone(), def_to_add);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                Err(e) => {
                                    eprintln!(
                                        "Warning: Failed to read imported file '{}': {}",
                                        resolved_path.display(),
                                        e
                                    );
                                }
                            }
                        }
                        Err(e) => {
                            eprintln!(
                                "Warning: Failed to resolve import path '{}': {}",
                                path_str, e
                            );
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
        // Use the initial file path stored in the loader.
        let initial_path_str = self
            .loader
            .get_current_file_path()
            .unwrap_or_else(|| "::memory".to_string());
        let initial_path = std::path::Path::new(&initial_path_str);
        self.process_imports_and_templates(program, initial_path);

        // New pass: collect all element properties for cross-element reference.
        self.collect_element_properties(&program.statements);

        // Determine the main namespace for the generation pass.
        let mut main_namespace = initial_path
            .file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("::default")
            .to_string();
        for statement in &program.statements {
            if let Statement::Namespace(ns) = statement {
                main_namespace = ns.name.value.clone();
                break;
            }
        }
        self.current_namespace = main_namespace;

        // Second pass: generate the actual HTML from the main program.
        let mut html = String::new();
        for statement in &program.statements {
            if let Statement::Use(use_stmt) = statement {
                if let UseTarget::Html5 = use_stmt.target {
                    self.use_html5 = true;
                }
                // Config handling would go here
            }

            // Don't generate output for top-level templates or imports.
            if !matches!(
                statement,
                Statement::TemplateDefinition(_)
                    | Statement::Import(_)
                    | Statement::Namespace(_)
                    | Statement::Use(_)
            ) {
                html.push_str(&self.generate_statement(statement));
            }
        }

        if !self.global_css.is_empty() {
            html = format!("<style>{}</style>{}", self.global_css, html);
        }

        if self.use_html5 {
            html = format!("<!DOCTYPE html>{}", html);
        }

        html
    }

    fn generate_statement(&mut self, statement: &Statement) -> String {
        match statement {
            Statement::Origin(origin_stmt) => origin_stmt.content.clone(),
            Statement::Element(element) => self.generate_element(element),
            Statement::Text(text) => self.generate_text(text),
            Statement::Script(script) => self.generate_script(script),
            Statement::Style(style) => {
                // For a global style block, there are no element attributes to modify.
                // We pass a dummy map and only the global_css will be affected.
                let mut dummy_attrs = std::collections::HashMap::new();
                let mut dummy_context = std::collections::HashMap::new();
                self.generate_style(style, &mut dummy_attrs, &mut dummy_context);
                String::new()
            }
            Statement::Comment(comment) => self.generate_comment(comment),
            Statement::UseTemplate(use_stmt) => {
                if use_stmt.template_type.value == "element" {
                    let namespace_key =
                        use_stmt.from.as_ref().map_or(&self.current_namespace, |f| &f.value);

                    if !self.is_symbol_exported(namespace_key, &use_stmt.name.value, "element") {
                        eprintln!(
                            "Warning: Attempt to use non-exported template Element '{}' from namespace '{}'",
                            use_stmt.name.value, namespace_key
                        );
                        return String::new();
                    }

                    if let Some(templates_in_ns) = self.templates.get(namespace_key) {
                        if let Some(template) = templates_in_ns.get(&use_stmt.name.value).cloned()
                        {
                            if matches!(template.template_type, TemplateType::Element) {
                                let specialized_body = self.apply_element_template(&template, &use_stmt.body);
                                let mut html = String::new();
                                for stmt in &specialized_body {
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

    fn generate_script(&self, script: &crate::chtl::node::ast::ScriptStatement) -> String {
        format!("<script>{}</script>", script.content)
    }

    fn process_element_body(
        &mut self,
        statements: &[Statement],
        context: &mut HashMap<String, Object>,
        attributes: &mut HashMap<String, String>,
        children: &mut String,
    ) {
        for statement in statements {
            match statement {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        let value_obj = self.eval_expression_to_object(expr, context);
                        let value_str = value_obj.to_string();
                        context.insert(attr.name.value.clone(), value_obj);
                        if attr.name.value == "text" {
                            children.push_str(&value_str);
                        } else {
                            attributes.insert(attr.name.value.clone(), value_str);
                        }
                    }
                }
                Statement::Style(style) => {
                    self.generate_style(style, attributes, context);
                }
                Statement::If(if_stmt) => {
                    let statements_to_add = self.evaluate_if_chain(if_stmt, context);
                    self.process_element_body(&statements_to_add, context, attributes, children);
                }
                _ => {
                    children.push_str(&self.generate_statement(statement));
                }
            }
        }
    }

    fn generate_element(&mut self, element: &ElementStatement) -> String {
        let mut attributes: HashMap<String, String> = HashMap::new();
        let mut children = String::new();
        let mut context = HashMap::new();

        self.process_element_body(&element.body, &mut context, &mut attributes, &mut children);

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
        context: &mut HashMap<String, Object>,
    ) {
        let mut inline_style_props = std::collections::HashMap::new();
        let mut style_rules = Vec::new();

        for statement in &style.body {
            match statement {
                Statement::Attribute(attr) => {
                    if let Some(expr) = &attr.value {
                        let value_obj = self.eval_expression_to_object(expr, context);
                        context.insert(attr.name.value.clone(), value_obj.clone());
                        inline_style_props.insert(attr.name.value.clone(), value_obj.to_string());
                    }
                }
                Statement::StyleRule(rule) => {
                    style_rules.push(rule);
                }
                Statement::UseTemplate(use_stmt) => {
                    if use_stmt.template_type.value == "style" {
                        let namespace_key = use_stmt
                            .from
                            .as_ref()
                            .map_or(self.current_namespace.clone(), |f| f.value.clone());

                        if !self.is_symbol_exported(
                            &namespace_key,
                            &use_stmt.name.value,
                            "style",
                        ) {
                            eprintln!(
                                "Warning: Attempt to use non-exported template Style '{}' from namespace '{}'",
                                use_stmt.name.value, namespace_key
                            );
                            continue;
                        }

                        if let Some(templates_in_ns) = self.templates.get(&namespace_key) {
                            if let Some(template) =
                                templates_in_ns.get(&use_stmt.name.value).cloned()
                            {
                                if matches!(template.template_type, TemplateType::Style) {
                                    self.apply_style_template(
                                        &template,
                                        &namespace_key,
                                        context,
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

        for rule in &style_rules {
            let mut rule_css_body = String::new();
            let mut rule_context = context.clone();
            for prop in &rule.body {
                if let Statement::Attribute(attr_prop) = prop {
                    if let Some(expr) = &attr_prop.value {
                        let value_obj = self.eval_expression_to_object(expr, &mut rule_context);
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

    fn is_symbol_exported(&self, namespace: &str, symbol_name: &str, symbol_type: &str) -> bool {
        if let Some(export_data) = self.module_exports.get(namespace) {
            // If an [Export] block exists, the symbol MUST be in it.
            return export_data.items.iter().any(|item| {
                // We are not checking the category ([Custom] vs [Template]) for now,
                // as the UseTemplateStatement does not distinguish this.
                // A match on type and name is sufficient for now.
                item.item_type.value.eq_ignore_ascii_case(symbol_type)
                    && item.names.iter().any(|n| n.value == symbol_name)
            });
        }
        // No [Export] block, so all symbols are considered public.
        true
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

    fn apply_element_template(
        &mut self,
        template: &TemplateDefinitionStatement,
        specialization_body: &Option<Vec<Statement>>,
    ) -> Vec<Statement> {
        let mut final_body = template.body.clone();

        if let Some(body) = specialization_body {
            for stmt in body {
                if let Statement::Insert(insert_stmt) = stmt {
                    // TODO: Implement insert logic
                }
            }
        }

        final_body
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::config_manager::ConfigManager;
    use crate::chtl::lexer::lexer::Lexer;
    use std::fs::File;
    use std::io::Write;
    use tempfile::Builder;
    use zip::write::{FileOptions, ZipWriter};

    fn create_cmod_file(path: &std::path::Path, module_name: &str, content: &str) {
        let file = File::create(path).unwrap();
        let mut zip = ZipWriter::new(file);
        let options = FileOptions::default().compression_method(zip::CompressionMethod::Stored);

        zip.add_directory("src/", options).unwrap();
        zip.start_file(format!("src/{}.chtl", module_name), options)
            .unwrap();
        zip.write_all(content.as_bytes()).unwrap();
        zip.finish().unwrap();
    }

    fn generate_html(input: &str) -> String {
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );
        let mut generator = Generator::new(None);
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
            [namespace] my_lib;

            [template] @element MyBox {
                div {
                    class: "box";
                }
            }
        "#;
        std::fs::write(&lib_path, lib_content).unwrap();

        // Create the main file that imports and uses the template
        let main_content = r#"
            [import] @chtl from "./lib.chtl";

            body {
                @element MyBox from my_lib;
            }
        "#;
        std::fs::write(&main_path, main_content).unwrap();

        // Set up the loader to know where the main file is
        let mut loader = Loader::new();
        loader.set_current_file_path(main_path.to_str().unwrap());

        // Parse the main file
        let config = ConfigManager::new();
        let lexer = Lexer::new(main_content, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );

        // Generate the HTML
        let mut generator = Generator::new(Some(main_path.to_str().unwrap()));
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
            [namespace] my_ui;

            [template] @element Button {
                button {
                    class: "btn";
                }
            }

            body {
                @element Button; // Implicitly from `my_ui`
            }
        "#;

        // Parse the file
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );

        // Generate the HTML
        let mut generator = Generator::new(None);
        let html = generator.generate(&program);

        // Verify the output
        let expected_html = r#"<body><button class="btn"></button></body>"#;
        assert_eq!(html.trim(), expected_html.trim());
    }

    #[test]
    fn test_exported_symbol_is_used() {
        let dir = Builder::new().prefix("export_test").tempdir().unwrap();
        let lib_path = dir.path().join("lib.chtl");
        let main_path = dir.path().join("main.chtl");

        let lib_content = r#"
            [namespace] my_lib;
            [export] {
                [template] @element ExportedBox;
            }
            [template] @element ExportedBox {
                div { class: "exported"; }
            }
        "#;
        std::fs::write(&lib_path, lib_content).unwrap();

        let main_content = r#"
            [import] @chtl from "./lib.chtl";
            body {
                @element ExportedBox from my_lib;
            }
        "#;
        std::fs::write(&main_path, main_content).unwrap();

        let mut loader = Loader::new();
        loader.set_current_file_path(main_path.to_str().unwrap());

        let config = ConfigManager::new();
        let lexer = Lexer::new(main_content, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(parser.errors().is_empty());

        let mut generator = Generator::new(Some(main_path.to_str().unwrap()));
        generator.loader = loader;
        let html = generator.generate(&program);

        assert!(html.contains(r#"<div class="exported"></div>"#));
    }

    #[test]
    fn test_unexported_symbol_is_ignored() {
        let dir = Builder::new().prefix("export_test").tempdir().unwrap();
        let lib_path = dir.path().join("lib.chtl");
        let main_path = dir.path().join("main.chtl");

        let lib_content = r#"
            [namespace] my_lib;
            [export] {
                [template] @element AnotherBox;
            }
            [template] @element SecretBox {
                div { class: "secret"; }
            }
        "#;
        std::fs::write(&lib_path, lib_content).unwrap();

        let main_content = r#"
            [import] @chtl from "./lib.chtl";
            body {
                @element SecretBox from my_lib;
            }
        "#;
        std::fs::write(&main_path, main_content).unwrap();

        let mut loader = Loader::new();
        loader.set_current_file_path(main_path.to_str().unwrap());

        let config = ConfigManager::new();
        let lexer = Lexer::new(main_content, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(parser.errors().is_empty());

        let mut generator = Generator::new(Some(main_path.to_str().unwrap()));
        generator.loader = loader;
        let html = generator.generate(&program);

        assert!(!html.contains("secret"));
    }

    #[test]
    fn test_script_block_generation() {
        let input = r#"
        div {
            script {
                if (true) {
                    console.log("Hello from script!");
                }
            }
        }
        "#;
        let html = generate_html(input);
        // More robust check that isn't sensitive to exact whitespace from the lexer
        assert!(html.contains("<script>"));
        assert!(html.contains(r#"console.log("Hello from script!");"#));
        assert!(html.contains("</script>"));
    }

    #[test]
    fn test_cmod_import_and_template_use() {
        let dir = Builder::new().prefix("cmod_import_test").tempdir().unwrap();
        let cmod_path = dir.path().join("my_mod.cmod");
        let main_path = dir.path().join("main.chtl");

        let cmod_content = r#"
            [namespace] cmod_lib;
            [template] @element CmodComponent {
                p {
                    text: "Hello from CMOD!";
                }
            }
        "#;
        create_cmod_file(&cmod_path, "my_mod", cmod_content);

        let main_content = format!(
            r#"
            [import] @chtl from "{}";

            body {{
                @element CmodComponent from cmod_lib;
            }}
        "#,
            cmod_path.to_str().unwrap()
        );
        std::fs::write(&main_path, &main_content).unwrap();

        // The generator will read the file from disk, so we don't need to pass the content directly.
        let source = std::fs::read_to_string(&main_path).unwrap();
        let config = ConfigManager::new();
        let lexer = Lexer::new(&source, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(
            parser.errors().is_empty(),
            "Parser errors: {:?}",
            parser.errors()
        );

        let mut generator = Generator::new(Some(main_path.to_str().unwrap()));
        let html = generator.generate(&program);

        let expected_html = r#"<body><p>Hello from CMOD!</p></body>"#;
        assert_eq!(html.trim(), expected_html.trim());
    }

    #[test]
    fn test_advanced_imports() {
        let dir = Builder::new().prefix("adv_import_test").tempdir().unwrap();
        let lib_path = dir.path().join("lib.chtl");
        let main_path = dir.path().join("main.chtl");

        let lib_content = r#"
            [namespace] my_lib;
            [export] {
                [template] @element ExportedButton;
                [custom] @style CustomText;
            }
            [template] @element ExportedButton {
                button { class: "exported"; }
            }
            [custom] @style CustomText {
                font-size: 16px;
            }
            [template] @element SecretButton {
                button { class: "secret"; }
            }
        "#;
        std::fs::write(&lib_path, lib_content).unwrap();

        let main_content = format!(
            r#"
            [import] [template] @element ExportedButton from "{}";
            [import] [custom] @style CustomText as MyText from "{}";
            [import] [template] @element SecretButton from "{}";

            body {{
                @element ExportedButton;
                div {{
                    style {{
                        @style MyText;
                    }}
                }}
            }}
        "#,
            lib_path.to_str().unwrap(),
            lib_path.to_str().unwrap(),
            lib_path.to_str().unwrap()
        );
        std::fs::write(&main_path, &main_content).unwrap();

        let source = std::fs::read_to_string(&main_path).unwrap();
        let config = ConfigManager::new();
        let lexer = Lexer::new(&source, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(parser.errors().is_empty(), "Parser errors: {:?}", parser.errors());

        let mut generator = Generator::new(Some(main_path.to_str().unwrap()));
        let html = generator.generate(&program);

        assert!(html.contains(r#"<button class="exported"></button>"#));
        assert!(html.contains(r#"<div style="font-size:16px"></div>"#));
        assert!(!html.contains("secret"));
    }

    #[test]
    fn test_origin_block_generation() {
        let input = r#"
        [origin] @html {
            <div><p>Raw HTML</p></div>
        }
        "#;
        let html = generate_html(input);
        assert_eq!(html.trim(), "<div><p>Raw HTML</p></div>");
    }

    #[test]
    fn test_use_html5_generation() {
        let input = r#"
        use html5;
        div {}
        "#;
        let html = generate_html(input);
        assert!(html.starts_with("<!DOCTYPE html>"));
    }
}