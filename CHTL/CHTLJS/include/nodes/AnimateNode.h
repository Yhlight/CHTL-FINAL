#pragma once

#include "AstNode.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTLJS
{
    struct AnimateNode : public AstNode
    {
        std::string target;
        int duration;
        std::string easing;
        std::unordered_map<std::string, std::string> begin_state;
        std::vector<std::unordered_map<std::string, std::string>> when_states;
        std::unordered_map<std::string, std::string> end_state;
        int loop;
        std::string direction;
        int delay;
        std::string callback;
        NodeType GetType() const override { return NodeType::Animate; }
        std::string ToString() const override { return "AnimateNode"; }
        std::unique_ptr<AstNode> clone() const override;
    };

}
