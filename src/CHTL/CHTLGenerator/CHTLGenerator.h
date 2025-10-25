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

    std::string generateNode(ASTNode* node);
    std::string generateElementNode(ElementNode* node);
    std::string generateTextNode(TextNode* node);
    void generateStyleNode(StyleNode* node, const std::string& parentSelector);
    void generateStyleTemplateNode(StyleTemplateNode* node);
};

#endif //CHTL_GENERATOR_H
