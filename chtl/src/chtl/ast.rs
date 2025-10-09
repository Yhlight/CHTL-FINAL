#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    Program(Program),
    Statement(Statement),
    Expression(Expression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct Program {
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Element(Element),
    Text(Text),
    Style(Style),
    Script(Script),
    GeneratorComment(String),
    Origin(Origin),
    StyleTemplateDefinition(StyleTemplateDefinition),
    StyleTemplateUsage(String),
    ElementTemplateDefinition(ElementTemplateDefinition),
    ElementTemplateUsage(String),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(String),
    StringLiteral(String),
}

#[derive(Debug, PartialEq, Clone)]
pub struct Element {
    pub tag: String,
    pub attributes: Vec<(String, String)>,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Text {
    pub value: String,
}

#[derive(Debug, PartialEq, Clone)]
pub enum StyleItem {
    Property((String, String)),
    TemplateUsage(String),
}

#[derive(Debug, PartialEq, Clone)]
pub struct Style {
    pub items: Vec<StyleItem>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Script {
    pub content: String,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Origin {
    pub origin_type: String,
    pub content: String,
}

#[derive(Debug, PartialEq, Clone)]
pub struct StyleTemplateDefinition {
    pub name: String,
    pub properties: Vec<(String, String)>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ElementTemplateDefinition {
    pub name: String,
    pub body: Vec<Statement>,
}