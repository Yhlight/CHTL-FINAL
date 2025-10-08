#pragma once

#include "../CHTLNode/TemplateDeclarationNode.h"
#include <string>
#include <map>
#include <memory>

class CHTLContext {
public:
    std::map<std::string, std::unique_ptr<TemplateDeclarationNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<TemplateDeclarationNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<TemplateDeclarationNode>> varTemplates;
};