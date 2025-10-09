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
pub struct StyleRuleStatement {
    pub selector: String,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum TemplateType {
    Style,
    Element,
    Var,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateDefinitionStatement {
    pub name: IdentifierExpression,
    pub template_type: TemplateType,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct UseTemplateStatement {
    pub name: IdentifierExpression,
    // For now, we'll just store the raw type identifier (e.g., "Style")
    // This can be refined later if needed.
    pub template_type: IdentifierExpression,
}


#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Element(ElementStatement),
    Text(TextStatement),
    Attribute(AttributeStatement),
    Style(StyleStatement),
    StyleRule(StyleRuleStatement),
    Comment(CommentStatement),
    TemplateDefinition(TemplateDefinitionStatement),
    UseTemplate(UseTemplateStatement),
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
pub struct ConditionalExpression {
    pub condition: Box<Expression>,
    pub consequence: Box<Expression>,
    pub alternative: Option<Box<Expression>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct FunctionCallExpression {
    pub function: Box<Expression>,
    pub arguments: Vec<Expression>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(IdentifierExpression),
    StringLiteral(StringLiteralExpression),
    NumberLiteral(NumberLiteralExpression),
    UnquotedLiteral(UnquotedLiteralExpression),
    Infix(InfixExpression),
    Conditional(ConditionalExpression),
    FunctionCall(FunctionCallExpression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct InfixExpression {
    pub left: Box<Expression>,
    pub operator: String,
    pub right: Box<Expression>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct UnquotedLiteralExpression {
    pub value: String,
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