#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLParser/ASTNode.h"
#include <string>

class CHTLGenerator {
public:
    CHTLGenerator(std::unique_ptr<ProgramNode> ast);
    std::string generate();

private:
    std::unique_ptr<ProgramNode> ast;
    std::string css;
    std::map<std::string, StyleTemplateNode*> styleTemplates;
    std::map<std::string, ElementTemplateNode*> elementTemplates;
    std::map<std::string, VarTemplateNode*> varTemplates;
    std::map<std::string, CustomStyleTemplateNode*> customStyleTemplates;
    std::map<std::string, CustomElementNode*> customElementTemplates;

    std::string generateNode(ASTNode* node);
    std::string generateElementNode(ElementNode* node);
    std::string generateTextNode(TextNode* node);
    void generateStyleNode(StyleNode* node, const std::string& parentSelector);
    void generateStyleTemplateNode(StyleTemplateNode* node);
    std::string valueNodeToString(const ValueNode* valueNode);
    std::string generateScriptNode(ScriptNode* node);
};

#endif //CHTL_GENERATOR_H
