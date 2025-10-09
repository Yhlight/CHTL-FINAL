pub mod token;

use self::token::Token;

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut lexer = Lexer {
            input,
            position: 0,
            read_position: 0,
            ch: 0,
        };
        lexer.read_char();
        lexer
    }

    fn read_char(&mut self) {
        if self.read_position >= self.input.len() {
            self.ch = 0;
        } else {
            self.ch = self.input.as_bytes()[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let token = match self.ch {
            b'=' => Token::Equal,
            b':' => Token::Colon,
            b';' => Token::Semicolon,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'[' => Token::LBracket,
            b']' => Token::RBracket,
            b',' => Token::Comma,
            b'.' => Token::Dot,
            b'@' => Token::At,
            b'#' => {
                if self.peek_char() == b' ' {
                    // Consume '#'
                    self.read_char();
                    // Consume ' '
                    self.read_char();

                    let position = self.position;
                    while self.ch != b'\n' && self.ch != 0 {
                        self.read_char();
                    }
                    let comment = self.input[position..self.position].to_string();
                    return Token::GeneratorComment(comment);
                } else {
                    Token::Hash
                }
            },
            b'&' => Token::Ampersand,
            b'$' => Token::Dollar,
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'*' => Token::Star,
            b'/' => {
                if self.peek_char() == b'/' {
                    self.read_char();
                    let comment = self.read_line_comment();
                    return Token::LineComment(comment);
                } else if self.peek_char() == b'*' {
                    self.read_char();
                    let comment = self.read_block_comment();
                    return Token::BlockComment(comment);
                } else {
                    Token::Slash
                }
            }
            b'%' => Token::Percent,
            b'?' => Token::Question,
            b'>' => Token::GreaterThan,
            b'<' => Token::LessThan,
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => {
                let ident = self.read_identifier();
                return match ident.as_str() {
                    "text" => Token::Text,
                    "style" | "Style" => Token::Style,
                    "script" => Token::Script,
                    "Template" => Token::Template,
                    "Custom" => Token::Custom,
                    "Origin" => Token::Origin,
                    "Element" => Token::Element,
                    "Import" => Token::Import,
                    "Namespace" => Token::Namespace,
                    "Configuration" => Token::Configuration,
                    "use" => Token::Use,
                    "html5" => Token::Html5,
                    "inherit" => Token::Inherit,
                    "delete" => Token::Delete,
                    "insert" => Token::Insert,
                    "from" => Token::From,
                    "as" => Token::As,
                    "except" => Token::Except,
                    _ => Token::Identifier(ident),
                };
            }
            b'0'..=b'9' => {
                return Token::Number(self.read_number());
            }
            b'"' => {
                return Token::String(self.read_string());
            }
            0 => Token::EOF,
            _ => Token::Identifier(format!("Illegal character: {}", self.ch as char)),
        };

        self.read_char();
        token
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' || self.ch == b'-' {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> f64 {
        let position = self.position;
        while self.ch.is_ascii_digit() || self.ch == b'.' {
            self.read_char();
        }
        self.input[position..self.position].parse().unwrap_or(0.0)
    }

    fn read_string(&mut self) -> String {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        let value = self.input[position..self.position].to_string();
        if self.ch == b'"' {
            self.read_char(); // Consume the closing quote
        }
        value
    }

    fn read_line_comment(&mut self) -> String {
        let position = self.position + 1;
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_block_comment(&mut self) -> String {
        let position = self.position + 1;
        loop {
            self.read_char();
            if (self.ch == b'*' && self.peek_char() == b'/') || self.ch == 0 {
                break;
            }
        }
        let comment = self.input[position..self.position].to_string();
        self.read_char();
        self.read_char();
        comment
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }

    pub fn read_raw_block_content(&mut self) -> String {
        let position = self.position;
        let mut brace_level = 1;

        while self.ch != 0 && brace_level > 0 {
            if self.ch == b'{' {
                brace_level += 1;
            } else if self.ch == b'}' {
                brace_level -= 1;
            }
            self.read_char();
        }

        if self.position > position && brace_level == 0 {
            self.input[position..self.position - 1].to_string()
        } else {
            "".to_string()
        }
    }
}

#[cfg(test)]
mod tests {
    use super::token::Token;
    use super::Lexer;

    #[test]
    fn test_next_token() {
        let input = r#"
            div {
                text: "Hello, World!";
                // This is a comment
            }
        "#;

        let tests = vec![
            Token::Identifier("div".to_string()),
            Token::LBrace,
            Token::Text,
            Token::Colon,
            Token::String("Hello, World!".to_string()),
            Token::Semicolon,
            Token::LineComment(" This is a comment".to_string()),
            Token::RBrace,
            Token::EOF,
        ];

        let mut lexer = Lexer::new(input);

        for expected_token in tests {
            let token = lexer.next_token();
            assert_eq!(token, expected_token);
        }
    }
}