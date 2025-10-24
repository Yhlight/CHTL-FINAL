#include "Generator.h"
#include <stdexcept>

std::string Generator::generate(Document& doc) {
    std::string style_string;
    if (!doc.globalStyles.empty()) {
        style_string += "<style>";
        for (const auto& rule : doc.globalStyles) {
            style_string += rule.selector + " {";
            for (const auto& prop : rule.properties) {
                style_string += prop.first + ": " + evaluateExpression(*prop.second) + ";";
            }
            style_string += "}";
        }
        style_string += "</style>";
    }

    if (doc.root == nullptr) return "";

    std::string body_html = generateNode(*doc.root);

    if (style_string.empty()) {
        return body_html;
    }

    // Check for head tag
    size_t head_pos = body_html.find("<head>");
    if (head_pos != std::string::npos) {
        size_t head_end_pos = body_html.find("</head>");
        body_html.insert(head_end_pos, style_string);
        return body_html;
    } else {
        // Find html tag
        size_t html_pos = body_html.find("<html>");
        if(html_pos != std::string::npos) {
             size_t html_end_pos = body_html.find(">", html_pos);
             body_html.insert(html_end_pos + 1, "<head>" + style_string + "</head>");
             return body_html;
        }
    }


    return "<head>" + style_string + "</head>" + body_html;
}

std::string Generator::generateNode(const Node& node) {
    std::string html;
    if (const ElementNode* element = dynamic_cast<const ElementNode*>(&node)) {
        html += "<" + element->tagName;
        for (const auto& attr : element->attributes) {
            html += " " + attr.first + "=\"" + attr.second + "\"";
        }
        if (!element->styles.empty()) {
            html += " style=\"";
            for (const auto& style : element->styles) {
                html += style.first + ": " + evaluateExpression(*style.second) + ";";
            }
            html += "\"";
        }
        html += ">";
        for (const auto& child : element->children) {
            html += generateNode(*child);
        }
        html += "</" + element->tagName + ">";
    } else if (const TextNode* text = dynamic_cast<const TextNode*>(&node)) {
        html += text->text;
    }
    return html;
}

std::string Generator::evaluateExpression(const Expression& expr) {
    if (const StringLiteral* s = dynamic_cast<const StringLiteral*>(&expr)) {
        return s->value;
    }
    if (const NumberLiteral* num = dynamic_cast<const NumberLiteral*>(&expr)) {
        return std::to_string(num->value) + num->unit;
    }
    if (const BinaryExpr* bin = dynamic_cast<const BinaryExpr*>(&expr)) {
        const NumberLiteral* left_num = dynamic_cast<const NumberLiteral*>(bin->left.get());
        const NumberLiteral* right_num = dynamic_cast<const NumberLiteral*>(bin->right.get());

        if (!left_num || !right_num) {
            throw std::runtime_error("Invalid expression: binary operations can only be performed on numbers.");
        }

        if (left_num->unit != right_num->unit && !left_num->unit.empty() && !right_num->unit.empty()) {
            throw std::runtime_error("Incompatible units in expression.");
        }

        double result = 0;
        switch (bin->op) {
            case Operator::Add: result = left_num->value + right_num->value; break;
            case Operator::Subtract: result = left_num->value - right_num->value; break;
            case Operator::Multiply: result = left_num->value * right_num->value; break;
            case Operator::Divide: result = left_num->value / right_num->value; break;
        }

        return std::to_string(result) + (left_num->unit.empty() ? right_num->unit : left_num->unit);
    }
    return "";
}
