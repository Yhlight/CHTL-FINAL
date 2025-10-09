use crate::chtl::node::ast::*;
use crate::chtl::evaluator::object::Object;

pub struct Evaluator;

impl Evaluator {
    pub fn new() -> Self {
        Evaluator
    }

    pub fn eval(&self, node: &Expression) -> Object {
        match node {
            Expression::StringLiteral(s) => Object::String(s.value.clone()),
            Expression::UnquotedLiteral(u) => self.eval_unquoted_literal(u),
            Expression::Infix(i) => self.eval_infix_expression(i),
            Expression::Identifier(i) => Object::String(i.value.clone()),
            Expression::NumberLiteral(n) => self.eval_unquoted_literal(&UnquotedLiteralExpression { value: n.value.clone() }),
        }
    }

    fn eval_unquoted_literal(&self, literal: &UnquotedLiteralExpression) -> Object {
        // Heuristic: if it starts with a digit or dot, treat as a number. Otherwise, a string.
        if literal.value.chars().next().map_or(false, |c| c.is_ascii_digit() || c == '.') {
            let (val, unit) = self.parse_value_unit(&literal.value);
            Object::Number(val, unit)
        } else {
            Object::String(literal.value.clone())
        }
    }

    fn eval_infix_expression(&self, node: &InfixExpression) -> Object {
        let left = self.eval(&node.left);
        let right = self.eval(&node.right);

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
                    _ => Object::Error(format!("Unknown operator: {}", node.operator)),
                }
            }
            _ => Object::Error(format!("Type mismatch for operator {}", node.operator)),
        }
    }

    fn parse_value_unit(&self, s: &str) -> (f64, String) {
        let s = s.trim();
        let split_index = s.find(|c: char| !c.is_ascii_digit() && c != '.').unwrap_or(s.len());
        let (value_str, unit_str) = s.split_at(split_index);

        let value = value_str.trim().parse::<f64>().unwrap_or(0.0);
        let unit = unit_str.trim().to_string();
        (value, unit)
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
                let evaluator = Evaluator::new();
                return evaluator.eval(&attr.value);
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
}