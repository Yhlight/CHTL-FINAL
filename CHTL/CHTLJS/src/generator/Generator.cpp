#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#include "CHTLJS/include/nodes/ProgramNode.h"
#include <map>

namespace CHTLJS {

Generator::Generator(std::shared_ptr<CHTL::SaltBridge> bridge)
    : bridge_(bridge) {}

std::string Generator::Generate(const ProgramNode &program) {
  output_.clear();
  for (const auto &child : program.children) {
    visit(*child);
  }
  return output_;
}

void Generator::visit(const AstNode &node) {
  switch (node.GetType()) {
  case NodeType::EnhancedSelector:
    visit(static_cast<const EnhancedSelectorNode &>(node));
    break;
  case NodeType::RawJS:
    visit(static_cast<const RawJSNode &>(node));
    break;
  // Add cases for other node types here...
    case NodeType::Animate:
        visit(static_cast<const AnimateNode&>(node));
        break;
    case NodeType::Delegate:
        visit(static_cast<const DelegateNode&>(node));
        break;
    case NodeType::Listen:
        visit(static_cast<const ListenNode&>(node));
        break;
    case NodeType::ScriptLoader:
        visit(static_cast<const ScriptLoaderNode&>(node));
        break;
    case NodeType::Vir:
        visit(static_cast<const VirNode&>(node));
        break;
  default:
    // Handle unknown node type
    break;
  }
}

void Generator::visit(const EnhancedSelectorNode &node) {
  if (node.selector == "&") {
    auto parent_selector_opt = bridge_->requestData("parent_selector");
    if (parent_selector_opt.has_value()) {
      try {
        std::string parent_selector =
            std::any_cast<std::string>(*parent_selector_opt);
        output_ += "document.querySelector('" + parent_selector + "')";
      } catch (const std::bad_any_cast &e) {
        // Handle error: data is not a string
      }
    } else {
      // Handle error: no parent selector available
    }
  } else {
    output_ += "document.querySelector('" + node.selector + "')";
  }
}

void Generator::visit(const RawJSNode &node) { output_ += node.content; }

void Generator::visit(const AnimateNode &node) {
    output_ += "{\n";
    output_ += "    const target = document.querySelector('" + node.target + "');\n";
    output_ += "    const keyframes = [\n";

    // Begin state
    output_ += "        {";
    std::map<std::string, std::string> sorted_begin_state(node.begin_state.begin(), node.begin_state.end());
    bool first = true;
    for (const auto& pair : sorted_begin_state) {
        if (!first) {
            output_ += ", ";
        }
        output_ += "\"" + pair.first + "\": \"" + pair.second + "\"";
        first = false;
    }
    output_ += "},\n";

    // When states
    for (const auto& state : node.when_states) {
        output_ += "        {";
        std::map<std::string, std::string> sorted_state(state.begin(), state.end());
        first = true;
        for (const auto& pair : sorted_state) {
            if (!first) {
                output_ += ", ";
            }
            output_ += "\"" + pair.first + "\": \"" + pair.second + "\"";
            first = false;
        }
        output_ += "},\n";
    }

    // End state
    output_ += "        {";
    std::map<std::string, std::string> sorted_end_state(node.end_state.begin(), node.end_state.end());
    first = true;
    for (const auto& pair : sorted_end_state) {
        if (!first) {
            output_ += ", ";
        }
        output_ += "\"" + pair.first + "\": \"" + pair.second + "\"";
        first = false;
    }
    output_ += "}\n";

    output_ += "    ];\n";
    output_ += "    const options = {\n";
    output_ += "        duration: " + std::to_string(node.duration) + ",\n";
    output_ += "        easing: \"" + node.easing + "\"\n";
    output_ += "    };\n";
    output_ += "    target.animate(keyframes, options);\n";
    output_ += "}\n";
}

void Generator::visit(const DelegateNode &node) {
    output_ += "{\n";
    output_ += "    const targets = [";
    for (size_t i = 0; i < node.targets.size(); ++i) {
        output_ += "\"" + node.targets[i] + "\"";
        if (i < node.targets.size() - 1) {
            output_ += ", ";
        }
    }
    output_ += "];\n";
    output_ += "    const events = {";
    std::map<std::string, std::string> sorted_events(node.events.begin(), node.events.end());
    bool first = true;
    for (const auto& pair : sorted_events) {
        if (!first) {
            output_ += ", ";
        }
        output_ += "\"" + pair.first + "\": " + pair.second;
        first = false;
    }
    output_ += "};\n";
    output_ += "    targets.forEach(target => {\n";
    output_ += "        document.querySelectorAll(target).forEach(element => {\n";
    output_ += "            for (const event in events) {\n";
    output_ += "                element.addEventListener(event, events[event]);\n";
    output_ += "            }\n";
    output_ += "        });\n";
    output_ += "    });\n";
    output_ += "}\n";
}

void Generator::visit(const ListenNode &node) {
    std::map<std::string, std::string> sorted_events(node.events.begin(), node.events.end());
    output_ += "{\n";
    output_ += "    const events = {";
    bool first = true;
    for (const auto& pair : sorted_events) {
        if (!first) {
            output_ += ", ";
        }
        output_ += "\"" + pair.first + "\": " + pair.second;
        first = false;
    }
    output_ += "};\n";
    output_ += "    for (const event in events) {\n";
    output_ += "        this.addEventListener(event, events[event]);\n";
    output_ += "    }\n";
    output_ += "}\n";
}

void Generator::visit(const ScriptLoaderNode &node) {
    output_ += "{\n";
    output_ += "    const scripts = [";
    for (size_t i = 0; i < node.paths.size(); ++i) {
        output_ += "\"" + node.paths[i] + "\"";
        if (i < node.paths.size() - 1) {
            output_ += ", ";
        }
    }
    output_ += "];\n";
    output_ += "    scripts.forEach(path => {\n";
    output_ += "        const script = document.createElement('script');\n";
    output_ += "        script.src = path;\n";
    output_ += "        document.head.appendChild(script);\n";
    output_ += "    });\n";
    output_ += "}\n";
}

void Generator::visit(const VirNode &node) {
    output_ += "{\n";
    output_ += "    const " + node.name + " = {\n";
    std::map<std::string, std::string> sorted_properties(node.properties.begin(), node.properties.end());
    bool first = true;
    for (const auto& pair : sorted_properties) {
        if (!first) {
            output_ += ",\n";
        }
        output_ += "        \"" + pair.first + "\": " + pair.second;
        first = false;
    }
    output_ += "\n    };\n";
    output_ += "}\n";
}

} // namespace CHTLJS
