#include "GlobalMap.h"

namespace CHTL {

GlobalMap::GlobalMap() {
    initializeKeywords();
    initializeHTMLElements();
}

void GlobalMap::initializeKeywords() {
    // 基础块关键字
    keywords_["text"] = TokenType::KEYWORD_TEXT;
    keywords_["style"] = TokenType::KEYWORD_STYLE;
    keywords_["script"] = TokenType::KEYWORD_SCRIPT;
    
    // 模板和自定义关键字
    keywords_["[Template]"] = TokenType::KEYWORD_TEMPLATE;
    keywords_["[Custom]"] = TokenType::KEYWORD_CUSTOM;
    keywords_["[Origin]"] = TokenType::KEYWORD_ORIGIN;
    keywords_["[Import]"] = TokenType::KEYWORD_IMPORT;
    keywords_["[Namespace]"] = TokenType::KEYWORD_NAMESPACE;
    keywords_["[Configuration]"] = TokenType::KEYWORD_CONFIGURATION;
    
    // 类型标识关键字
    keywords_["@Style"] = TokenType::KEYWORD_AT_STYLE;
    keywords_["@Element"] = TokenType::KEYWORD_AT_ELEMENT;
    keywords_["@Var"] = TokenType::KEYWORD_AT_VAR;
    keywords_["@Html"] = TokenType::KEYWORD_AT_HTML;
    keywords_["@JavaScript"] = TokenType::KEYWORD_AT_JAVASCRIPT;
    keywords_["@Chtl"] = TokenType::KEYWORD_AT_CHTL;
    keywords_["@Config"] = TokenType::KEYWORD_AT_CONFIG;
    keywords_["@CJmod"] = TokenType::KEYWORD_AT_CJMOD;
    
    // 操作符关键字
    keywords_["inherit"] = TokenType::KEYWORD_INHERIT;
    keywords_["delete"] = TokenType::KEYWORD_DELETE;
    keywords_["insert"] = TokenType::KEYWORD_INSERT;
    keywords_["after"] = TokenType::KEYWORD_AFTER;
    keywords_["before"] = TokenType::KEYWORD_BEFORE;
    keywords_["replace"] = TokenType::KEYWORD_REPLACE;
    keywords_["from"] = TokenType::KEYWORD_FROM;
    keywords_["as"] = TokenType::KEYWORD_AS;
    keywords_["except"] = TokenType::KEYWORD_EXCEPT;
    keywords_["use"] = TokenType::KEYWORD_USE;
    keywords_["html5"] = TokenType::KEYWORD_HTML5;
    
    // "at top" 和 "at bottom" 需要特殊处理（两个词的关键字）
    // 这里先添加基础版本
    keywords_["at"] = TokenType::IDENTIFIER; // "at" 单独出现时是标识符
}

void GlobalMap::initializeHTMLElements() {
    // HTML5 常用元素标签
    // 文档结构
    htmlElements_["html"] = true;
    htmlElements_["head"] = true;
    htmlElements_["body"] = true;
    htmlElements_["title"] = true;
    htmlElements_["meta"] = true;
    htmlElements_["link"] = true;
    
    // 分区和语义
    htmlElements_["header"] = true;
    htmlElements_["footer"] = true;
    htmlElements_["nav"] = true;
    htmlElements_["main"] = true;
    htmlElements_["section"] = true;
    htmlElements_["article"] = true;
    htmlElements_["aside"] = true;
    htmlElements_["div"] = true;
    htmlElements_["span"] = true;
    
    // 文本内容
    htmlElements_["h1"] = true;
    htmlElements_["h2"] = true;
    htmlElements_["h3"] = true;
    htmlElements_["h4"] = true;
    htmlElements_["h5"] = true;
    htmlElements_["h6"] = true;
    htmlElements_["p"] = true;
    htmlElements_["br"] = true;
    htmlElements_["hr"] = true;
    
    // 列表
    htmlElements_["ul"] = true;
    htmlElements_["ol"] = true;
    htmlElements_["li"] = true;
    htmlElements_["dl"] = true;
    htmlElements_["dt"] = true;
    htmlElements_["dd"] = true;
    
    // 链接和媒体
    htmlElements_["a"] = true;
    htmlElements_["img"] = true;
    htmlElements_["video"] = true;
    htmlElements_["audio"] = true;
    htmlElements_["source"] = true;
    htmlElements_["iframe"] = true;
    htmlElements_["canvas"] = true;
    htmlElements_["svg"] = true;
    
    // 表单
    htmlElements_["form"] = true;
    htmlElements_["input"] = true;
    htmlElements_["button"] = true;
    htmlElements_["select"] = true;
    htmlElements_["option"] = true;
    htmlElements_["textarea"] = true;
    htmlElements_["label"] = true;
    htmlElements_["fieldset"] = true;
    htmlElements_["legend"] = true;
    
    // 表格
    htmlElements_["table"] = true;
    htmlElements_["thead"] = true;
    htmlElements_["tbody"] = true;
    htmlElements_["tfoot"] = true;
    htmlElements_["tr"] = true;
    htmlElements_["th"] = true;
    htmlElements_["td"] = true;
    htmlElements_["caption"] = true;
    
    // 格式化
    htmlElements_["strong"] = true;
    htmlElements_["em"] = true;
    htmlElements_["b"] = true;
    htmlElements_["i"] = true;
    htmlElements_["u"] = true;
    htmlElements_["s"] = true;
    htmlElements_["code"] = true;
    htmlElements_["pre"] = true;
    htmlElements_["blockquote"] = true;
    
    // 其他常用元素
    htmlElements_["details"] = true;
    htmlElements_["summary"] = true;
    htmlElements_["dialog"] = true;
    htmlElements_["template"] = true;
    htmlElements_["slot"] = true;
}

TokenType GlobalMap::lookup(const std::string& lexeme) const {
    auto it = keywords_.find(lexeme);
    if (it != keywords_.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

bool GlobalMap::isKeyword(const std::string& lexeme) const {
    return keywords_.find(lexeme) != keywords_.end();
}

bool GlobalMap::isHTMLElement(const std::string& lexeme) const {
    return htmlElements_.find(lexeme) != htmlElements_.end();
}

} // namespace CHTL
