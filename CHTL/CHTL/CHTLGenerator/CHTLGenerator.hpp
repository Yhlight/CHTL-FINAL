#ifndef CHTL_GENERATOR_HPP
#define CHTL_GENERATOR_HPP

#include "CHTLNode/AstNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TextNode.hpp"
#include "CHTLNode/RuleNode.hpp"
#include "CHTLNode/ScriptNode.hpp"
#include "CHTLEvaluator/Evaluator.hpp"
#include <string>
#include "Document.hpp"

class CHTLGenerator {
public:
    Document generate(const AstNode& root);

private:
    void visit(const AstNode& node);
    void visitElementNode(const ElementNode& node);
    void visitTextNode(const TextNode& node);
    void visitRuleNode(const RuleNode& node);
    void visitScriptNode(const ScriptNode& node);

    std::string html_;
    std::string css_;
    std::string js_;
    Evaluator evaluator_;
};

#endif // CHTL_GENERATOR_HPP
