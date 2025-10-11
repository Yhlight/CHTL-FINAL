use std::fs;
use std::path::Path;

use clap::Parser as ClapParser;

pub mod chtl;
use chtl::config_manager::ConfigManager;
use chtl::generator::generator::Generator;
use chtl::lexer::lexer::Lexer;
use chtl::node::ast::Statement;
use chtl::parser::parser::Parser;

/// A simple CHTL compiler
#[derive(ClapParser, Debug)]
#[command(author, version, about, long_about = None)]
struct Cli {
    /// The CHTL file to compile
    input: String,

    /// The output file path
    #[arg(short, long)]
    output: Option<String>,
}

fn main() {
    let cli = Cli::parse();

    let filename = &cli.input;
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

    // Find all named configs and the first unnamed one.
    let mut named_configs = std::collections::HashMap::new();
    let mut unnamed_config = None;
    for stmt in &pre_program.statements {
        if let Statement::Configuration(config_stmt) = stmt {
            if let Some(name) = &config_stmt.name {
                named_configs.insert(name.value.clone(), config_stmt.clone());
            } else if unnamed_config.is_none() {
                unnamed_config = Some(config_stmt.clone());
            }
        }
    }

    // Check for a `use @Config <name>;` statement.
    let mut use_config_name = None;
    for stmt in &pre_program.statements {
        if let Statement::Use(use_stmt) = stmt {
            if let chtl::node::ast::UseTarget::Config(config_name) = &use_stmt.target {
                use_config_name = Some(config_name.value.clone());
                break;
            }
        }
    }

    if let Some(config_name) = use_config_name {
        if let Some(config_stmt) = named_configs.get(&config_name) {
            config_manager.apply_config(config_stmt);
        } else {
            eprintln!("Warning: Configuration '{}' specified in 'use' statement not found.", config_name);
            // Fallback to unnamed config if the named one isn't found
            if let Some(config_stmt) = unnamed_config {
                config_manager.apply_config(&config_stmt);
            }
        }
    } else if let Some(config_stmt) = unnamed_config {
        // Default behavior: apply the first unnamed config if no `use` statement is present
        config_manager.apply_config(&config_stmt);
    }

    // --- Main parsing pass ---
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

    let mut generator = Generator::new(Some(filename));
    let html = generator.generate(&program);

    // Determine the output path
    let output_path = match cli.output {
        Some(path) => path,
        None => {
            let path = Path::new(filename);
            path.with_extension("html").to_str().unwrap().to_string()
        }
    };

    // Write the output
    match fs::write(&output_path, html) {
        Ok(_) => println!("Successfully compiled {} to {}", filename, output_path),
        Err(e) => eprintln!("Error writing to file '{}': {}", output_path, e),
    }
}