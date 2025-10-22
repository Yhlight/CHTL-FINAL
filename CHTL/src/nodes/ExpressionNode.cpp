#include "ExpressionNode.h"
#include <sstream>

namespace CHTL
{
    std::string ConditionalExpression::ToString() const
    {
        std::stringstream ss;
        ss << "(" << condition->ToString() << " ? " << consequence->ToString();
        if (alternative)
        {
            ss << " : " << alternative->ToString();
        }
        ss << ")";
        return ss.str();
    }

    std::string VariableAccessNode::ToString() const
    {
        return template_name + "(" + variable_name + ")";
    }

    std::string ExpressionListNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExpressionList([";
        for (size_t i = 0; i < expressions.size(); ++i)
        {
            ss << expressions[i]->ToString();
            if (i < expressions.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> NumberLiteral::clone() const {
        auto node = std::make_unique<NumberLiteral>();
        node->value = this->value;
        node->unit = this->unit;
        return node;
    }

    std::unique_ptr<AstNode> InfixExpression::clone() const {
        auto node = std::make_unique<InfixExpression>();
        node->left = std::unique_ptr<Expression>(static_cast<Expression*>(this->left->clone().release()));
        node->op = this->op;
        node->right = std::unique_ptr<Expression>(static_cast<Expression*>(this->right->clone().release()));
        return node;
    }

    std::unique_ptr<AstNode> Identifier::clone() const {
        auto node = std::make_unique<Identifier>();
        node->value = this->value;
        return node;
    }

    std::unique_ptr<AstNode> StringLiteral::clone() const {
        auto node = std::make_unique<StringLiteral>();
        node->value = this->value;
        return node;
    }

    std::unique_ptr<AstNode> ConditionalExpression::clone() const {
        auto node = std::make_unique<ConditionalExpression>();
        node->condition = std::unique_ptr<Expression>(static_cast<Expression*>(this->condition->clone().release()));
        node->consequence = std::unique_ptr<Expression>(static_cast<Expression*>(this->consequence->clone().release()));
        if (this->alternative) {
            node->alternative = std::unique_ptr<Expression>(static_cast<Expression*>(this->alternative->clone().release()));
        }
        return node;
    }

    std::unique_ptr<AstNode> VariableAccessNode::clone() const {
        auto node = std::make_unique<VariableAccessNode>();
        node->template_name = this->template_name;
        node->variable_name = this->variable_name;
        return node;
    }

    std::unique_ptr<AstNode> ExpressionListNode::clone() const {
        auto node = std::make_unique<ExpressionListNode>();
        for (const auto& expr : this->expressions) {
            node->expressions.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(expr->clone().release())));
        }
        return node;
    }

    std::unique_ptr<AstNode> AttributeAccessExpression::clone() const {
        auto node = std::make_unique<AttributeAccessExpression>();
        node->selector = this->selector;
        node->attribute_name = this->attribute_name;
        return node;
    }

    // NumberLiteral
    void NumberLiteral::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
        size_t len = unit.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(unit.c_str(), len);
    }

    std::unique_ptr<NumberLiteral> NumberLiteral::deserialize(std::istream& is) {
        auto node = std::make_unique<NumberLiteral>();
        is.read(reinterpret_cast<char*>(&node->value), sizeof(node->value));
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->unit.resize(len);
        is.read(&node->unit[0], len);
        return node;
    }

    // InfixExpression
    void InfixExpression::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        left->serialize(os);
        size_t len = op.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(op.c_str(), len);
        right->serialize(os);
    }

    std::unique_ptr<InfixExpression> InfixExpression::deserialize(std::istream& is) {
        auto node = std::make_unique<InfixExpression>();
        node->left = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->op.resize(len);
        is.read(&node->op[0], len);
        node->right = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        return node;
    }

    // Identifier
    void Identifier::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        size_t len = value.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(value.c_str(), len);
    }

    std::unique_ptr<Identifier> Identifier::deserialize(std::istream& is) {
        auto node = std::make_unique<Identifier>();
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->value.resize(len);
        is.read(&node->value[0], len);
        return node;
    }

    // StringLiteral
    void StringLiteral::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        size_t len = value.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(value.c_str(), len);
    }

    std::unique_ptr<StringLiteral> StringLiteral::deserialize(std::istream& is) {
        auto node = std::make_unique<StringLiteral>();
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->value.resize(len);
        is.read(&node->value[0], len);
        return node;
    }

    // ConditionalExpression
    void ConditionalExpression::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        condition->serialize(os);
        consequence->serialize(os);
        bool has_alternative = alternative != nullptr;
        os.write(reinterpret_cast<const char*>(&has_alternative), sizeof(has_alternative));
        if (has_alternative) {
            alternative->serialize(os);
        }
    }

    std::unique_ptr<ConditionalExpression> ConditionalExpression::deserialize(std::istream& is) {
        auto node = std::make_unique<ConditionalExpression>();
        node->condition = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        node->consequence = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        bool has_alternative;
        is.read(reinterpret_cast<char*>(&has_alternative), sizeof(has_alternative));
        if (has_alternative) {
            node->alternative = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        }
        return node;
    }

    // VariableAccessNode
    void VariableAccessNode::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        size_t t_len = template_name.length();
        os.write(reinterpret_cast<const char*>(&t_len), sizeof(t_len));
        os.write(template_name.c_str(), t_len);

        size_t v_len = variable_name.length();
        os.write(reinterpret_cast<const char*>(&v_len), sizeof(v_len));
        os.write(variable_name.c_str(), v_len);
    }

    std::unique_ptr<VariableAccessNode> VariableAccessNode::deserialize(std::istream& is) {
        auto node = std::make_unique<VariableAccessNode>();
        size_t t_len;
        is.read(reinterpret_cast<char*>(&t_len), sizeof(t_len));
        node->template_name.resize(t_len);
        is.read(&node->template_name[0], t_len);

        size_t v_len;
        is.read(reinterpret_cast<char*>(&v_len), sizeof(v_len));
        node->variable_name.resize(v_len);
        is.read(&node->variable_name[0], v_len);
        return node;
    }

    // ExpressionListNode
    void ExpressionListNode::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        size_t count = expressions.size();
        os.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (const auto& expr : expressions) {
            expr->serialize(os);
        }
    }

    std::unique_ptr<ExpressionListNode> ExpressionListNode::deserialize(std::istream& is) {
        auto node = std::make_unique<ExpressionListNode>();
        size_t count;
        is.read(reinterpret_cast<char*>(&count), sizeof(count));
        for (size_t i = 0; i < count; ++i) {
            node->expressions.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release())));
        }
        return node;
    }

    // AttributeAccessExpression
    void AttributeAccessExpression::serialize(std::ostream& os) const {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        size_t s_len = selector.length();
        os.write(reinterpret_cast<const char*>(&s_len), sizeof(s_len));
        os.write(selector.c_str(), s_len);

        size_t a_len = attribute_name.length();
        os.write(reinterpret_cast<const char*>(&a_len), sizeof(a_len));
        os.write(attribute_name.c_str(), a_len);
    }

    std::unique_ptr<AttributeAccessExpression> AttributeAccessExpression::deserialize(std::istream& is) {
        auto node = std::make_unique<AttributeAccessExpression>();
        size_t s_len;
        is.read(reinterpret_cast<char*>(&s_len), sizeof(s_len));
        node->selector.resize(s_len);
        is.read(&node->selector[0], s_len);

        size_t a_len;
        is.read(reinterpret_cast<char*>(&a_len), sizeof(a_len));
        node->attribute_name.resize(a_len);
        is.read(&node->attribute_name[0], a_len);
        return node;
    }
}