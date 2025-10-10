use std::env;
use std::fs;

pub mod chtl;
use chtl::config_manager::ConfigManager;
use chtl::generator::generator::Generator;
use chtl::lexer::lexer::Lexer;
use chtl::node::ast::Statement;
use chtl::parser::parser::Parser;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: chtl-compiler <file>");
        return;
    }

    let filename = &args[1];
    let source = match fs::read_to_string(filename) {
        Ok(s) => s,
        Err(e) => {
            eprintln!("Error reading file '{}': {}", filename, e);
            return;
        }
    };

    // --- Pre-parsing pass for configuration ---
    let mut config_manager = ConfigManager::new();
    // Use a temporary default config for the pre-pass itself, since we don't know the config yet.
    let temp_config = ConfigManager::new();
    let pre_lexer = Lexer::new(&source, &temp_config);
    let mut pre_parser = Parser::new(pre_lexer);
    let pre_program = pre_parser.parse_program();

    if !pre_parser.errors().is_empty() {
        // We don't want to halt on pre-parsing errors, but we might want to log them.
        // For now, we'll ignore them as the main pass will catch them anyway.
    }

    // Find the first unnamed [Configuration] block and apply it.
    for stmt in &pre_program.statements {
        if let Statement::Configuration(config_stmt) = stmt {
            if config_stmt.name.is_none() {
                config_manager.apply_config(config_stmt);
                break; // Only the first unnamed config is used
            }
        }
    }

    // --- Main parsing pass ---
    // Now, create the main lexer with the potentially updated configuration.
    let lexer = Lexer::new(&source, &config_manager);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    if !parser.errors().is_empty() {
        eprintln!("Parser errors:");
        for error in parser.errors() {
            eprintln!("\t{}", error);
        }
        return;
    }

    let mut generator = Generator::new();
    let html = generator.generate(&program);
    println!("{}", html);
}