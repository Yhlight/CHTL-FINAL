#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#include "CHTLJS/include/nodes/EventBindNode.h"
#include "CHTLJS/include/nodes/RouterNode.h"
#include "CHTLJS/include/nodes/ReactiveValueNode.h"

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
  case NodeType::ScriptLoader:
    visit(static_cast<const ScriptLoaderNode &>(node));
    break;
  case NodeType::Listen:
    visit(static_cast<const ListenNode &>(node));
    break;
  case NodeType::Delegate:
    visit(static_cast<const DelegateNode &>(node));
    break;
  case NodeType::Animate:
    visit(static_cast<const AnimateNode &>(node));
    break;
  case NodeType::Vir:
    visit(static_cast<const VirNode &>(node));
    break;
  case NodeType::EventBind:
    visit(static_cast<const EventBindNode &>(node));
    break;
  case NodeType::Router:
    visit(static_cast<const RouterNode &>(node));
    break;
  case NodeType::ReactiveValue:
    visit(static_cast<const ReactiveValueNode &>(node));
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

void Generator::visit(const ScriptLoaderNode &node) {
  output_ += "(function() {\n";
  for (const auto &path : node.paths) {
    output_ += "  let script = document.createElement('script');\n";
    output_ += "  script.src = '" + path + "';\n";
    output_ += "  script.async = true;\n";
    output_ += "  document.head.appendChild(script);\n";
  }
  output_ += "})();\n";
}

void Generator::visit(const ListenNode &node) {
  std::string current_output = output_;
  output_ = "";
  visit(*node.target);
  std::string target_selector_js = output_;
  output_ = current_output;

  for (const auto &pair : node.events) {
    const std::string &event_name = pair.first;
    const std::string &handler = pair.second;
    output_ +=
        target_selector_js + ".addEventListener('" + event_name + "', " + handler + ");\n";
  }
}

void Generator::visit(const DelegateNode &node) {
  std::string current_output = output_;
  output_ = "";
  visit(*node.parent_target);
  std::string parent_selector_js = output_;
  output_ = current_output;

  output_ += "{\n";
  output_ += "  const parent = " + parent_selector_js + ";\n";
  for (const auto &pair : node.events) {
    const std::string &event_name = pair.first;
    const std::string &handler = pair.second;

    output_ += "  parent.addEventListener('" + event_name +
               "', (event) => {\n";
    output_ += "    let target = event.target;\n";
    output_ += "    while (target && target !== parent && target !== document) {\n";
    for (const auto &child_selector : node.child_targets) {
      output_ += "      if (target.matches('" + child_selector + "')) {\n";
      output_ += "        (" + handler + ").call(target, event);\n";
      output_ += "        return;\n";
      output_ += "      }\n";
    }
    output_ += "      target = target.parentNode;\n";
    output_ += "    }\n";
    output_ += "  });\n";
  }
  output_ += "}\n";
}

void Generator::visit(const AnimateNode &node) {
  std::string current_output = output_;
  output_ = "";
  visit(*node.target);
  std::string target_selector_js = output_;
  output_ = current_output;

  output_ += "{\n";
  output_ += "const keyframes = [\n";
  if (!node.begin_state.empty()) {
    output_ += "  { offset: 0";
    for (const auto &prop : node.begin_state) {
      output_ += ", " + prop.first + ": '" + prop.second + "'";
    }
    output_ += " },\n";
  }
  for (const auto &keyframe : node.when_states) {
    output_ += "  { offset: " + std::to_string(keyframe.at);
    for (const auto &prop : keyframe.properties) {
      output_ += ", " + prop.first + ": '" + prop.second + "'";
    }
    output_ += " },\n";
  }
  if (!node.end_state.empty()) {
    output_ += "  { offset: 1";
    for (const auto &prop : node.end_state) {
      output_ += ", " + prop.first + ": '" + prop.second + "'";
    }
    output_ += " },\n";
  }
  output_ += "];\n";

  output_ += "const options = {\n";
  if (node.duration > 0) {
    output_ += "  duration: " + std::to_string(node.duration) + ",\n";
  }
  if (!node.easing.empty()) {
    output_ += "  easing: '" + node.easing + "',\n";
  }
  if (node.delay > 0) {
    output_ += "  delay: " + std::to_string(node.delay) + ",\n";
  }
  if (node.loop != 0) {
    output_ += "  iterations: " +
               (node.loop == -1 ? "Infinity" : std::to_string(node.loop)) +
               ",\n";
  }
  if (!node.direction.empty()) {
    output_ += "  direction: '" + node.direction + "',\n";
  }
  output_ += "};\n";

  output_ += "const anim = " + target_selector_js + ".animate(keyframes, options);\n";

  if (!node.callback.empty()) {
    output_ += "anim.onfinish = " + node.callback + ";\n";
  }
  output_ += "}\n";
}

void Generator::visit(const VirNode &node) {
  // Vir is a compile-time construct and does not generate runtime code.
}

void Generator::visit(const EventBindNode &node) {
  // 获取目标选择器
  std::string current_output = output_;
  output_ = "";
  visit(*node.target);
  std::string target_selector_js = output_;
  output_ = current_output;

  if (node.isBlock) {
    // 块形式: {{box}} &-> { click: fn1, mouseenter: fn2 }
    for (const auto &[events, handler] : node.blockEvents) {
      for (const auto &event : events) {
        output_ += target_selector_js + ".addEventListener('" + event + "', " + handler + ");\n";
      }
    }
  } else {
    // 单事件或多事件: {{box}} &-> click, mouseenter: handler
    for (const auto &event : node.eventNames) {
      output_ += target_selector_js + ".addEventListener('" + event + "', " + node.handler + ");\n";
    }
  }
}

void Generator::visit(const RouterNode &node) {
  output_ += "(function() {\n";
  output_ += "  const routes = {};\n";
  
  // 添加路由映射
  for (const auto &[url, page] : node.urlPageMappings) {
    output_ += "  routes['" + url + "'] = '" + page + "';\n";
  }
  
  // 路由函数
  output_ += "  function navigate(url) {\n";
  output_ += "    const page = routes[url];\n";
  output_ += "    if (page) {\n";
  if (!node.rootPath.empty() || node.rootContainer) {
    if (node.rootContainer) {
      std::string current_output = output_;
      output_ = "";
      visit(*node.rootContainer);
      std::string container_js = output_;
      output_ = current_output;
      output_ += "      const container = " + container_js + ";\n";
    } else {
      output_ += "      const container = document.querySelector('" + node.rootPath + "');\n";
    }
    output_ += "      if (container) {\n";
    output_ += "        document.querySelectorAll('.page').forEach(p => p.style.display = 'none');\n";
    output_ += "        const targetPage = document.querySelector(page);\n";
    output_ += "        if (targetPage) targetPage.style.display = 'block';\n";
    output_ += "      }\n";
  }
  output_ += "    }\n";
  output_ += "  }\n";
  
  // 设置路由模式
  if (node.mode == "history") {
    output_ += "  window.addEventListener('popstate', (e) => {\n";
    output_ += "    navigate(window.location.pathname);\n";
    output_ += "  });\n";
    output_ += "  document.addEventListener('click', (e) => {\n";
    output_ += "    if (e.target.tagName === 'A') {\n";
    output_ += "      e.preventDefault();\n";
    output_ += "      const url = e.target.getAttribute('href');\n";
    output_ += "      history.pushState(null, '', url);\n";
    output_ += "      navigate(url);\n";
    output_ += "    }\n";
    output_ += "  });\n";
  } else if (node.mode == "hash") {
    output_ += "  window.addEventListener('hashchange', () => {\n";
    output_ += "    navigate(window.location.hash.slice(1));\n";
    output_ += "  });\n";
  }
  
  output_ += "  navigate(window.location.";
  output_ += (node.mode == "hash" ? std::string("hash.slice(1)") : std::string("pathname"));
  output_ += ");\n";
  output_ += "})();\n";
}

void Generator::visit(const ReactiveValueNode &node) {
  // 响应式值在编译时被替换为变量引用
  output_ += node.variableName;
}

} // namespace CHTLJS
