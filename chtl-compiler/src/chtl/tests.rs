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

    #[test]
    fn test_unquoted_text_statement() {
        let input = r#"
        div {
            text {
                unquoted text
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

        let mut generator = crate::chtl::generator::generator::Generator::new(None);
        let html = generator.generate(&program);

        assert_eq!(html.trim(), "<div>unquoted text</div>");
    }

    #[test]
    fn test_multi_value_keyword_config() {
        let source = r#"
        [configuration] {
            [name] {
                KEYWORD_STYLE = ["css", "style", "schtyle"];
            }
        }

        css {}
        style {}
        schtyle {}
        "#;

        let mut config_manager = ConfigManager::new();
        let pre_lexer = Lexer::new(&source, &config_manager);
        let mut pre_parser = Parser::new(pre_lexer);
        let pre_program = pre_parser.parse_program();

        assert!(pre_parser.errors().is_empty(), "Pre-parser has errors: {:?}", pre_parser.errors());

        for stmt in &pre_program.statements {
            if let Statement::Configuration(config_stmt) = stmt {
                if config_stmt.name.is_none() {
                    config_manager.apply_config(config_stmt);
                    break;
                }
            }
        }

        assert_eq!(config_manager.keyword_tokens.get("css"), Some(&crate::chtl::lexer::token::Token::Style));
        assert_eq!(config_manager.keyword_tokens.get("style"), Some(&crate::chtl::lexer::token::Token::Style));
        assert_eq!(config_manager.keyword_tokens.get("schtyle"), Some(&crate::chtl::lexer::token::Token::Style));

        let main_lexer = Lexer::new(&source, &config_manager);
        let mut main_parser = Parser::new(main_lexer);
        main_parser.parse_program();

        assert!(main_parser.errors().is_empty(), "Main parser has errors: {:?}", main_parser.errors());
    }
}
