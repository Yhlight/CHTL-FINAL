use std::collections::HashMap;
use crate::chtl::node::ast::{ConfigurationStatement, Expression};

#[derive(Debug, PartialEq, Clone)]
pub struct KeywordConfig {
    // Define fields for each customizable keyword
    pub custom_style: Vec<String>,
    // Add other keywords here...
}

impl Default for KeywordConfig {
    fn default() -> Self {
        Self {
            // Default CHTL keywords
            custom_style: vec!["@Style".to_string(), "@style".to_string(), "@CSS".to_string(), "@Css".to_string(), "@css".to_string()],
            // Initialize other keywords...
        }
    }
}

#[derive(Debug, PartialEq, Clone)]
pub struct ConfigurationManager {
    pub index_initial_count: i64,
    pub debug_mode: bool,
    pub keywords: KeywordConfig,
    // A map for other, non-keyword settings
    pub other_settings: HashMap<String, String>,
}

impl Default for ConfigurationManager {
    fn default() -> Self {
        Self {
            index_initial_count: 0,
            debug_mode: false,
            keywords: KeywordConfig::default(),
            other_settings: HashMap::new(),
        }
    }
}

impl ConfigurationManager {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn apply_config(&mut self, config_stmt: &ConfigurationStatement) {
        for setting in &config_stmt.settings {
            let key = &setting.name.value;
            if let Some(value_expr) = &setting.value {
                // For now, we'll just convert the expression to a string.
                // A more robust implementation would evaluate it to the correct type.
                let value_str = match value_expr {
                    Expression::NumberLiteral(n) => n.value.clone(),
                    Expression::Identifier(i) => i.value.clone(),
                    Expression::StringLiteral(s) => s.value.clone(),
                    _ => "".to_string(), // Or handle error
                };

                match key.as_str() {
                    "INDEX_INITIAL_COUNT" => {
                        if let Ok(val) = value_str.parse::<i64>() {
                            self.index_initial_count = val;
                        }
                    }
                    "DEBUG_MODE" => {
                        if let Ok(val) = value_str.parse::<bool>() {
                            self.debug_mode = val;
                        }
                    }
                    // We will handle keyword configs later when we parse the [Name] block
                    _ => {
                        self.other_settings.insert(key.clone(), value_str);
                    }
                }
            }
        }
    }
}