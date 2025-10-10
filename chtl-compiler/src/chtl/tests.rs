#[cfg(test)]
mod tests {
    use crate::chtl::config_manager::ConfigManager;
    use crate::chtl::lexer::lexer::Lexer;
    use crate::chtl::node::ast::Statement;
    use crate::chtl::parser::parser::Parser;

    #[test]
    fn test_pre_parsing_and_config_application() {
        let source = r#"
        [Configuration] {
            INDEX_INITIAL_COUNT: 1;
            DEBUG_MODE: true;
        }

        div {}
        "#;

        // --- Pre-parsing pass for configuration ---
        let mut config_manager = ConfigManager::new();
        let pre_lexer = Lexer::new(&source, &config_manager);
        let mut pre_parser = Parser::new(pre_lexer);
        let pre_program = pre_parser.parse_program();

        // Find the first unnamed [Configuration] block and apply it.
        for stmt in &pre_program.statements {
            if let Statement::Configuration(config_stmt) = stmt {
                if config_stmt.name.is_none() {
                    config_manager.apply_config(config_stmt);
                    break;
                }
            }
        }

        assert_eq!(config_manager.index_initial_count, 1);
        assert_eq!(config_manager.debug_mode, true);
    }
}