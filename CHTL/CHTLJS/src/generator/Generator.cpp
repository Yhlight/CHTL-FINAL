#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"

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

} // namespace CHTLJS
