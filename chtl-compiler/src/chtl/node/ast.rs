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
    pub is_custom: bool,
}

#[derive(Debug, PartialEq, Clone)]
pub struct UseTemplateStatement {
    pub name: IdentifierExpression,
    pub template_type: IdentifierExpression,
    pub body: Option<Vec<Statement>>,
    pub from: Option<IdentifierExpression>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct DeleteStatement {
    pub targets: Vec<Expression>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct IfStatement {
    pub condition: Expression,
    pub consequence: Vec<Statement>,
    pub alternative: Option<Box<Statement>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ElseStatement {
    pub consequence: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct NamespaceStatement {
    pub name: IdentifierExpression,
}

#[derive(Debug, PartialEq, Clone)]
pub struct InfoStatement {
    pub attributes: Vec<AttributeStatement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ExportStatement {
    pub items: Vec<ExportItem>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ExportItem {
    pub category: ImportItemCategory,
    pub item_type: IdentifierExpression,
    pub names: Vec<IdentifierExpression>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct NameBlock {
    pub settings: Vec<AttributeStatement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ConfigurationStatement {
    pub name: Option<IdentifierExpression>,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct OriginStatement {
    pub origin_type: IdentifierExpression,
    pub name: Option<IdentifierExpression>,
    pub content: String,
}

#[derive(Debug, PartialEq, Clone)]
pub enum UseTarget {
    Html5,
    Config(IdentifierExpression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct UseStatement {
    pub target: UseTarget,
}

#[derive(Debug, PartialEq, Clone)]
pub enum InsertPosition {
    After,
    Before,
    Replace,
    AtTop,
    AtBottom,
}

#[derive(Debug, PartialEq, Clone)]
pub struct InsertStatement {
    pub position: InsertPosition,
    pub target: Option<Expression>,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct InheritStatement {
    pub name: IdentifierExpression,
    pub template_type: IdentifierExpression,
    pub from: Option<IdentifierExpression>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Inherit(InheritStatement),
    Insert(InsertStatement),
    Use(UseStatement),
    Origin(OriginStatement),
    Element(ElementStatement),
    Text(TextStatement),
    Attribute(AttributeStatement),
    Style(StyleStatement),
    StyleRule(StyleRuleStatement),
    Comment(CommentStatement),
    TemplateDefinition(TemplateDefinitionStatement),
    UseTemplate(UseTemplateStatement),
    Import(ImportStatement),
    Delete(DeleteStatement),
    If(IfStatement),
    Else(ElseStatement),
    Namespace(NamespaceStatement),
    Info(InfoStatement),
    Export(ExportStatement),
    Configuration(ConfigurationStatement),
    NameBlock(NameBlock),
    Script(ScriptStatement),
}

#[derive(Debug, PartialEq, Clone)]
pub struct CommentStatement {
    pub value: String,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ScriptStatement {
    pub content: String,
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
    PropertyAccess(PropertyAccessExpression),
    Index(IndexExpression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct IndexExpression {
    pub left: Box<Expression>,
    pub index: Box<Expression>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct PropertyAccessExpression {
    pub object: Box<Expression>,
    pub property: IdentifierExpression,
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
    pub value: Expression,
}

#[derive(Debug, PartialEq, Clone)]
pub struct AttributeStatement {
    pub name: IdentifierExpression,
    pub value: Option<Expression>,
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
    pub unit: Option<String>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ImportStatement {
    pub path: Expression,
    pub alias: Option<IdentifierExpression>,
    pub specifier: ImportSpecifier,
}

#[derive(Debug, PartialEq, Clone)]
pub enum ImportSpecifier {
    File(ImportFileType),
    Item(ImportItemSpecifier),
}

#[derive(Debug, PartialEq, Clone)]
pub enum ImportFileType {
    Chtl,
    Html,
    Style,
    JavaScript,
    CjMod,
    Config,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ImportItemSpecifier {
    pub category: Option<ImportItemCategory>,
    pub item_type: Option<IdentifierExpression>,
    pub name: Option<IdentifierExpression>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum ImportItemCategory {
    Custom,
    Template,
    Origin,
    Configuration,
}