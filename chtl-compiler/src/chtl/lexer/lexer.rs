use crate::chtl::lexer::token::Token;

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,      // current position in input (points to current char)
    read_position: usize, // current reading position in input (after current char)
    ch: u8,               // current char under examination
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
            input,
            position: 0,
            read_position: 0,
            ch: 0,
        };
        l.read_char();
        l
    }

    fn read_char(&mut self) {
        if self.read_position >= self.input.len() {
            self.ch = 0; // NUL character, signifies EOF
        } else {
            self.ch = self.input.as_bytes()[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let tok = match self.ch {
            b'=' => Token::Assign,
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
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'/' => {
                if self.peek_char() == b'/' {
                    self.read_char();
                    let comment = self.read_single_line_comment();
                    return Token::SingleLineComment(comment);
                } else if self.peek_char() == b'*' {
                    self.read_char();
                    let comment = self.read_multi_line_comment();
                    return Token::MultiLineComment(comment);
                } else {
                    Token::Slash
                }
            }
            b'*' => {
                if self.peek_char() == b'*' {
                    self.read_char();
                    Token::Power
                } else {
                    Token::Asterisk
                }
            }
            b'%' => Token::Percent,
            b'>' => Token::Gt,
            b'<' => Token::Lt,
            b'!' => Token::Bang,
            b'&' => Token::Ampersand,
            b'@' => Token::At,
            b'#' => {
                if self.peek_char() == b' ' {
                    self.read_char();
                    let comment = self.read_generator_comment();
                    return Token::GeneratorComment(comment);
                } else {
                    Token::Hash
                }
            }
            b'"' => self.read_string(),
            b'\'' => self.read_string(),
            0 => Token::Eof,
            _ => {
                if is_letter(self.ch) {
                    let literal = self.read_identifier();
                    return match literal.as_str() {
                        "style" => Token::Style,
                        "script" => Token::Script,
                        "template" => Token::Template,
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
                            if self.peek_char() == b' ' {
                                let next_word = self.peek_word();
                                if next_word == "top" {
                                    self.read_char(); // consume space
                                    self.read_identifier(); // consume "top"
                                    return Token::AtTop;
                                } else if next_word == "bottom" {
                                    self.read_char(); // consume space
                                    self.read_identifier(); // consume "bottom"
                                    return Token::AtBottom;
                                }
                            }
                            Token::Identifier(literal)
                        }
                        "from" => Token::From,
                        "as" => Token::As,
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
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while is_letter(self.ch) || is_digit(self.ch) {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        while is_digit(self.ch) || self.ch == b'.' {
            self.read_char();
        }
        let number_str = &self.input[position..self.position];
        Token::Number(number_str.to_string())
    }

    fn read_string(&mut self) -> Token {
        let quote_type = self.ch;
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == quote_type || self.ch == 0 {
                break;
            }
        }
        let value = self.input[position..self.position].to_string();
        Token::String(value)
    }

    fn read_single_line_comment(&mut self) -> String {
        let position = self.position + 1;
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_multi_line_comment(&mut self) -> String {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'*' && self.peek_char() == b'/' {
                break;
            }
            if self.ch == 0 {
                break;
            }
        }
        let value = self.input[position..self.position].to_string();
        self.read_char(); // consume *
        self.read_char(); // consume /
        value
    }

    fn read_generator_comment(&mut self) -> String {
        let position = self.position + 1;
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }


    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }

    fn peek_word(&self) -> &str {
        let start = self.read_position;
        if start >= self.input.len() {
            return "";
        }
        let mut end = start;
        while end < self.input.len() && is_letter(self.input.as_bytes()[end]) {
            end += 1;
        }
        &self.input[start..end]
    }
}

fn is_letter(ch: u8) -> bool {
    ch.is_ascii_alphabetic() || ch == b'_'
}

fn is_digit(ch: u8) -> bool {
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
            Token::Identifier("text".to_string()),
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
            Token::Number("100".to_string()),
            Token::Identifier("px".to_string()),
            Token::Semicolon,
            Token::Identifier("height".to_string()),
            Token::Colon,
            Token::Number("200.5".to_string()),
            Token::Identifier("px".to_string()),
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
}