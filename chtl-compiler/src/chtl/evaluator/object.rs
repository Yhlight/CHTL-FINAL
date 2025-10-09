#[derive(Debug, PartialEq, Clone)]
pub enum Object {
    Number(f64, String), // Value and unit
    String(String),
    Error(String),
}

impl std::fmt::Display for Object {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            Object::Number(val, unit) => write!(f, "{}{}", val, unit),
            Object::String(s) => write!(f, "{}", s),
            Object::Error(s) => write!(f, "Error: {}", s),
        }
    }
}