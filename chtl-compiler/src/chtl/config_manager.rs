use crate::chtl::lexer::token::Token;
use crate::chtl::node::ast::{
    AttributeStatement, ConfigurationStatement, Expression, NameBlock, Statement,
};
use std::collections::HashMap;

#[derive(Debug, Clone, PartialEq)]
pub struct ConfigManager {
    pub keyword_tokens: HashMap<String, Token>, // Maps user-facing string to Token
    pub keywords: HashMap<String, String>,       // Maps internal keyword name to its string representation
    pub index_initial_count: i64,
    pub debug_mode: bool,
    pub disable_name_group: bool,
}

impl ConfigManager {
    pub fn new() -> Self {
        let mut keywords = HashMap::new();
        let mut keyword_tokens = HashMap::new();

        macro_rules! add_keyword {
            ($k_map:expr, $kt_map:expr, $internal:expr, $default:expr, $token:expr) => {
                $k_map.insert($internal.to_string(), $default.to_string());
                $kt_map.insert($default.to_lowercase(), $token);
            };
        }

        add_keyword!(keywords, keyword_tokens, "KEYWORD_TEXT", "text", Token::Text);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_STYLE", "style", Token::Style);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_SCRIPT", "script", Token::Script);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_TEMPLATE", "template", Token::Template);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_ELEMENT", "element", Token::Element);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_VAR", "var", Token::Var);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_CUSTOM", "custom", Token::Custom);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_ORIGIN", "origin", Token::Origin);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_IMPORT", "import", Token::Import);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_NAMESPACE", "namespace", Token::Namespace);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_CONFIGURATION", "configuration", Token::Configuration);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_USE", "use", Token::Use);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_IF", "if", Token::If);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_ELSE", "else", Token::Else);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_EXCEPT", "except", Token::Except);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_INHERIT", "inherit", Token::Inherit);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_DELETE", "delete", Token::Delete);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_INSERT", "insert", Token::Insert);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_AFTER", "after", Token::After);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_BEFORE", "before", Token::Before);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_REPLACE", "replace", Token::Replace);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_FROM", "from", Token::From);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_AS", "as", Token::As);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_HTML", "html", Token::Html);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_JAVASCRIPT", "javascript", Token::JavaScript);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_CHTL", "chtl", Token::Chtl);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_CJMOD", "cjmod", Token::CjMod);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_CONFIG", "config", Token::Config);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_INFO", "info", Token::Info);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_EXPORT", "export", Token::Export);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_NAME", "name", Token::Name);
        add_keyword!(keywords, keyword_tokens, "KEYWORD_HTML5", "html5", Token::Html);

        ConfigManager {
            keywords,
            keyword_tokens,
            index_initial_count: 0,
            debug_mode: false,
            disable_name_group: false,
        }
    }

    pub fn apply_config(&mut self, config_stmt: &ConfigurationStatement) {
        for config_item in &config_stmt.body {
            match config_item {
                Statement::Attribute(attr) => self.apply_attribute(attr),
                Statement::NameBlock(name_block) => {
                    if !self.disable_name_group {
                        self.apply_name_block(name_block)
                    }
                }
                _ => {}
            }
        }
    }

    fn apply_attribute(&mut self, attr: &AttributeStatement) {
        if let Some(value_expr) = &attr.value {
            match attr.name.value.as_str() {
                "INDEX_INITIAL_COUNT" => {
                    if let Expression::NumberLiteral(num) = value_expr {
                        if let Ok(val) = num.value.parse::<i64>() {
                            self.index_initial_count = val;
                        }
                    }
                }
                "DEBUG_MODE" => {
                    if let Expression::Identifier(ident) = value_expr {
                        if let Ok(val) = ident.value.to_lowercase().parse::<bool>() {
                            self.debug_mode = val;
                        }
                    }
                }
                "DISABLE_NAME_GROUP" => {
                    if let Expression::Identifier(ident) = value_expr {
                        if let Ok(val) = ident.value.to_lowercase().parse::<bool>() {
                            self.disable_name_group = val;
                        }
                    }
                }
                _ => {}
            }
        }
    }

    fn apply_name_block(&mut self, name_block: &NameBlock) {
        for setting in &name_block.settings {
            if let Some(value) = &setting.value {
                let internal_name = setting.name.value.clone();

                // Find the token associated with this internal keyword name.
                // We do this by looking up the current string representation...
                let old_keyword_string = self.keywords.get(&internal_name).cloned();

                // ...and then finding and removing its token from the map.
                let token = if let Some(ref old_str) = old_keyword_string {
                    self.keyword_tokens.remove(&old_str.to_lowercase())
                } else {
                    None
                };

                // If we didn't find a token, we can't proceed with this setting.
                if let Some(token) = token {
                    match value {
                        Expression::ArrayLiteral(array) => {
                            let mut first_keyword = None;

                            for element in &array.elements {
                                let keyword_str = match element {
                                    Expression::UnquotedLiteral(val) => Some(val.value.clone()),
                                    Expression::Identifier(val) => Some(val.value.clone()),
                                    Expression::StringLiteral(val) => Some(val.value.clone()),
                                    _ => None,
                                };

                                if let Some(s) = keyword_str {
                                    if first_keyword.is_none() {
                                        first_keyword = Some(s.clone());
                                    }
                                    self.keyword_tokens.insert(s.to_lowercase(), token.clone());
                                }
                            }

                            // Update the primary keyword map to point to the first item in the array.
                            if let Some(s) = first_keyword {
                                self.keywords.insert(internal_name, s);
                            } else if let Some(old_str) = old_keyword_string {
                                // If array was empty or invalid, restore the old token mapping.
                                self.keyword_tokens.insert(old_str.to_lowercase(), token);
                            }
                        }
                        _ => { // Handle single values
                            let keyword_str = match value {
                                Expression::UnquotedLiteral(val) => Some(val.value.clone()),
                                Expression::Identifier(val) => Some(val.value.clone()),
                                Expression::StringLiteral(val) => Some(val.value.clone()),
                                _ => None,
                            };

                            if let Some(s) = keyword_str {
                                self.keyword_tokens.insert(s.to_lowercase(), token);
                                self.keywords.insert(internal_name, s);
                            } else if let Some(old_str) = old_keyword_string {
                                // If expression was invalid, restore the old token mapping.
                                self.keyword_tokens.insert(old_str.to_lowercase(), token);
                            }
                        }
                    }
                }
            }
        }
    }
}

impl Default for ConfigManager {
    fn default() -> Self {
        Self::new()
    }
}