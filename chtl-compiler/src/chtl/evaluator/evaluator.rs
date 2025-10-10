use crate::chtl::evaluator::object::Object;
use crate::chtl::node::ast::*;
use std::collections::HashMap;

pub type DocumentMap = HashMap<String, Vec<(String, Expression)>>;

pub struct Evaluator;

impl Evaluator {
    pub fn new() -> Self {
        Evaluator
    }

    pub fn eval(
        &self,
        node: &Expression,
        context: &HashMap<String, Object>,
        templates: &HashMap<String, TemplateDefinitionStatement>,
        document_map: &DocumentMap,
    ) -> Object {
        match node {
            Expression::StringLiteral(s) => Object::String(s.value.clone()),
            Expression::UnquotedLiteral(u) => self.eval_unquoted_literal(u, context),
            Expression::Infix(i) => self.eval_infix_expression(i, context, templates, document_map),
            Expression::Identifier(i) => {
                if let Some(val) = context.get(&i.value) {
                    return val.clone();
                }
                Object::String(i.value.clone())
            }
            Expression::NumberLiteral(n) => {
                let value = n.value.parse::<f64>().unwrap_or(0.0);
                let unit = n.unit.clone().unwrap_or_else(|| "".to_string());
                Object::Number(value, unit)
            }
            Expression::Conditional(c) => {
                self.eval_conditional_expression(c, context, templates, document_map)
            }
            Expression::FunctionCall(f) => {
                self.eval_function_call_expression(f, templates, document_map)
            }
            Expression::PropertyAccess(p) => {
                self.eval_property_access_expression(p, context, templates, document_map)
            }
        }
    }

    fn eval_property_access_expression(
        &self,
        node: &PropertyAccessExpression,
        context: &HashMap<String, Object>,
        templates: &HashMap<String, TemplateDefinitionStatement>,
        document_map: &DocumentMap,
    ) -> Object {
        let object_selector = match self.eval(&node.object, context, templates, document_map) {
            Object::String(s) => s,
            _ => {
                return Object::Error(format!(
                    "Property access object must be a selector string, but got {:?}",
                    &node.object
                ))
            }
        };

        let element_id = object_selector.trim_start_matches(|c| c == '#' || c == '.');
        let property_name = &node.property.value;

        if let Some(properties) = document_map.get(element_id) {
            let mut referenced_context = HashMap::new();
            for (p_name, p_expr) in properties {
                let value = self.eval(p_expr, &referenced_context, templates, document_map);
                referenced_context.insert(p_name.clone(), value);
            }

            if let Some(final_value) = referenced_context.get(property_name) {
                return final_value.clone();
            } else {
                return Object::Error(format!(
                    "Property '{}' not found on element '{}' after evaluation",
                    property_name, element_id
                ));
            }
        }

        Object::Error(format!(
            "Element with selector '{}' not found in document map",
            object_selector
        ))
    }

    fn eval_function_call_expression(
        &self,
        node: &FunctionCallExpression,
        templates: &HashMap<String, TemplateDefinitionStatement>,
        document_map: &DocumentMap,
    ) -> Object {
        if let Expression::Identifier(ident) = &*node.function {
            let template_name = &ident.value;
            if let Some(template) = templates.get(template_name) {
                if let TemplateType::Var = template.template_type {
                    if let Some(Expression::Identifier(arg_ident)) = node.arguments.get(0) {
                        let var_name = &arg_ident.value;
                        for stmt in &template.body {
                            if let Statement::Attribute(attr) = stmt {
                                if attr.name.value == *var_name {
                                    if let Some(expr) = &attr.value {
                                        let empty_context = HashMap::new();
                                        return self.eval(
                                            expr,
                                            &empty_context,
                                            templates,
                                            document_map,
                                        );
                                    }
                                }
                            }
                        }
                        return Object::Error(format!(
                            "Variable '{}' not found in template '{}'",
                            var_name, template_name
                        ));
                    }
                }
            }
            return Object::Error(format!("Template '{}' not found", template_name));
        }
        Object::Error("Function call must be an identifier".to_string())
    }

    fn eval_conditional_expression(
        &self,
        node: &ConditionalExpression,
        context: &HashMap<String, Object>,
        templates: &HashMap<String, TemplateDefinitionStatement>,
        document_map: &DocumentMap,
    ) -> Object {
        let condition = self.eval(&node.condition, context, templates, document_map);

        match condition {
            Object::Boolean(b) => {
                if b {
                    self.eval(&node.consequence, context, templates, document_map)
                } else if let Some(alt) = &node.alternative {
                    self.eval(alt, context, templates, document_map)
                } else {
                    Object::String("".to_string())
                }
            }
            _ => Object::Error(
                "Condition in conditional expression did not evaluate to a boolean".to_string(),
            ),
        }
    }

    fn eval_unquoted_literal(
        &self,
        literal: &UnquotedLiteralExpression,
        context: &HashMap<String, Object>,
    ) -> Object {
        if let Some(obj) = context.get(&literal.value) {
            return obj.clone();
        }
        Object::String(literal.value.clone())
    }

    fn eval_infix_expression(
        &self,
        node: &InfixExpression,
        context: &HashMap<String, Object>,
        templates: &HashMap<String, TemplateDefinitionStatement>,
        document_map: &DocumentMap,
    ) -> Object {
        let left = self.eval(&node.left, context, templates, document_map);
        let right = self.eval(&node.right, context, templates, document_map);

        match (left, right) {
            (Object::Number(left_val, left_unit), Object::Number(right_val, right_unit)) => {
                if !left_unit.is_empty() && !right_unit.is_empty() && left_unit != right_unit {
                    return Object::Error(format!("Unit mismatch: {} and {}", left_unit, right_unit));
                }

                let result_unit = if !left_unit.is_empty() { left_unit } else { right_unit };

                match node.operator.as_str() {
                    "+" => Object::Number(left_val + right_val, result_unit),
                    "-" => Object::Number(left_val - right_val, result_unit),
                    "*" => Object::Number(left_val * right_val, result_unit),
                    "/" => Object::Number(left_val / right_val, result_unit),
                    "**" => Object::Number(left_val.powf(right_val), result_unit),
                    "%" => Object::Number(left_val % right_val, result_unit),
                    ">" => Object::Boolean(left_val > right_val),
                    "<" => Object::Boolean(left_val < right_val),
                    _ => Object::Error(format!("Unknown operator: {}", node.operator)),
                }
            }
            _ => Object::Error(format!("Type mismatch for operator {}", node.operator)),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::lexer::lexer::Lexer;
    use crate::chtl::parser::parser::Parser;

    fn test_eval(input: &str) -> Object {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            let attr_stmt = &style_stmt.body[0];
            if let Statement::Attribute(attr) = attr_stmt {
                if let Some(expr) = &attr.value {
                    let evaluator = Evaluator::new();
                    let context = std::collections::HashMap::new();
                    let templates = std::collections::HashMap::new();
                    let document_map = std::collections::HashMap::new();
                    return evaluator.eval(expr, &context, &templates, &document_map);
                }
            }
        }
        panic!("Invalid test input");
    }

    #[test]
    fn test_eval_integer_expression() {
        let tests = vec![
            ("width: 5 + 5;", Object::Number(10.0, "".to_string())),
            ("width: 10 - 5;", Object::Number(5.0, "".to_string())),
            ("width: 2 * 2;", Object::Number(4.0, "".to_string())),
            ("width: 10 / 2;", Object::Number(5.0, "".to_string())),
            ("width: 2 ** 3;", Object::Number(8.0, "".to_string())),
            ("width: 10 % 3;", Object::Number(1.0, "".to_string())),
            ("width: 10px + 5px;", Object::Number(15.0, "px".to_string())),
            ("width: 10 + 5px;", Object::Number(15.0, "px".to_string())),
        ];

        for (input, expected) in tests {
            let full_input = format!("style {{ {} }}", input);
            let result = test_eval(&full_input);
            assert_eq!(result, expected);
        }
    }

    #[test]
    fn test_eval_conditional_expression() {
        let tests = vec![
            (
                "background: 10 > 5 ? \"red\" : \"blue\";",
                Object::String("red".to_string()),
            ),
            (
                "background: 5 > 10 ? \"red\" : \"blue\";",
                Object::String("blue".to_string()),
            ),
            (
                "background: 10 > 5 ? 100px : 200px;",
                Object::Number(100.0, "px".to_string()),
            ),
        ];

        for (input, expected) in tests {
            let full_input = format!("style {{ {} }}", input);
            let result = test_eval(&full_input);
            assert_eq!(result, expected);
        }
    }

    #[test]
    fn test_eval_with_context() {
        let input = r#"
        style {
            width: 100px;
            height: width > 50px ? 200px : 50px;
        }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let stmt = &program.statements[0];

        if let Statement::Style(style_stmt) = stmt {
            let evaluator = Evaluator::new();
            let mut context = std::collections::HashMap::new();
            let templates = std::collections::HashMap::new();
            let document_map = std::collections::HashMap::new();

            // Manually evaluate the first attribute to populate context
            if let Statement::Attribute(attr) = &style_stmt.body[0] {
                if let Some(expr) = &attr.value {
                    let val = evaluator.eval(expr, &context, &templates, &document_map);
                    context.insert(attr.name.value.clone(), val);
                }
            } else {
                panic!("Expected first statement to be an attribute");
            }

            // Evaluate the second attribute using the populated context
            if let Statement::Attribute(attr) = &style_stmt.body[1] {
                if let Some(expr) = &attr.value {
                    let result = evaluator.eval(expr, &context, &templates, &document_map);
                    assert_eq!(result, Object::Number(200.0, "px".to_string()));
                }
            } else {
                panic!("Expected second statement to be an attribute");
            }
        } else {
            panic!("Expected a style statement");
        }
    }

    #[test]
    fn test_eval_property_access_expression() {
        // 1. Setup: Create a DocumentMap that simulates a parsed document.
        let mut document_map = DocumentMap::new();
        let box1_props = vec![
            (
                "width".to_string(),
                Expression::NumberLiteral(NumberLiteralExpression {
                    value: "100".to_string(),
                    unit: Some("px".to_string()),
                }),
            ),
            (
                "height".to_string(),
                Expression::Identifier(IdentifierExpression {
                    value: "width".to_string(),
                }),
            ),
        ];
        document_map.insert("box1".to_string(), box1_props);

        // 2. Create the expression we want to evaluate: `#box1.height`
        let property_access_expr = Expression::PropertyAccess(PropertyAccessExpression {
            object: Box::new(Expression::UnquotedLiteral(UnquotedLiteralExpression {
                value: "#box1".to_string(),
            })),
            property: IdentifierExpression {
                value: "height".to_string(),
            },
        });

        // 3. Evaluate the expression.
        let evaluator = Evaluator::new();
        let context = HashMap::new();
        let templates = HashMap::new();
        let result = evaluator.eval(
            &property_access_expr,
            &context,
            &templates,
            &document_map,
        );

        // 4. Assert the result. The evaluator should resolve `#box1.height` to `#box1.width`, which is `100px`.
        assert_eq!(result, Object::Number(100.0, "px".to_string()));
    }

    #[test]
    fn test_eval_variable_group_template() {
        let input = r#"
        [Template] @Var ThemeColor {
            tableColor: "rgb(255, 192, 203)";
        }

        style {
            background-color: ThemeColor(tableColor);
        }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let template_def = program.statements[0].clone();
        let style_stmt = &program.statements[1];

        let mut templates = std::collections::HashMap::new();
        if let Statement::TemplateDefinition(def) = template_def {
            templates.insert(def.name.value.clone(), def);
        } else {
            panic!("Expected a template definition");
        }

        if let Statement::Style(style) = style_stmt {
            if let Statement::Attribute(attr) = &style.body[0] {
                if let Some(expr) = &attr.value {
                    let evaluator = Evaluator::new();
                    let context = std::collections::HashMap::new();
                    let document_map = std::collections::HashMap::new();
                    let result = evaluator.eval(expr, &context, &templates, &document_map);
                    assert_eq!(result, Object::String("rgb(255, 192, 203)".to_string()));
                }
            } else {
                panic!("Expected an attribute statement");
            }
        } else {
            panic!("Expected a style statement");
        }
    }
}