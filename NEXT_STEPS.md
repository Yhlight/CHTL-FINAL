# CHTL 项目下一步开发计划

更新时间: 2025-10-24

## 当前状态摘要

✅ **核心功能完成**: 所有测试通过(978个断言)
✅ **构建系统**: Python脚本自动化构建和测试
✅ **文档**: 详细的语言规范和项目文档
⚠️ **进行中**: 事件绑定操作符实现

## 正在进行的工作

### 1. 事件绑定操作符 (`&->`)

**目标**: 实现CHTL JS事件绑定操作符语法

**进度**:
- ✅ 创建 EventBindNode AST节点结构
- ✅ 更新 NodeType 枚举添加 EventBind类型
- ✅ 添加 BIND_OP 和 ARROW token类型
- ⏳ 需要增强Lexer识别 `&->`
- ⏳ 需要在Parser中添加事件绑定解析逻辑
- ⏳ 需要在Generator中生成对应的JS代码
- ⏳ 需要添加单元测试

**相关文件**:
- `/workspace/CHTL/CHTLJS/include/nodes/EventBindNode.h` - 已创建
- `/workspace/CHTL/CHTLJS/src/nodes/EventBindNode.cpp` - 已创建
- `/workspace/CHTL/CHTLJS/include/nodes/AstNode.h` - 已更新
- `/workspace/CHTL/CHTLJS/include/lexer/Token.h` - 已更新
- `/workspace/CHTL/CHTLJS/src/lexer/Token.cpp` - 已更新

**待实现**:
```cpp
// 在 Lexer.cpp 中需要识别 &->
// 示例代码框架:
Token Lexer::NextToken()
{
    // ... 现有代码
    
    // 需要添加对 &-> 的识别
    if (m_char == '&' && peekChar() == '-')
    {
        readChar(); // consume '&'
        if (peekChar() == '>')
        {
            readChar(); // consume '-'
            readChar(); // consume '>'
            return {TokenType::BIND_OP, "&->", tok.line, tok.column};
        }
    }
    
    // ... 继续现有代码
}

// 在 Parser.cpp 中需要在parseStatement中添加
std::unique_ptr<AstNode> Parser::parseEventBind(std::unique_ptr<EnhancedSelectorNode> target)
{
    auto node = std::make_unique<EventBindNode>();
    node->target = std::move(target);
    
    // 解析事件绑定语法
    // 支持:
    // 1. 单事件: &-> click: () => {}
    // 2. 多事件: &-> click, mouseenter: () => {}
    // 3. 链式: &-> click: fn1, &-> mouseenter: fn2
    // 4. 块: &-> { click: fn1, mouseenter: fn2 }
    
    return node;
}

// 在 Generator.cpp 中需要添加
void Generator::visit(const EventBindNode& node)
{
    // 生成 addEventListener 调用
    // 例如: target.addEventListener('click', handler);
}
```

**语法示例**:
```chtl
// 单事件绑定
{{box}} &-> click: () => {
    console.log('Clicked!');
};

// 多事件绑定
{{box}} &-> click, mouseenter, mouseleave: () => {
    console.log('Event fired!');
};

// 链式绑定
{{box}} &-> click: handleClick,
        &-> mouseenter: handleMouseEnter,
        &-> mouseleave: handleMouseLeave;

// 块形式
{{box}} &-> {
    click: handleClick,
    mouseenter: handleMouseEnter,
    mouseleave: handleMouseLeave
};
```

## 未来功能实现计划

### 2. 路由系统 (`Router`)

**优先级**: 高
**预估工作量**: 2-3天

需要实现:
- RouterNode AST节点
- 路由配置解析
- 代码生成（history/hash模式）
- 页面切换逻辑

**语法**:
```chtl
Router {
    url: "/home",
    page: {{#home-page}},
}

Router {
    root: "/",
    mode: "history"
}
```

### 3. 动态属性条件表达式

**优先级**: 中
**预估工作量**: 1-2天

扩展现有的静态属性条件表达式为动态版本:
```chtl
div {
    style {
        width: {{box}}->width > 2 ? 100px : 50px;  // 动态变化
    }
}
```

### 4. 响应式值 (`$变量名$`)

**优先级**: 中
**预估工作量**: 2-3天

实现CHTL与CHTL JS的交互:
```chtl
div {
    class = $boxClass$;
    style {
        width: $boxWidth$ + 20px;
    }
    script {
        let boxClass = "box";
        let boxWidth = 100;
    }
}
```

### 5. VSCode扩展增强

**优先级**: 中高
**预估工作量**: 1周

需要实现:
- 代码格式化器
- 基础代码补全
- 错误诊断
- 代码跳转

### 6. LSP语言服务器

**优先级**: 中
**预估工作量**: 2-3周

完整的IDE支持:
- 智能补全
- 实时错误检查
- 符号搜索
- 重构支持

### 7. 官方模块开发

**优先级**: 低（功能稳定后）
**预估工作量**: 持续进行

#### Chtholly珂朵莉模块
- 手风琴组件
- 四叶窗相册
- 备忘录组件
- 暖色笔记
- 樱花雨效果
- 鼠标特效

#### Yuigahama由比滨结衣模块
- 音乐播放器
- 图片组件
- UI组件库

## 技术债务

### 1. Lexer架构优化
当前Lexer过于简单，所有非 `{{`/`}}` 内容都作为RAW_JS处理。这对于简单功能可行，但对于复杂语法（如 `&->`）需要增强。

**建议**:
- 实现两阶段词法分析
- 第一阶段：识别CHTL JS结构（{{}}、关键字）
- 第二阶段：在需要时对RAW_JS进行精细分析

### 2. 错误恢复
当前解析器在遇到错误时停止。需要改进错误恢复机制以提供更好的开发体验。

### 3. 性能优化
- 大文件编译性能
- 内存使用优化
- 增量编译支持

### 4. 测试覆盖率
需要增加边界情况和错误处理的测试:
- 语法错误测试
- 边界条件测试
- 性能测试
- 集成测试

## 开发流程建议

### 实现新功能的标准流程:

1. **设计阶段**
   - 在CHTL.md中明确语法规范
   - 设计AST节点结构
   - 确定生成的JS代码格式

2. **实现阶段**
   - 创建AST节点类（.h和.cpp）
   - 更新Token类型（如需要）
   - 更新Lexer识别新token（如需要）
   - 在Parser中添加解析逻辑
   - 在Generator中添加代码生成逻辑
   - 更新AstNode的序列化/反序列化

3. **测试阶段**
   - 编写单元测试
   - 测试正常情况
   - 测试边界情况
   - 测试错误处理

4. **文档阶段**
   - 更新ROADMAP.md
   - 添加使用示例
   - 更新API文档

5. **集成阶段**
   - 运行完整测试套件
   - 更新构建脚本
   - 提交代码

## 依赖和工具

### 需要的工具和库:
- CMake 3.10+
- C++17编译器
- Catch2测试框架（已包含）
- Node.js (VSCode扩展开发)
- Python 3.6+ (构建脚本)

### 推荐的开发工具:
- VSCode (主开发IDE)
- CLion / Visual Studio (C++开发)
- Git (版本控制)
- Valgrind / AddressSanitizer (内存检查)

## 贡献指南

详见 `CONTRIBUTING.md`

关键点:
- 遵循现有代码风格
- 所有新功能必须有测试
- 保持文档更新
- PR前确保所有测试通过

## 联系和资源

- 项目文档: `/workspace/CHTL.md`
- 路线图: `/workspace/ROADMAP.md`
- 项目状态: `/workspace/PROJECT_STATUS.md`
- 贡献指南: `/workspace/CONTRIBUTING.md`

---

本文档会随项目进展持续更新。
