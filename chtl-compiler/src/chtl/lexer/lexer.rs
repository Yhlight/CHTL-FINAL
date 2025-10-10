use crate::chtl::lexer::token::Token;
use std::iter::Peekable;
use std::str::Chars;

pub struct Lexer<'a> {
    input: Peekable<Chars<'a>>,
    ch: char,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
            input: input.chars().peekable(),
            ch: ' ',
        };
        l.read_char();
        l
    }

    fn read_char(&mut self) {
        self.ch = self.input.next().unwrap_or('\0');
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let tok = match self.ch {
            '=' | ':' => Token::Colon,
            ';' => Token::Semicolon,
            '?' => Token::Question,
            '(' => Token::LParen,
            ')' => Token::RParen,
            '{' => Token::LBrace,
            '}' => Token::RBrace,
            '[' => Token::LBracket,
            ']' => Token::RBracket,
            ',' => Token::Comma,
            '.' => Token::Dot,
            '+' => Token::Plus,
            '-' => Token::Minus,
            '/' => {
                if let Some(&c) = self.input.peek() {
                    if c == '/' {
                        self.read_char(); // consume first /
                        self.read_char(); // consume second /
                        return Token::SingleLineComment(self.read_single_line_comment());
                    } else if c == '*' {
                        self.read_char(); // consume /
                        self.read_char(); // consume *
                        return Token::MultiLineComment(self.read_multi_line_comment());
                    }
                }
                Token::Slash
            }
            '*' => {
                if let Some(&'*') = self.input.peek() {
                    self.read_char();
                    Token::Power
                } else {
                    Token::Asterisk
                }
            }
            '%' => Token::Percent,
            '>' => Token::Gt,
            '<' => Token::Lt,
            '!' => Token::Bang,
            '&' => Token::Ampersand,
            '@' => Token::At,
            '#' => {
                if let Some(&' ') = self.input.peek() {
                    self.read_char(); // consume #
                    self.read_char(); // consume space
                    return Token::GeneratorComment(self.read_single_line_comment());
                } else {
                    Token::Hash
                }
            }
            '"' | '\'' => {
                return self.read_string();
            }
            '\0' => Token::Eof,
            _ => {
                if is_letter(self.ch) {
                    let literal = self.read_identifier();
                    return match literal.to_lowercase().as_str() {
                        "text" => Token::Text,
                        "style" => Token::Style,
                        "script" => Token::Script,
                        "template" => Token::Template,
                        "element" => Token::Element,
                        "var" => Token::Var,
                        "custom" => Token::Custom,
                        "origin" => Token::Origin,
                        "import" => Token::Import,
                        "namespace" => Token::Namespace,
                        "configuration" => Token::Configuration,
                        "use" => Token::Use,
                        "if" => Token::If,
                        "else" => Token::Else,
                        "except" => Token::Except,
                        "inherit" => Token::Inherit,
                        "delete" => Token::Delete,
                        "insert" => Token::Insert,
                        "after" => Token::After,
                        "before" => Token::Before,
                        "replace" => Token::Replace,
                        "at" => {
                            let start_ch = self.ch;
                            let start_input = self.input.clone();
                            self.skip_whitespace();
                            if !is_letter(self.ch) {
                                self.ch = start_ch;
                                self.input = start_input;
                                return Token::Identifier(literal);
                            }
                            let next_word = self.read_identifier();
                            if next_word.to_lowercase() == "top" {
                                Token::AtTop
                            } else if next_word.to_lowercase() == "bottom" {
                                Token::AtBottom
                            } else {
                                self.ch = start_ch;
                                self.input = start_input;
                                Token::Identifier(literal)
                            }
                        }
                        "from" => Token::From,
                        "as" => Token::As,
                        "html" => Token::Html,
                        "javascript" => Token::JavaScript,
                        "chtl" => Token::Chtl,
                        "cjmod" => Token::CjMod,
                        "config" => Token::Config,
                        _ => Token::Identifier(literal),
                    };
                } else if is_digit(self.ch) {
                    return self.read_number();
                } else {
                    Token::Illegal
                }
            }
        };

        self.read_char();
        tok
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_whitespace() {
            self.read_char();
        }
    }

    fn read_identifier(&mut self) -> String {
        let mut identifier = String::new();
        while is_letter(self.ch) || is_digit(self.ch) {
            identifier.push(self.ch);
            self.read_char();
        }
        identifier
    }

    fn read_number(&mut self) -> Token {
        let mut number = String::new();
        while is_digit(self.ch) || self.ch == '.' {
            number.push(self.ch);
            self.read_char();
        }

        let unit = self.read_unit();

        Token::Number(number, unit)
    }

    fn read_unit(&mut self) -> Option<String> {
        // CSS units can contain letters or a percentage sign.
        // We'll keep it simple and read any sequence of letters.
        if is_letter(self.ch) || self.ch == '%' {
            let mut unit = String::new();
            while is_letter(self.ch) || self.ch == '%' {
                unit.push(self.ch);
                self.read_char();
            }
            return Some(unit);
        }
        None
    }

    fn read_string(&mut self) -> Token {
        let quote_type = self.ch;
        let mut value = String::new();
        self.read_char(); // Consume opening quote
        while self.ch != quote_type && self.ch != '\0' {
            value.push(self.ch);
            self.read_char();
        }
        self.read_char(); // Consume closing quote
        Token::String(value)
    }

    fn read_single_line_comment(&mut self) -> String {
        let mut comment = String::new();
        while self.ch != '\n' && self.ch != '\0' {
            comment.push(self.ch);
            self.read_char();
        }
        comment
    }

    fn read_multi_line_comment(&mut self) -> String {
        let mut comment = String::new();
        while !(self.ch == '*' && self.input.peek() == Some(&'/')) {
            if self.ch == '\0' {
                return comment;
            }
            comment.push(self.ch);
            self.read_char();
        }
        self.read_char(); // consume '*'
        self.read_char(); // consume '/'
        comment
    }
}

fn is_letter(ch: char) -> bool {
    ch.is_alphabetic() || ch == '_' || ch == '-'
}

fn is_digit(ch: char) -> bool {
    ch.is_ascii_digit()
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl::lexer::token::Token;

    #[test]
    fn test_next_token() {
        let input = r#"
        div {
            id: "box";
            class: welcome;
            text { "Hello World" }
        }

        // This is a comment
        /* This is a multi-line comment */
        # This is a generator comment

        style {
            .box {
                width: 100px;
                height: 200.5px;
            }
        }
        "#;

        let tests = vec![
            Token::Identifier("div".to_string()),
            Token::LBrace,
            Token::Identifier("id".to_string()),
            Token::Colon,
            Token::String("box".to_string()),
            Token::Semicolon,
            Token::Identifier("class".to_string()),
            Token::Colon,
            Token::Identifier("welcome".to_string()),
            Token::Semicolon,
            Token::Text,
            Token::LBrace,
            Token::String("Hello World".to_string()),
            Token::RBrace,
            Token::RBrace,
            Token::SingleLineComment(" This is a comment".to_string()),
            Token::MultiLineComment(" This is a multi-line comment ".to_string()),
            Token::GeneratorComment("This is a generator comment".to_string()),
            Token::Style,
            Token::LBrace,
            Token::Dot,
            Token::Identifier("box".to_string()),
            Token::LBrace,
            Token::Identifier("width".to_string()),
            Token::Colon,
            Token::Number("100".to_string(), Some("px".to_string())),
            Token::Semicolon,
            Token::Identifier("height".to_string()),
            Token::Colon,
            Token::Number("200.5".to_string(), Some("px".to_string())),
            Token::Semicolon,
            Token::RBrace,
            Token::RBrace,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);

        for expected_token in tests {
            let tok = lexer.next_token();
            assert_eq!(tok, expected_token);
        }
    }

    #[test]
    fn test_ce_equivalence() {
        let input = "id: box; class = container;";
        let tests = vec![
            Token::Identifier("id".to_string()),
            Token::Colon,
            Token::Identifier("box".to_string()),
            Token::Semicolon,
            Token::Identifier("class".to_string()),
            Token::Colon,
            Token::Identifier("container".to_string()),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);
        for expected_token in tests {
            let tok = lexer.next_token();
            assert_eq!(tok, expected_token);
        }
    }

    #[test]
    fn test_text_keyword() {
        let input = "text { \"some text\" }";
        let tests = vec![
            Token::Text,
            Token::LBrace,
            Token::String("some text".to_string()),
            Token::RBrace,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);
        for expected_token in tests {
            let tok = lexer.next_token();
            assert_eq!(tok, expected_token);
        }
    }

    #[test]
    fn test_percentage_unit() {
        let input = "width: 50%;";
        let tests = vec![
            Token::Identifier("width".to_string()),
            Token::Colon,
            Token::Number("50".to_string(), Some("%".to_string())),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);
        for expected_token in tests {
            let tok = lexer.next_token();
            assert_eq!(tok, expected_token);
        }
    }

    #[test]
    fn test_import_keywords() {
        let input = r#"
        [Import] @Html from "a.html" as A;
        [Import] @JavaScript from "b.js" as B;
        [Import] @Chtl from "c.chtl" as C;
        [Import] @CJmod from "d.cjmod" as D;
        [Import] @Config from "e.chtl" as E;
        "#;

        let tests = vec![
            // First line
            Token::LBracket,
            Token::Import,
            Token::RBracket,
            Token::At,
            Token::Html,
            Token::From,
            Token::String("a.html".to_string()),
            Token::As,
            Token::Identifier("A".to_string()),
            Token::Semicolon,
            // Second line
            Token::LBracket,
            Token::Import,
            Token::RBracket,
            Token::At,
            Token::JavaScript,
            Token::From,
            Token::String("b.js".to_string()),
            Token::As,
            Token::Identifier("B".to_string()),
            Token::Semicolon,
            // Third line
            Token::LBracket,
            Token::Import,
            Token::RBracket,
            Token::At,
            Token::Chtl,
            Token::From,
            Token::String("c.chtl".to_string()),
            Token::As,
            Token::Identifier("C".to_string()),
            Token::Semicolon,
            // Fourth line
            Token::LBracket,
            Token::Import,
            Token::RBracket,
            Token::At,
            Token::CjMod,
            Token::From,
            Token::String("d.cjmod".to_string()),
            Token::As,
            Token::Identifier("D".to_string()),
            Token::Semicolon,
            // Fifth line
            Token::LBracket,
            Token::Import,
            Token::RBracket,
            Token::At,
            Token::Config,
            Token::From,
            Token::String("e.chtl".to_string()),
            Token::As,
            Token::Identifier("E".to_string()),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);

        for expected_token in tests {
            let tok = lexer.next_token();
            assert_eq!(tok, expected_token);
        }
    }
}