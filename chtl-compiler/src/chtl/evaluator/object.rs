#[derive(Debug, PartialEq, Clone)]
pub enum Object {
    Number(f64, String), // Value and unit
    String(String),
    Boolean(bool),
    Error(String),
}

impl std::fmt::Display for Object {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            Object::Number(val, unit) => write!(f, "{}{}", val, unit),
            Object::String(s) => write!(f, "{}", s),
            Object::Boolean(b) => write!(f, "{}", b),
            Object::Error(s) => write!(f, "Error: {}", s),
        }
    }
}

impl Object {
    pub fn is_truthy(&self) -> bool {
        match self {
            Object::Boolean(b) => *b,
            Object::Number(val, _) => *val != 0.0,
            Object::String(s) => !s.is_empty(),
            _ => false,
        }
    }
}