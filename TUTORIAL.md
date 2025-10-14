# CHTL 快速教程

**版本**: v0.5.0-script-support  
**更新日期**: 2025-10-14

## 📚 目录

1. [Hello World](#hello-world)
2. [元素和属性](#元素和属性)
3. [内联样式](#内联样式)
4. [属性运算](#属性运算)
5. [脚本块](#脚本块) ⭐ 新增
6. [实战示例](#实战示例)

---

## Hello World

最简单的 CHTL 程序：

```chtl
html {
    body {
        text: "Hello, CHTL!";
    }
}
```

编译：
```bash
./build/bin/chtl hello.chtl
```

输出：
```html
<html>
  <body>Hello, CHTL!</body>
</html>
```

---

## 元素和属性

### 基础元素

```chtl
div {
    id: "container";
    class: "main";
    
    text: "这是一个 div 元素";
}
```

输出：
```html
<div class="main" id="container">这是一个 div 元素</div>
```

### 嵌套元素

```chtl
div {
    id: "parent";
    
    div {
        id: "child";
        text: "子元素";
    }
}
```

### CE 对等式

`:` 和 `=` 完全等价：

```chtl
div {
    id: "box";      // 使用冒号
    class = "red";  // 使用等号 - 完全相同
}
```

---

## 内联样式

### 基础样式

```chtl
div {
    style {
        width: 100px;
        height: 200px;
        background-color: red;
    }
}
```

输出：
```html
<div style="background-color: red; height: 200px; width: 100px" />
```

### 样式与属性结合

```chtl
div {
    id: "header";
    class: "container";
    
    style {
        width: 100%;
        padding: 20px;
        margin: 0;
    }
    
    text: "标题";
}
```

---

## 属性运算

### 基础运算

```chtl
div {
    style {
        // 加法
        width: 100px + 50px;        // 150px
        
        // 减法
        height: 200px - 50px;       // 150px
        
        // 乘法
        padding: 10px * 2;          // 20px
        
        // 除法
        margin: 100px / 4;          // 25px
    }
}
```

### 运算符优先级

```chtl
div {
    style {
        // 乘法优先于加法
        width: 100px + 50px * 2;    // 200px (100 + 100)
        
        // 除法优先于减法
        height: 100px - 50px / 2;   // 75px (100 - 25)
    }
}
```

### 括号改变优先级

```chtl
div {
    style {
        width: (100px + 50px) * 2;  // 300px
        height: (200px - 50px) / 3; // 50px
    }
}
```

### 多种单位

```chtl
div {
    style {
        // px 单位
        width: 100px + 50px;        // 150px
        
        // em 单位
        font-size: 1em * 1.5;       // 1.5em
        
        // rem 单位
        padding: 2rem + 1rem;       // 3rem
        
        // 百分比
        width: 100% - 20%;          // 80%
    }
}
```

### 复杂表达式

```chtl
div {
    style {
        // 黄金比例宽度
        width: 1000px / 1.618;
        
        // 响应式间距
        padding: (8px * 3) + (8px * 2);
        
        // 嵌套计算
        height: ((200px + 100px) * 2) / 3;
    }
}
```

---

## 脚本块

### 基础用法 ⭐ v0.5.0 新功能

```chtl
button {
    script {
        alert("Hello!");
    }
    
    text: "点击我";
}
```

### 事件监听器

```chtl
button {
    id: "myBtn";
    
    script {
        this.addEventListener("click", function() {
            alert("按钮被点击了!");
        });
    }
    
    text: "Click Me";
}
```

### DOM 操作

```chtl
div {
    div {
        id: "output";
        text: "初始文本";
    }
    
    button {
        script {
            this.addEventListener("click", function() {
                document.getElementById("output").textContent = "文本已更新!";
            });
        }
        
        text: "更新文本";
    }
}
```

### 计数器示例

```chtl
div {
    div {
        id: "counter";
        text: "计数: 0";
    }
    
    button {
        script {
            var count = 0;
            this.addEventListener("click", function() {
                count++;
                document.getElementById("counter").textContent = "计数: " + count;
            });
        }
        
        text: "增加";
    }
}
```

### 结合样式

```chtl
button {
    style {
        padding: 10px 20px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 4px;
    }
    
    script {
        this.addEventListener("click", function() {
            this.style.backgroundColor = "#0056b3";
        });
    }
    
    text: "悬停变色";
}
```

---

## 实战示例

### 卡片组件

```chtl
div {
    class: "card";
    
    style {
        width: 300px;
        padding: 20px;
        border-radius: 8px;
        box-shadow: 0px 2px 8px rgba;
        background-color: white;
    }
    
    h3 {
        style {
            margin-top: 0;
            font-size: 16px * 1.5;      // 24px
            color: #333;
        }
        
        text: "卡片标题";
    }
    
    p {
        style {
            font-size: 16px;
            line-height: 16px * 1.6;    // 25.6px
            color: #666;
        }
        
        text: "这是卡片内容";
    }
}
```

### 三列布局

```chtl
div {
    class: "container";
    
    style {
        width: 1200px;
        margin: 0 auto;
    }
    
    // 列 1
    div {
        class: "column";
        style {
            width: (1200px - 60px) / 3;  // 380px
            padding: 20px;
            margin: 10px;
        }
        text: "列 1";
    }
    
    // 列 2
    div {
        class: "column";
        style {
            width: 380px;
            padding: 20px;
            margin: 10px;
        }
        text: "列 2";
    }
    
    // 列 3
    div {
        class: "column";
        style {
            width: 380px;
            padding: 20px;
            margin: 10px;
        }
        text: "列 3";
    }
}
```

### 间距系统

使用 8px 倍数创建一致的间距：

```chtl
div {
    div {
        class: "space-1";
        style {
            padding: 8px * 1;   // 8px
            margin: 8px * 1;
        }
    }
    
    div {
        class: "space-2";
        style {
            padding: 8px * 2;   // 16px
            margin: 8px * 2;
        }
    }
    
    div {
        class: "space-3";
        style {
            padding: 8px * 3;   // 24px
            margin: 8px * 3;
        }
    }
}
```

### 字体大小层级

```chtl
div {
    // 基准字体 16px
    
    h1 {
        style {
            font-size: 16px * 2.5;  // 40px
        }
        text: "主标题";
    }
    
    h2 {
        style {
            font-size: 16px * 2;    // 32px
        }
        text: "副标题";
    }
    
    p {
        style {
            font-size: 16px * 1;    // 16px
        }
        text: "正文";
    }
    
    small {
        style {
            font-size: 16px * 0.875; // 14px
        }
        text: "小字";
    }
}
```

---

## 脚本块

### 基础脚本

CHTL v0.5.0 新增了 `script {}` 块支持：

```chtl
button {
    id: "myButton";
    
    script {
        this.addEventListener("click", function() {
            alert("按钮被点击了!");
        });
    }
    
    text: "点击我";
}
```

**输出**:
```html
<button id="myButton" text="点击我">
  <script>
    this.addEventListener("click", function() {
        alert("按钮被点击了!");
    });
  </script>
</button>
```

### 结合样式和脚本

```chtl
div {
    id: "counter";
    
    style {
        padding: 20px;
        background-color: #f0f0f0;
        text-align: center;
    }
    
    script {
        var count = 0;
        var btn = document.createElement("button");
        btn.textContent = "点击计数";
        btn.onclick = function() {
            count++;
            this.parentElement.querySelector("p").textContent = "计数: " + count;
        };
        this.appendChild(btn);
    }
    
    p {
        text: "计数: 0";
    }
}
```

### DOM 操作

```chtl
input {
    id: "nameInput";
    type: "text";
    placeholder: "输入名字";
}

button {
    script {
        this.addEventListener("click", function() {
            var name = document.getElementById("nameInput").value;
            alert("你好, " + name);
        });
    }
    
    text: "问候";
}
```

---

## 💡 最佳实践

### 1. 使用变量式的计算

虽然 CHTL 目前不支持变量，但可以通过重复使用相同的值来模拟：

```chtl
// 基准单位：8px
div {
    style {
        padding: 8px * 2;       // 16px
        margin: 8px * 3;        // 24px
        border-radius: 8px * 1; // 8px
    }
}
```

### 2. 保持一致的比例

```chtl
div {
    style {
        // 使用黄金比例
        width: 1000px / 1.618;
        height: 1000px / (1.618 * 1.618);
    }
}
```

### 3. 响应式计算

```chtl
div {
    style {
        // 容器宽度减去边距
        width: 1200px - (20px * 2);
        
        // 平分剩余空间
        width: (1200px - (20px * 4)) / 3;
    }
}
```

---

## 📖 进阶阅读

- **完整语法规范**: 查看 `CHTL.md`
- **项目文档**: 查看 `README.md`
- **示例文件**: 查看 `examples/` 目录
  - `simple.chtl` - 基础示例
  - `style_demo.chtl` - 样式演示
  - `advanced_expression.chtl` - 高级表达式
  - `complete_website.chtl` - 完整网站

---

## 🚀 快速开始

```bash
# 1. 构建项目
python3 build.py --all

# 2. 编译你的第一个 CHTL 文件
echo 'html { body { text: "Hello CHTL!"; } }' > hello.chtl
./build/bin/chtl hello.chtl

# 3. 输出到文件
./build/bin/chtl hello.chtl --output hello.html

# 4. 添加 DOCTYPE
./build/bin/chtl hello.chtl --doctype --output hello.html

# 5. 查看示例
./build/bin/chtl examples/complete_website.chtl --doctype
```

---

## ❓ 常见问题

### Q: 为什么我的样式没有生效？

A: 确保样式块在元素内部：

```chtl
div {
    style {              // ✅ 正确
        width: 100px;
    }
}
```

### Q: 如何使用百分比？

A: 直接使用 `%` 符号：

```chtl
div {
    style {
        width: 100% - 20%;   // 80%
    }
}
```

### Q: 不同单位可以运算吗？

A: 不可以！同单位才能加减：

```chtl
// ✅ 正确
width: 100px + 50px;    // 150px

// ❌ 错误
width: 100px + 50em;    // 报错！
```

### Q: 如何查看编译错误？

A: CHTL 会显示详细的错误信息，包括文件名、行号、列号。

---

## 🎯 下一步

1. 尝试编译示例文件
2. 创建自己的 CHTL 文件
3. 实验属性运算
4. 构建完整的网站

---

## 🎯 完整示例

### 交互式 TODO 应用

查看 `examples/simple_todo.chtl` 获取一个完整的交互式应用示例，包含：
- ✅ 输入框和按钮
- ✅ 动态添加列表项
- ✅ 事件处理
- ✅ DOM 操作

```bash
./build/bin/chtl examples/simple_todo.chtl --output todo.html
```

### Script 演示

查看 `examples/script_demo.chtl` 获取 script 块的各种用法：
- ✅ 按钮点击事件
- ✅ 计数器（状态管理）
- ✅ 文本交互
- ✅ 页面初始化

```bash
./build/bin/chtl examples/script_demo.chtl --doctype --output demo.html
```

---

**Happy Coding with CHTL v0.5.0!** 🎉

**新功能**: script {} 块 | 优化的样式处理 | 56个测试用例

---

## 🆕 v0.5.0 新特性

### script {} 块

v0.5.0 引入了完整的脚本块支持！

**特点**:
- ✅ 完整的 JavaScript 代码支持
- ✅ 支持事件监听器
- ✅ 支持 DOM 操作
- ✅ 可与 style {} 块结合

**示例**:
```bash
# 查看脚本演示
./build/bin/chtl examples/script_demo.chtl --doctype

# 查看 TODO 应用
./build/bin/chtl examples/simple_todo.chtl --doctype --output todo.html
open todo.html
```

### 颜色代码优化

现在颜色代码不再有空格问题：
- ✅ `#fff` → `#fff` (正确)
- ✅ `#3498db` → `#3498db` (正确)

### CSS 空格优化

现在多值属性的空格正确了：
- ✅ `margin: 0 auto` → `margin: 0 auto` (正确)
- ✅ `padding: 10px 20px` → `padding: 10px 20px` (正确)

