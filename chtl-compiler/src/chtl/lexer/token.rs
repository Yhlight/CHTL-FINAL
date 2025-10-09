#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // General
    Illegal,
    Eof,

    // Identifiers & Literals
    Identifier(String),
    String(String),
    Number(String),

    // Operators
    Colon,       // :
    Semicolon,   // ;
    LParen,      // (
    RParen,      // )
    LBrace,      // {
    RBrace,      // }
    LBracket,    // [
    RBracket,    // ]
    Comma,       // ,
    Dot,         // .
    Plus,        // +
    Minus,       // -
    Asterisk,    // *
    Slash,       // /
    Percent,     // %
    Power,       // **
    Gt,          // >
    Lt,          // <
    Bang,        // !
    Ampersand,   // &
    At,          // @
    Hash,        // #

    // Keywords
    Style,
    Script,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration,
    Use,
    If,
    Else,
    Except,
    Inherit,
    Delete,
    Insert,
    After,
    Before,
    Replace,
    AtTop,
    AtBottom,
    From,
    As,

    // Comments
    SingleLineComment(String),
    MultiLineComment(String),
    GeneratorComment(String),
}