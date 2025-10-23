#include "Serialize.h"
#include "AstNode.h"
#include "nodes/ExpressionNode.h"
#include "CHTLJS/include/nodes/AnimateNode.h"

namespace CHTL
{
    template <>
    void Serializer::serialize<std::string>(const std::string& value, std::ostream& os)
    {
        size_t size = value.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        os.write(value.c_str(), size);
    }

    template <>
    void Serializer::deserialize<std::string>(std::string& value, std::istream& is)
    {
        size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<char> buffer(size);
        is.read(buffer.data(), size);
        value.assign(buffer.data(), size);
    }

    template <>
    void Serializer::serialize<std::unique_ptr<AstNode>>(const std::unique_ptr<AstNode>& node, std::ostream& os)
    {
        node->serialize(os);
    }

    template <>
    void Serializer::deserialize<std::unique_ptr<AstNode>>(std::unique_ptr<AstNode>& node, std::istream& is)
    {
        node = AstNode::deserialize(is);
    }

    template <>
    void Serializer::serialize<std::unique_ptr<Expression>>(const std::unique_ptr<Expression>& expr, std::ostream& os)
    {
        expr->serialize(os);
    }

    template <>
    void Serializer::deserialize<std::unique_ptr<Expression>>(std::unique_ptr<Expression>& expr, std::istream& is)
    {
        expr = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
    }

    template <>
    void Serializer::serialize<CHTLJS::Keyframe>(const CHTLJS::Keyframe& keyframe, std::ostream& os)
    {
        os.write(reinterpret_cast<const char*>(&keyframe.at), sizeof(keyframe.at));
        Serializer::serialize(keyframe.properties, os);
    }

    template <>
    void Serializer::deserialize<CHTLJS::Keyframe>(CHTLJS::Keyframe& keyframe, std::istream& is)
    {
        is.read(reinterpret_cast<char*>(&keyframe.at), sizeof(keyframe.at));
        Serializer::deserialize(keyframe.properties, is);
    }
}
