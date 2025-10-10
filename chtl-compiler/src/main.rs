use std::env;
use std::fs;

pub mod chtl;
use chtl::config_manager::ConfigurationManager;
use chtl::lexer::lexer::Lexer;
use chtl::node::ast::Statement;
use chtl::parser::parser::Parser;
use chtl::generator::generator::Generator;

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

    // Pre-pass to extract configuration
    let mut config_manager = ConfigurationManager::new();
    let pre_lexer = Lexer::new(&source);
    let mut pre_parser = Parser::new(pre_lexer);
    let pre_program = pre_parser.parse_program();

    for stmt in &pre_program.statements {
        if let Statement::Configuration(config_stmt) = stmt {
            // We only apply the first unnamed configuration block for now.
            if config_stmt.name.is_none() {
                config_manager.apply_config(config_stmt);
                break;
            }
        }
    }

    // Main pass with the loaded configuration
    let lexer = Lexer::new(&source); // In the future, this will take the config_manager
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