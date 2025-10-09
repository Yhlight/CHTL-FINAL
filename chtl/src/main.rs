use std::env;
use std::fs;
use chtl::chtl::{
    ast::Node,
    generator::Generator,
    lexer::Lexer,
    parser::Parser,
};

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: chtl <file>");
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

    let lexer = Lexer::new(&source);
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
    let node = Node::Program(program);
    let html = generator.generate(&node);

    println!("{}", html);
}