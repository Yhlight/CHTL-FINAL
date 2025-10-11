#[cfg(test)]
mod tests {
    use crate::chtl::config_manager::ConfigManager;
    use crate::chtl::lexer::lexer::Lexer;
    use crate::chtl::node::ast::Statement;
    use crate::chtl::parser::parser::Parser;

    #[test]
    fn test_pre_parsing_and_config_application() {
        let source = r#"
        [configuration] {
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

    #[test]
    fn test_use_html5_generation() {
        let source = r#"
        use html5;
        div {}
        "#;
        let config = ConfigManager::new();
        let lexer = Lexer::new(&source, &config);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors().is_empty(), "Parser errors: {:?}", parser.errors());

        let mut generator = crate::chtl::generator::generator::Generator::new(None);
        let html = generator.generate(&program);

        assert!(html.starts_with("<!DOCTYPE html>"));
    }
}
