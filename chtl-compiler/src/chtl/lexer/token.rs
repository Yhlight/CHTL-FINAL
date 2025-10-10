#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // General
    Illegal,
    Eof,

    // Identifiers & Literals
    Identifier(String),
    String(String),
    Number(String, Option<String>),

    // Operators
    Colon,       // :
    Semicolon,   // ;
    Question,    // ?
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
    And,         // &&
    Or,          // ||

    // Keywords
    Text,
    Style,
    Script,
    Template,
    Element,
    Var,
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
    Html,
    JavaScript,
    Chtl,
    CjMod,
    Config,
    Info,
    Export,
    Name,

    // Comments
    SingleLineComment(String),
    MultiLineComment(String),
    GeneratorComment(String),
}