#pragma once

#include "AstNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTLJS
{
    struct Keyframe
    {
        double at;
        std::unordered_map<std::string, std::string> properties;
    };

    struct AnimateNode : public AstNode
    {
        std::unique_ptr<AstNode> target;
        int duration;
        std::string easing;
        std::unordered_map<std::string, std::string> begin_state;
        std::vector<Keyframe> when_states;
        std::unordered_map<std::string, std::string> end_state;
        int loop;
        std::string direction;
        int delay;
        std::string callback;
        NodeType GetType() const override { return NodeType::Animate; }
        std::string ToString() const override { return "AnimateNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<AnimateNode> deserialize(std::istream& is);
    };

}
