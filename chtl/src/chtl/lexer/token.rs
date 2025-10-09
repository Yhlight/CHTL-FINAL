#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Literals
    Identifier(String),
    String(String),
    Number(f64),

    // Keywords
    Text,
    Style,
    Script,
    Template,
    Custom,
    Origin,
    Element,
    Import,
    Namespace,
    Configuration,
    Use,
    Html5,
    Inherit,
    Delete,
    Insert,
    From,
    As,
    Except,

    // Symbols
    Colon,
    Equal,
    LBrace,
    RBrace,
    LParen,
    RParen,
    LBracket,
    RBracket,
    Semicolon,
    Comma,
    Dot,
    At,
    Hash,
    Ampersand,
    Dollar,

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Power,
    Question,
    LogicalAnd,
    LogicalOr,
    GreaterThan,
    LessThan,
    GreaterThanOrEqual,
    LessThanOrEqual,

    // Comments
    LineComment(String),
    BlockComment(String),
    GeneratorComment(String),

    // Special
    CHTLJSStart, // {{
    CHTJSEnd,    // }}

    // End of File
    EOF,
}