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
    Call,        // my_function(X)
    PropertyAccess, // .
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
                if self.peek_token_is(&Token::Template) || self.peek_token_is(&Token::Custom) {
                    return self.parse_template_definition_statement();
                } else if self.peek_token_is(&Token::Import) {
                    return self.parse_import_statement();
                } else if self.peek_token_is(&Token::Namespace) {
                    return self.parse_namespace_statement();
                } else if self.peek_token_is(&Token::Info) {
                    return self.parse_info_statement();
                } else if self.peek_token_is(&Token::Export) {
                    return self.parse_export_statement();
                } else if self.peek_token_is(&Token::Configuration) {
                    return self.parse_configuration_statement();
                } else if self.peek_token_is(&Token::Name) {
                    return self.parse_name_block();
                }
                None
            }
            Token::At => return self.parse_use_template_statement(),
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
            Token::Script => self.parse_script_statement(),
            Token::GeneratorComment(value) => self.parse_comment_statement(value),
            Token::Delete => self.parse_delete_statement(),
            Token::If => self.parse_if_statement(),
            Token::Else => {
                self.errors.push("`else` without a preceding `if`".to_string());
                None
            }
            _ => None,
        }
    }

    fn parse_configuration_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::Configuration)?;
        self.expect_peek(&Token::RBracket)?;
        self.next_token();

        let mut name = None;
        if self.current_token_is(&Token::At) {
            self.next_token();
            if !self.current_token_is(&Token::Config) {
                self.errors.push(format!("Expected 'Config' after '@' in configuration, got {:?}", self.current_token));
                return None;
            }
            self.next_token();

            if let Token::Identifier(config_name) = self.current_token.clone() {
                name = Some(IdentifierExpression { value: config_name });
            } else {
                self.errors.push(format!("Expected identifier for configuration name, got {:?}", self.current_token));
                return None;
            }
            self.next_token();
        }

        if !self.current_token_is(&Token::LBrace) {
            self.errors.push(format!("Expected '{{' to start configuration body, got {:?}", self.current_token));
            return None;
        }
        self.next_token();

        let mut body = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            let stmt = match self.current_token.clone() {
                Token::LBracket if self.peek_token_is(&Token::Name) => self.parse_name_block(),
                _ => self.parse_attribute_statement(),
            };

            if let Some(s) = stmt {
                body.push(s);
            } else {
                self.errors.push(format!("Unexpected token in configuration body: {:?}", self.current_token));
                self.next_token();
            }
        }

        Some(Statement::Configuration(ConfigurationStatement { name, body }))
    }

    fn parse_name_block(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::Name)?;
        self.expect_peek(&Token::RBracket)?;
        self.expect_peek(&Token::LBrace)?;
        self.next_token();

        let mut settings = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(Statement::Attribute(attr)) = self.parse_attribute_statement() {
                settings.push(attr);
            } else {
                self.errors.push(format!("Expected attribute statement in [Name] block, got {:?}", self.current_token));
                self.next_token();
            }
        }
        Some(Statement::NameBlock(NameBlock { settings }))
    }

    fn parse_info_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::Info)?;
        self.expect_peek(&Token::RBracket)?;
        self.expect_peek(&Token::LBrace)?;
        self.next_token();

        let mut attributes = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(Statement::Attribute(attr)) = self.parse_attribute_statement() {
                attributes.push(attr);
            } else {
                self.errors.push(format!("Expected attribute statement in [Info] block, got {:?}", self.current_token));
            }
            self.next_token();
        }
        Some(Statement::Info(InfoStatement { attributes }))
    }

    fn parse_export_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::Export)?;
        self.expect_peek(&Token::RBracket)?;
        self.expect_peek(&Token::LBrace)?;
        self.next_token();

        let mut items = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(item) = self.parse_export_item() {
                items.push(item);
            } else {
                self.errors.push(format!("Expected export item in [Export] block, got {:?}", self.current_token));
            }
            self.next_token();
        }
        Some(Statement::Export(ExportStatement { items }))
    }

    fn parse_if_statement(&mut self) -> Option<Statement> {
        if !self.peek_token_is(&Token::LBrace) {
            self.errors.push(format!("Expected '{{' after 'if', got {:?}", self.peek_token));
            return None;
        }
        self.next_token();
        self.next_token();

        let mut stmts = Vec::new();
        while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                stmts.push(stmt);
            }
            self.next_token();
        }

        let mut condition: Option<Expression> = None;
        let mut consequence: Vec<Statement> = Vec::new();
        let mut condition_found = false;

        for stmt in stmts {
            if !condition_found {
                if let Statement::Attribute(attr) = &stmt {
                    if attr.name.value == "condition" {
                        if let Some(expr) = attr.value.clone() {
                            condition = Some(expr);
                            condition_found = true;
                            continue;
                        }
                    }
                }
            }
            consequence.push(stmt);
        }

        let cond = if let Some(c) = condition {
            c
        } else {
            self.errors.push("`if` block must contain a `condition` property".to_string());
            return None;
        };

        let mut alternative: Option<Box<Statement>> = None;
        if self.peek_token_is(&Token::Else) {
            self.next_token();

            if self.peek_token_is(&Token::If) {
                self.next_token();
                alternative = self.parse_if_statement().map(Box::new);
            } else if self.peek_token_is(&Token::LBrace) {
                self.next_token();
                self.next_token();

                let mut else_consequence = Vec::new();
                while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
                    if let Some(stmt) = self.parse_statement() {
                        else_consequence.push(stmt);
                    }
                    self.next_token();
                }
                let else_stmt = ElseStatement { consequence: else_consequence };
                alternative = Some(Box::new(Statement::Else(else_stmt)));
            } else {
                self.errors.push(format!(
                    "Expected 'if' or '{{' after 'else', got {:?}",
                    self.peek_token
                ));
            }
        }

        Some(Statement::If(IfStatement {
            condition: cond,
            consequence,
            alternative,
        }))
    }

    fn parse_namespace_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::Namespace)?;
        self.expect_peek(&Token::RBracket)?;
        self.next_token();

        let name = if let Token::Identifier(n) = self.current_token.clone() {
            IdentifierExpression { value: n }
        } else {
            self.errors.push(format!(
                "Expected namespace name identifier, got {:?}",
                self.current_token
            ));
            return None;
        };

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Namespace(NamespaceStatement { name }))
    }

    fn parse_delete_statement(&mut self) -> Option<Statement> {
        self.next_token();

        let mut targets = Vec::new();

        loop {
            let expr = if self.current_token_is(&Token::At) {
                self.next_token();
                let type_name =
                    if let Some(s) = self.token_to_string_for_unquoted_literal(&self.current_token)
                    {
                        s
                    } else {
                        self.errors.push(format!(
                            "Expected template type after @ in delete, got {:?}",
                            self.current_token
                        ));
                        return None;
                    };
                self.next_token();

                let template_name = if let Token::Identifier(s) = self.current_token.clone() {
                    s
                } else {
                    self.errors.push(format!(
                        "Expected template name after type in delete, got {:?}",
                        self.current_token
                    ));
                    return None;
                };
                Some(Expression::UnquotedLiteral(UnquotedLiteralExpression {
                    value: format!("@{} {}", type_name, template_name),
                }))
            } else {
                self.parse_expression(Precedence::Lowest)
            };

            if let Some(e) = expr {
                targets.push(e);
            } else {
                self.errors.push(format!(
                    "Invalid expression in delete statement near {:?}",
                    self.current_token
                ));
                return None;
            }

            if self.peek_token_is(&Token::Comma) {
                self.next_token();
                self.next_token();
            } else {
                break;
            }
        }

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Delete(DeleteStatement { targets }))
    }

    fn parse_comment_statement(&mut self, value: String) -> Option<Statement> {
        Some(Statement::Comment(CommentStatement { value }))
    }

    fn parse_script_statement(&mut self) -> Option<Statement> {
        // When this is called, current_token is 'script' and peek_token is '{'.
        // The lexer's internal `ch` is positioned right after the '{' due to the peek.
        // This is the perfect time to read the raw body.
        if !self.peek_token_is(&Token::LBrace) {
            self.errors.push(format!("Expected '{{' after 'script', got {:?}", self.peek_token));
            return None;
        }

        let content = self.lexer.read_raw_body();

        // Now, the lexer has advanced past the entire script block.
        // The parser's token state is stale (current: 'script', peek: '{').
        // We need to sync the parser by setting its current and peek tokens
        // to reflect the new state of the lexer.

        // This is equivalent to calling next_token() twice, but on the *new* lexer state.
        self.next_token();
        self.next_token();

        Some(Statement::Script(ScriptStatement { content }))
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

        self.next_token();

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
        self.next_token();
        let consequence = self.parse_expression(Precedence::Conditional)?;

        let alternative = if self.peek_token_is(&Token::Colon) {
            self.next_token();
            self.next_token();
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
        let is_custom = self.peek_token_is(&Token::Custom);

        self.next_token();

        let keyword_token = self.current_token.clone();
        if !matches!(keyword_token, Token::Template | Token::Custom) {
            self.errors.push(format!(
                "Expected 'Template' or 'Custom' keyword, got {:?}",
                keyword_token
            ));
            return None;
        }

        self.next_token();
        if !self.current_token_is(&Token::RBracket) {
            self.errors.push(format!(
                "Expected ']' after template keyword, got {:?}",
                self.current_token
            ));
            return None;
        }

        if self.expect_peek(&Token::At).is_none() {
            return None;
        }

        self.next_token();

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

        self.next_token();

        let name = if let Token::Identifier(n) = self.current_token.clone() {
            IdentifierExpression { value: n }
        } else {
            self.errors.push(format!(
                "Expected template name identifier, got {:?}",
                self.current_token
            ));
            return None;
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

        Some(Statement::TemplateDefinition(TemplateDefinitionStatement {
            name,
            template_type,
            body,
            is_custom,
        }))
    }

    fn parse_import_statement(&mut self) -> Option<Statement> {
        self.expect_peek(&Token::Import)?;
        self.expect_peek(&Token::RBracket)?;
        self.next_token();

        if self.current_token_is(&Token::At) {
             if let Some(file_type) = match self.peek_token.clone() {
                Token::Chtl => Some(ImportFileType::Chtl),
                Token::Html => Some(ImportFileType::Html),
                Token::Style => Some(ImportFileType::Style),
                Token::JavaScript => Some(ImportFileType::JavaScript),
                Token::CjMod => Some(ImportFileType::CjMod),
                Token::Config => Some(ImportFileType::Config),
                _ => None,
            } {
                self.next_token();
                self.next_token();
                let specifier = ImportSpecifier::File(file_type);
                return self.parse_import_from_path_alias(specifier);
            }
        }

        let mut category: Option<ImportItemCategory> = None;
        if self.current_token_is(&Token::LBracket) {
            self.next_token();
            category = match self.current_token.clone() {
                Token::Custom => Some(ImportItemCategory::Custom),
                Token::Template => Some(ImportItemCategory::Template),
                Token::Origin => Some(ImportItemCategory::Origin),
                Token::Configuration => Some(ImportItemCategory::Configuration),
                _ => {
                    self.errors.push(format!("Invalid import category: {:?}", self.current_token));
                    return None;
                }
            };
            self.next_token();
            if !self.current_token_is(&Token::RBracket) {
                 self.errors.push(format!("Expected ']' after import category, got {:?}", self.current_token));
                 return None;
            }
            self.next_token();
        }

        let mut item_type: Option<IdentifierExpression> = None;
        if self.current_token_is(&Token::At) {
            self.next_token();
            if let Some(type_str) = self.token_to_string_for_unquoted_literal(&self.current_token) {
                item_type = Some(IdentifierExpression { value: type_str });
                self.next_token();
            } else {
                self.errors.push(format!("Expected item type after @, got {:?}", self.current_token));
                return None;
            }
        }

        let mut name: Option<IdentifierExpression> = None;
        if let Token::Identifier(n) = self.current_token.clone() {
            name = Some(IdentifierExpression { value: n });
            self.next_token();
        }

        let specifier = ImportSpecifier::Item(ImportItemSpecifier {
            category,
            item_type,
            name,
        });

        self.parse_import_from_path_alias(specifier)
    }

    fn parse_path_expression(&mut self) -> Option<Expression> {
        if let Token::String(s) = self.current_token.clone() {
            self.next_token();
            return Some(Expression::StringLiteral(StringLiteralExpression { value: s }));
        }

        let mut path = String::new();
        let is_path_token = |token: &Token| -> bool {
            matches!(token, Token::Identifier(_) | Token::Dot | Token::Slash | Token::Number(_, _) | Token::Chtl | Token::Html | Token::JavaScript | Token::CjMod)
        };

        while is_path_token(&self.current_token) {
            let part = match self.current_token.clone() {
                Token::Identifier(s) => s,
                Token::Dot => ".".to_string(),
                Token::Slash => "/".to_string(),
                Token::Number(n, _) => n,
                Token::Chtl => "chtl".to_string(),
                Token::Html => "html".to_string(),
                Token::JavaScript => "js".to_string(),
                Token::CjMod => "cjmod".to_string(),
                _ => break,
            };
            path.push_str(&part);
            self.next_token();
        }

        if path.is_empty() {
            self.errors.push(format!("Expected a path literal, got {:?}", self.current_token));
            return None;
        }

        Some(Expression::UnquotedLiteral(UnquotedLiteralExpression { value: path }))
    }

    fn parse_import_from_path_alias(&mut self, specifier: ImportSpecifier) -> Option<Statement> {
        let mut alias: Option<IdentifierExpression> = None;
        if self.current_token_is(&Token::As) {
            self.next_token();
            if let Token::Identifier(alias_name) = self.current_token.clone() {
                alias = Some(IdentifierExpression { value: alias_name });
                self.next_token();
            } else {
                self.errors.push(format!("Expected identifier for alias, got {:?}", self.current_token));
                return None;
            }
        }

        if !self.current_token_is(&Token::From) {
            self.errors.push(format!("Expected 'from' after import specifier, got {:?}", self.current_token));
            return None;
        }
        self.next_token();

        let path = self.parse_path_expression()?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Import(ImportStatement {
            path,
            alias,
            specifier,
        }))
    }

    fn parse_use_template_statement(&mut self) -> Option<Statement> {
        self.next_token();

        let template_type_str = match self.current_token.clone() {
            Token::Style => "style",
            Token::Element => "element",
            Token::Var => "var",
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

        self.next_token();

        let name = if let Token::Identifier(n) = self.current_token.clone() {
            IdentifierExpression { value: n }
        } else {
            self.errors.push(format!(
                "Expected template name identifier, got {:?}",
                self.current_token
            ));
            return None;
        };
        self.next_token();

        let mut from = None;
        if self.current_token_is(&Token::From) {
            self.next_token();
            if let Token::Identifier(ns) = self.current_token.clone() {
                from = Some(IdentifierExpression { value: ns });
                self.next_token();
            } else {
                self.errors.push(format!(
                    "Expected namespace identifier after 'from', got {:?}",
                    self.current_token
                ));
                return None;
            }
        }

        let mut body = None;
        if self.current_token_is(&Token::LBrace) {
            self.next_token();

            let mut stmts = Vec::new();
            while !self.current_token_is(&Token::RBrace) && !self.current_token_is(&Token::Eof) {
                if let Some(stmt) = self.parse_statement() {
                    stmts.push(stmt);
                }
                self.next_token();
            }
            body = Some(stmts);
        }

        Some(Statement::UseTemplate(UseTemplateStatement {
            name,
            template_type,
            body,
            from,
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
            Token::Name => IdentifierExpression { value: "name".to_string() },
            _ => return None,
        };

        if self.peek_token_is(&Token::Colon) {
            self.next_token();
            self.next_token();

            let value = self.parse_expression(Precedence::Lowest)?;

            if self.peek_token_is(&Token::Semicolon) {
                self.next_token();
            }

            Some(Statement::Attribute(AttributeStatement {
                name,
                value: Some(value),
            }))
        } else {
            if self.peek_token_is(&Token::Semicolon) {
                self.next_token();
            } else if self.peek_token_is(&Token::Comma) {
                self.next_token();
            }

            Some(Statement::Attribute(AttributeStatement { name, value: None }))
        }
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
            if self.peek_token_is(&Token::LParen) {
                self.next_token();
                left_exp = self.parse_call_expression(left_exp)?;
            } else if self.peek_token_is(&Token::Question) {
                self.next_token();
                left_exp = self.parse_conditional_expression(left_exp)?;
            } else if self.peek_token_is(&Token::Dot) {
                self.next_token();
                left_exp = self.parse_property_access_expression(left_exp)?;
            } else {
                self.next_token();
                left_exp = self.parse_infix_expression(left_exp)?;
            }
        }

        Some(left_exp)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match self.current_token.clone() {
            Token::String(s) => Some(Expression::StringLiteral(StringLiteralExpression { value: s })),
            Token::Number(value, unit) => Some(Expression::NumberLiteral(NumberLiteralExpression {
                value,
                unit,
            })),
            Token::Hash | Token::Dot => {
                let mut selector = if self.current_token_is(&Token::Hash) {
                    "#".to_string()
                } else {
                    ".".to_string()
                };
                self.next_token();
                if let Token::Identifier(name) = self.current_token.clone() {
                    selector.push_str(&name);
                    Some(Expression::UnquotedLiteral(UnquotedLiteralExpression {
                        value: selector,
                    }))
                } else {
                    self.errors
                        .push(format!("Expected identifier after # or ., got {:?}", self.current_token));
                    None
                }
            }
            tok if self.is_unquoted_literal_token(&tok) => {
                if let Token::Identifier(s) = &tok {
                    if !self.is_peek_unquoted_literal_token() {
                        return Some(Expression::Identifier(IdentifierExpression {
                            value: s.clone(),
                        }));
                    }
                }

                let mut literal = String::new();
                let mut current_tok = tok;
                loop {
                    if let Some(s_val) = self.token_to_string_for_unquoted_literal(&current_tok) {
                        literal.push_str(&s_val);
                    } else {
                        break;
                    }

                    if self.is_peek_unquoted_literal_token() {
                        self.next_token();
                        current_tok = self.current_token.clone();
                        literal.push(' ');
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

    fn parse_call_expression(&mut self, function: Expression) -> Option<Expression> {
        let arguments = self.parse_call_arguments()?;
        Some(Expression::FunctionCall(FunctionCallExpression {
            function: Box::new(function),
            arguments,
        }))
    }

    fn parse_property_access_expression(&mut self, object: Expression) -> Option<Expression> {
        if !self.peek_token_is(&Token::Identifier("".to_string())) {
            self.errors.push(format!(
                "Expected property name after '.', got {:?}",
                self.peek_token
            ));
            return None;
        }
        self.next_token();

        let property = if let Token::Identifier(name) = self.current_token.clone() {
            IdentifierExpression { value: name }
        } else {
            unreachable!();
        };

        Some(Expression::PropertyAccess(PropertyAccessExpression {
            object: Box::new(object),
            property,
        }))
    }

    fn parse_call_arguments(&mut self) -> Option<Vec<Expression>> {
        let mut args = Vec::new();

        if self.peek_token_is(&Token::RParen) {
            self.next_token();
            return Some(args);
        }

        self.next_token();

        args.push(self.parse_expression(Precedence::Lowest)?);

        while self.peek_token_is(&Token::Comma) {
            self.next_token();
            self.next_token();
            args.push(self.parse_expression(Precedence::Lowest)?);
        }

        if self.expect_peek(&Token::RParen).is_none() {
            return None;
        }

        Some(args)
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
            Token::LParen => Precedence::Call,
            Token::Dot => Precedence::PropertyAccess,
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
            Token::Number(val, unit) => {
                if let Some(u) = unit {
                    Some(format!("{}{}", val, u))
                } else {
                    Some(val)
                }
            }
            Token::Text => Some("text".to_string()),
            Token::Style => Some("style".to_string()),
            Token::Script => Some("script".to_string()),
            Token::Template => Some("template".to_string()),
            Token::Element => Some("element".to_string()),
            Token::Var => Some("var".to_string()),
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
            Token::Html => Some("html".to_string()),
            Token::JavaScript => Some("javascript".to_string()),
            Token::Chtl => Some("chtl".to_string()),
            Token::CjMod => Some("cjmod".to_string()),
            Token::Config => Some("config".to_string()),
            Token::Name => Some("name".to_string()),
            _ => None,
        }
    }

    fn parse_export_item(&mut self) -> Option<ExportItem> {
        if !self.current_token_is(&Token::LBracket) {
            self.errors.push(format!("Expected '[' at start of export item, got {:?}", self.current_token));
            return None;
        }
        self.next_token();

        let category = match self.current_token.clone() {
            Token::Custom => ImportItemCategory::Custom,
            Token::Template => ImportItemCategory::Template,
            Token::Origin => ImportItemCategory::Origin,
            Token::Configuration => ImportItemCategory::Configuration,
            _ => {
                self.errors.push(format!("Invalid export category: {:?}", self.current_token));
                return None;
            }
        };
        self.next_token();

        if !self.current_token_is(&Token::RBracket) {
            self.errors.push(format!("Expected ']' after export category, got {:?}", self.current_token));
            return None;
        }
        self.next_token();

        if !self.current_token_is(&Token::At) {
            self.errors.push(format!("Expected '@' after export category, got {:?}", self.current_token));
            return None;
        }
        self.next_token();

        let item_type = if let Some(type_str) = self.token_to_string_for_unquoted_literal(&self.current_token) {
            IdentifierExpression { value: type_str }
        } else {
            self.errors.push(format!("Expected item type after @, got {:?}", self.current_token));
            return None;
        };
        self.next_token();

        let mut names = Vec::new();
        loop {
            if let Token::Identifier(name) = self.current_token.clone() {
                names.push(IdentifierExpression { value: name });
            } else {
                self.errors.push(format!("Expected identifier for export name, got {:?}", self.current_token));
                return None;
            }

            self.next_token();

            if self.current_token_is(&Token::Comma) {
                self.next_token();
                continue;
            } else if self.current_token_is(&Token::Semicolon) {
                break;
            } else {
                break;
            }
        }

        Some(ExportItem {
            category,
            item_type,
            names,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::config_manager::ConfigManager;
    use crate::chtl::lexer::lexer::Lexer;

    #[test]
    fn test_script_statement_parsing() {
        let input = r#"
        script {
            console.log("hello");
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let stmt = &program.statements[0];
        if let Statement::Script(script_stmt) = stmt {
            assert_eq!(script_stmt.content.trim(), r#"console.log("hello");"#);
        } else {
            panic!("Expected ScriptStatement, got {:?}", stmt);
        }
    }

    #[test]
    fn test_property_access_expression() {
        let input = r#"
        style {
            width: #box.width;
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
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
                assert_eq!(attr.name.value, "width");
                if let Some(Expression::PropertyAccess(prop_access)) = &attr.value {
                    if let Expression::UnquotedLiteral(obj) = &*prop_access.object {
                        assert_eq!(obj.value, "#box");
                    } else {
                        panic!("Expected UnquotedLiteral for object");
                    }
                    assert_eq!(prop_access.property.value, "width");
                } else {
                    panic!("Expected PropertyAccessExpression");
                }
            } else {
                panic!("Expected AttributeStatement");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }

    #[test]
    fn test_delete_statement_parsing() {
        let input = "delete font-size, @style Base, some-other-prop;";
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let stmt = parser.parse_statement().unwrap();

        if let Statement::Delete(del_stmt) = stmt {
            assert_eq!(del_stmt.targets.len(), 3);
            if let Expression::Identifier(ident) = &del_stmt.targets[0] {
                assert_eq!(ident.value, "font-size");
            } else {
                panic!("Expected Identifier for first delete target");
            }
            if let Expression::UnquotedLiteral(lit) = &del_stmt.targets[1] {
                assert_eq!(lit.value, "@style Base");
            } else {
                panic!("Expected UnquotedLiteral for second delete target");
            }
            if let Expression::Identifier(ident) = &del_stmt.targets[2] {
                assert_eq!(ident.value, "some-other-prop");
            } else {
                panic!("Expected Identifier for third delete target");
            }
        } else {
            panic!("Expected DeleteStatement");
        }
    }

    #[test]
    fn test_if_statement_parsing() {
        let input = r#"
        if {
            condition: a > b;
            display: "block";
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let stmt = parser.parse_statement().unwrap();

        if let Statement::If(if_stmt) = stmt {
            // Check the condition
            if let Expression::Infix(infix) = &if_stmt.condition {
                assert_eq!(infix.operator, ">");
            } else {
                panic!("Expected InfixExpression for condition");
            }

            // Check the consequence
            assert_eq!(if_stmt.consequence.len(), 1);
            if let Statement::Attribute(attr) = &if_stmt.consequence[0] {
                assert_eq!(attr.name.value, "display");
                if let Some(Expression::StringLiteral(s)) = &attr.value {
                    assert_eq!(s.value, "block");
                } else {
                    panic!("Expected StringLiteral for display value");
                }
            } else {
                panic!("Expected AttributeStatement in if consequence");
            }

            // Check that there is no alternative
            assert!(if_stmt.alternative.is_none());
        } else {
            panic!("Expected IfStatement");
        }
    }

    #[test]
    fn test_if_else_statement_parsing() {
        let input = r#"
        if {
            condition: x > y;
            color: "red";
        } else {
            color: "blue";
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let stmt = parser.parse_statement().unwrap();

        if let Statement::If(if_stmt) = stmt {
            // Check the alternative
            assert!(if_stmt.alternative.is_some());
            if let Some(else_stmt_node) = &if_stmt.alternative {
                if let Statement::Else(else_stmt) = &**else_stmt_node {
                    assert_eq!(else_stmt.consequence.len(), 1);
                    if let Statement::Attribute(attr) = &else_stmt.consequence[0] {
                        assert_eq!(attr.name.value, "color");
                    } else {
                        panic!("Expected AttributeStatement in else consequence");
                    }
                } else {
                    panic!("Expected ElseStatement");
                }
            }
        } else {
            panic!("Expected IfStatement");
        }
    }

    #[test]
    fn test_if_else_if_else_statement_parsing() {
        let input = r#"
        if {
            condition: x > y;
            color: "red";
        } else if {
            condition: x < y;
            color: "green";
        } else {
            color: "blue";
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let stmt = parser.parse_statement().unwrap();

        if let Statement::If(if_stmt) = stmt {
            // Check the first `else if`
            assert!(if_stmt.alternative.is_some());
            if let Some(else_if_node) = &if_stmt.alternative {
                if let Statement::If(else_if_stmt) = &**else_if_node {
                    // Check the second `else`
                    assert!(else_if_stmt.alternative.is_some());
                    if let Some(else_node) = &else_if_stmt.alternative {
                        if let Statement::Else(_) = &**else_node {
                            // Correct
                        } else {
                            panic!("Expected ElseStatement at the end");
                        }
                    }
                } else {
                    panic!("Expected IfStatement for the else-if block");
                }
            }
        } else {
            panic!("Expected IfStatement");
        }
    }

    #[test]
    fn test_namespace_statement_parsing() {
        let input = "[namespace] my_space;";
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let stmt = parser.parse_statement().unwrap();

        if let Statement::Namespace(ns_stmt) = stmt {
            assert_eq!(ns_stmt.name.value, "my_space");
        } else {
            panic!("Expected NamespaceStatement");
        }
    }

    #[test]
    fn test_use_template_from_namespace() {
        let input = "@element MyTemplate from my_space;";
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let stmt = parser.parse_statement().unwrap();

        if let Statement::UseTemplate(use_stmt) = stmt {
            assert_eq!(use_stmt.name.value, "MyTemplate");
            assert!(use_stmt.from.is_some());
            assert_eq!(use_stmt.from.unwrap().value, "my_space");
            assert_eq!(use_stmt.template_type.value, "element");
        } else {
            panic!("Expected UseTemplateStatement");
        }
    }

    #[test]
    fn test_use_template_with_specialization() {
        let input = r#"
        style {
            @style MyTemplate {
                color: "blue";
                delete font-size, @style Base;
            }
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            let use_stmt = &style_stmt.body[0];
            if let Statement::UseTemplate(use_template) = use_stmt {
                assert_eq!(use_template.name.value, "MyTemplate");
                assert!(use_template.body.is_some());
                let body = use_template.body.as_ref().unwrap();
                assert_eq!(body.len(), 2);

                // Check attribute override
                if let Statement::Attribute(attr) = &body[0] {
                    assert_eq!(attr.name.value, "color");
                } else {
                    panic!("Expected AttributeStatement");
                }

                // Check delete statement
                if let Statement::Delete(del_stmt) = &body[1] {
                    assert_eq!(del_stmt.targets.len(), 2);
                    if let Expression::Identifier(ident) = &del_stmt.targets[0] {
                        assert_eq!(ident.value, "font-size");
                    } else {
                        panic!("Expected Identifier for first delete target");
                    }
                    if let Expression::UnquotedLiteral(lit) = &del_stmt.targets[1] {
                        assert_eq!(lit.value, "@style Base");
                    } else {
                        panic!("Expected UnquotedLiteral for second delete target");
                    }
                } else {
                    panic!("Expected DeleteStatement");
                }
            } else {
                panic!("Expected UseTemplateStatement");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }

    #[test]
    fn test_parse_program() {
        let input = r#"
        div {
            id: "box";
            text { "Hello" }
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
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
                if let Some(Expression::StringLiteral(s)) = &attr.value {
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
    fn test_element_template_usage_parsing() {
        let input = r#"
        body {
            @element Box;
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let body_stmt = &program.statements[0];
        if let Statement::Element(element) = body_stmt {
            assert_eq!(element.name.value, "body");
            let use_template_stmt = &element.body[0];
            if let Statement::UseTemplate(use_stmt) = use_template_stmt {
                assert_eq!(use_stmt.name.value, "Box");
                assert_eq!(use_stmt.template_type.value, "element");
            } else {
                panic!("Expected UseTemplateStatement");
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
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors.is_empty(), "Parser errors: {:?}", parser.errors);

        let stmt = &program.statements[0];
        if let Statement::Style(style_stmt) = stmt {
            let attr_stmt = &style_stmt.body[0];
            if let Statement::Attribute(attr) = attr_stmt {
                assert_eq!(attr.name.value, "width");
                if let Some(Expression::Infix(infix_exp)) = &attr.value {
                    // 10 + (2 * (3 ** 2))
                    assert_eq!(infix_exp.operator, "+");

                    if let Expression::NumberLiteral(left) = &*infix_exp.left {
                        assert_eq!(left.value, "10");
                        assert_eq!(left.unit, None);
                    } else {
                        panic!("Expected NumberLiteral for left expression");
                    }

                    if let Expression::Infix(right_infix) = &*infix_exp.right {
                        assert_eq!(right_infix.operator, "*");
                        if let Expression::NumberLiteral(right_left) = &*right_infix.left {
                             assert_eq!(right_left.value, "2");
                             assert_eq!(right_left.unit, None);
                        } else {
                             panic!("Expected NumberLiteral for right-left expression");
                        }

                        if let Expression::Infix(power_infix) = &*right_infix.right {
                            assert_eq!(power_infix.operator, "**");
                            if let Expression::NumberLiteral(power_left) = &*power_infix.left {
                                assert_eq!(power_left.value, "3");
                                assert_eq!(power_left.unit, None);
                            }
                            if let Expression::NumberLiteral(power_right) = &*power_infix.right {
                                assert_eq!(power_right.value, "2");
                                assert_eq!(power_right.unit, None);
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
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
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
                if let Some(Expression::Conditional(cond_expr)) = &attr.value {
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
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
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
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
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
                    if let Some(Expression::StringLiteral(s)) = &attr.value {
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
                    if let Some(Expression::NumberLiteral(n)) = &attr.value {
                        assert_eq!(n.value, "16");
                        assert_eq!(n.unit, Some("px".to_string()));
                    } else {
                        panic!("Expected NumberLiteral for font-size value");
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
                    if let Some(Expression::NumberLiteral(n)) = &attr.value {
                        assert_eq!(n.value, "0");
                        assert_eq!(n.unit, None);
                    } else {
                        panic!("Expected NumberLiteral for margin value");
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
        [template] @style DefaultText {
            color: "black";
            line-height: 1.6;
        }

        div {
            style {
                @style DefaultText;
            }
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
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
                    assert_eq!(use_stmt.template_type.value, "style");
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

    #[test]
    fn test_variable_group_template_usage_parsing() {
        let input = r#"
        style {
            color: ThemeColor(tableColor);
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let style_stmt = &program.statements[0];
        if let Statement::Style(style) = style_stmt {
            let attr_stmt = &style.body[0];
            if let Statement::Attribute(attr) = attr_stmt {
                if let Some(Expression::FunctionCall(call)) = &attr.value {
                    if let Expression::Identifier(func_name) = &*call.function {
                        assert_eq!(func_name.value, "ThemeColor");
                    } else {
                        panic!("Expected Identifier for function name");
                    }
                    assert_eq!(call.arguments.len(), 1);
                    if let Expression::Identifier(arg) = &call.arguments[0] {
                        assert_eq!(arg.value, "tableColor");
                    } else {
                        panic!("Expected Identifier for argument");
                    }
                } else {
                    panic!("Expected FunctionCallExpression");
                }
            } else {
                panic!("Expected AttributeStatement");
            }
        } else {
            panic!("Expected StyleStatement");
        }
    }

    #[test]
    fn test_parse_info_statement() {
        let input = r#"
        [info] {
            name: "my-module";
            version: "1.0.0";
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let stmt = &program.statements[0];
        if let Statement::Info(info_stmt) = stmt {
            assert_eq!(info_stmt.attributes.len(), 2);
            assert_eq!(info_stmt.attributes[0].name.value, "name");
            assert_eq!(info_stmt.attributes[1].name.value, "version");
        } else {
            panic!("Expected InfoStatement, got {:?}", stmt);
        }
    }

    #[test]
    fn test_parse_export_statement() {
        let input = r#"
        [export] {
            [custom] @style ChthollyStyle, ChthollyCard;
            [template] @element Box;
        }
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(input, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors().is_empty(),
            "Parser has errors: {:?}",
            parser.errors()
        );

        let stmt = &program.statements[0];
        if let Statement::Export(export_stmt) = stmt {
            assert_eq!(export_stmt.items.len(), 2);

            let item1 = &export_stmt.items[0];
            assert!(matches!(item1.category, ImportItemCategory::Custom));
            assert_eq!(item1.item_type.value, "style");
            assert_eq!(item1.names.len(), 2);
            assert_eq!(item1.names[0].value, "ChthollyStyle");
            assert_eq!(item1.names[1].value, "ChthollyCard");

            let item2 = &export_stmt.items[1];
            assert!(matches!(item2.category, ImportItemCategory::Template));
            assert_eq!(item2.item_type.value, "element");
            assert_eq!(item2.names.len(), 1);
            assert_eq!(item2.names[0].value, "Box");
        } else {
            panic!("Expected ExportStatement, got {:?}", stmt);
        }
    }
}