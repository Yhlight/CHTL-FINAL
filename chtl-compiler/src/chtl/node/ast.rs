#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    Program(Program),
    Statement(Statement),
    Expression(Expression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct Program {
    pub statements: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Element(ElementStatement),
    Text(TextStatement),
    Attribute(AttributeStatement),
    Style(StyleStatement),
    Comment(CommentStatement),
}

#[derive(Debug, PartialEq, Clone)]
pub struct CommentStatement {
    pub value: String,
}

#[derive(Debug, PartialEq, Clone)]
pub struct StyleStatement {
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(IdentifierExpression),
    StringLiteral(StringLiteralExpression),
    NumberLiteral(NumberLiteralExpression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct ElementStatement {
    pub name: IdentifierExpression,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TextStatement {
    pub value: StringLiteralExpression,
}

#[derive(Debug, PartialEq, Clone)]
pub struct AttributeStatement {
    pub name: IdentifierExpression,
    pub value: Expression,
}

#[derive(Debug, PartialEq, Clone)]
pub struct IdentifierExpression {
    pub value: String,
}

#[derive(Debug, PartialEq, Clone)]
pub struct StringLiteralExpression {
    pub value: String,
}

#[derive(Debug, PartialEq, Clone)]
pub struct NumberLiteralExpression {
    pub value: String,
}