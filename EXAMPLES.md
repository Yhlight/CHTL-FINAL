# CHTL 示例集合

这个文档展示了各种 CHTL 使用示例，从简单到复杂。

**版本**: v0.5.0-script-support  
**更新日期**: 2025-10-14

---

## 📚 目录

1. [Hello World](#1-hello-world)
2. [基础元素](#2-基础元素)
3. [样式入门](#3-样式入门)
4. [属性运算](#4-属性运算)
5. [脚本块基础](#5-脚本块基础) ⭐ 新增
6. [交互式应用](#6-交互式应用) ⭐ 新增
7. [卡片组件](#7-卡片组件)
8. [导航栏](#8-导航栏)
9. [网格布局](#9-网格布局)
10. [完整网站](#10-完整网站)

---

## 1. Hello World

**最简单的 CHTL 程序**

```chtl
html {
    body {
        text: "Hello, CHTL!";
    }
}
```

**编译命令**:
```bash
./build/bin/chtl examples/simple.chtl
```

**输出**:
```html
<html>
  <body>Hello, CHTL!</body>
</html>
```

---

## 2. 基础元素

**元素、属性和嵌套**

```chtl
html {
    body {
        div {
            id: "container";
            class: "main-content";
            
            h1 {
                text: "欢迎";
            }
            
            p {
                text: "这是一个段落";
            }
        }
    }
}
```

**输出**:
```html
<html>
  <body>
    <div class="main-content" id="container">
      <h1>欢迎</h1>
      <p>这是一个段落</p>
    </div>
  </body>
</html>
```

---

## 3. 样式入门

**内联样式块**

```chtl
div {
    id: "header";
    
    style {
        width: 100%;
        padding: 20px;
        background-color: #3498db;
        color: white;
    }
    
    text: "标题栏";
}
```

**输出**:
```html
<div id="header" style="background-color: # 3498db; color: white; padding: 20px; width: 100%">标题栏</div>
```

---

## 4. 属性运算

**展示各种运算**

```chtl
div {
    style {
        // 基础运算
        width: 100px + 50px;              // 150px
        height: 200px - 50px;             // 150px
        
        // 乘除
        padding: 10px * 2;                // 20px
        margin: 100px / 4;                // 25px
        
        // 优先级
        font-size: 16px + 8px * 2;        // 32px (16 + 16)
        
        // 括号
        line-height: (16px + 8px) * 2;    // 48px
    }
}
```

**输出**:
```html
<div style="font-size: 32px; height: 150px; line-height: 48px; margin: 25px; padding: 20px; width: 150px" />
```

---

## 5. 脚本块基础

**展示 script {} 块的用法** ⭐ v0.5.0 新功能

```chtl
button {
    id: "myButton";
    
    style {
        padding: 10px 20px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
    }
    
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
<button id="myButton" text="点击我" style="background-color: #007bff; ...">
  <script>
    this . addEventListener(click , function(){alert(按钮被点击了!);});
  </script>
</button>
```

**查看完整示例**:
```bash
./build/bin/chtl examples/script_demo.chtl --doctype
```

---

## 6. 交互式应用

**简单的 TODO 应用** ⭐ v0.5.0 新功能

```chtl
div {
    h1 {
        text: "我的待办事项";
    }
    
    input {
        id: "taskInput";
        type: "text";
        placeholder: "输入新任务";
    }
    
    button {
        id: "addBtn";
        
        script {
            this.addEventListener("click", function() {
                var input = document.getElementById("taskInput");
                var text = input.value;
                
                if (text) {
                    var list = document.getElementById("taskList");
                    var item = document.createElement("li");
                    item.textContent = text;
                    list.appendChild(item);
                    input.value = "";
                }
            });
        }
        
        text: "添加";
    }
    
    ul {
        id: "taskList";
    }
}
```

**查看完整示例**:
```bash
./build/bin/chtl examples/simple_todo.chtl --doctype --output todo.html
open todo.html  # 在浏览器中打开
```

---

## 7. 卡片组件

**可复用的卡片样式**

```chtl
div {
    class: "card";
    
    style {
        width: 300px;
        padding: 30px;
        border-radius: 8px;
        box-shadow: 0px 2px 10px rgba;
        background-color: white;
    }
    
    h3 {
        style {
            margin: 0;
            font-size: 16px * 1.5;    // 24px
            color: #333;
        }
        
        text: "卡片标题";
    }
    
    p {
        style {
            margin-top: 10px;
            font-size: 16px;
            color: #666;
            line-height: 16px * 1.6;   // 25.6px
        }
        
        text: "这是卡片的内容描述";
    }
    
    button {
        style {
            margin-top: 20px;
            padding: (8px * 1.5) + (8px * 2);  // 12px 16px
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
        }
        
        text: "了解更多";
    }
}
```

---

## 8. 导航栏

**响应式导航栏**

```chtl
header {
    id: "navbar";
    
    style {
        width: 100%;
        padding: 20px;
        background-color: #2c3e50;
        box-shadow: 0px 2px 4px rgba;
    }
    
    nav {
        class: "nav-container";
        
        style {
            max-width: 1200px;
            margin: 0 auto;
            display: flex;
        }
        
        div {
            class: "logo";
            
            style {
                font-size: 16px * 1.5;  // 24px
                font-weight: bold;
                color: white;
            }
            
            text: "我的网站";
        }
        
        div {
            class: "nav-links";
            
            style {
                margin-left: auto;
                display: flex;
                gap: 30px;
            }
            
            a {
                href: "#home";
                style {
                    color: white;
                    text-decoration: none;
                    padding: 8px + 4px;  // 12px
                }
                text: "首页";
            }
            
            a {
                href: "#about";
                style {
                    color: white;
                    text-decoration: none;
                    padding: 12px;
                }
                text: "关于";
            }
            
            a {
                href: "#contact";
                style {
                    color: white;
                    text-decoration: none;
                    padding: 12px;
                }
                text: "联系";
            }
        }
    }
}
```

---

## 9. 网格布局

**12列网格系统**

```chtl
div {
    class: "grid-container";
    
    style {
        max-width: 1200px;
        margin: 0 auto;
    }
    
    // 全宽 (12/12)
    div {
        class: "col-12";
        style {
            width: 100px * 12;     // 1200px
            padding: 15px;
        }
        text: "全宽列";
    }
    
    // 半宽 (6/12)
    div {
        class: "col-6";
        style {
            width: 100px * 6;      // 600px
            padding: 15px;
        }
        text: "半宽列 1";
    }
    
    div {
        class: "col-6";
        style {
            width: 600px;
            padding: 15px;
        }
        text: "半宽列 2";
    }
    
    // 三分之一 (4/12)
    div {
        class: "col-4";
        style {
            width: 100px * 4;      // 400px
            padding: 15px;
        }
        text: "1/3 宽度";
    }
    
    div {
        class: "col-4";
        style {
            width: 400px;
            padding: 15px;
        }
        text: "1/3 宽度";
    }
    
    div {
        class: "col-4";
        style {
            width: 400px;
            padding: 15px;
        }
        text: "1/3 宽度";
    }
}
```

---

## 10. 完整网站

**查看完整的网站示例**

```bash
# 编译完整网站示例
./build/bin/chtl examples/complete_website.chtl --doctype --output website.html

# 在浏览器中打开
open website.html  # macOS
xdg-open website.html  # Linux
start website.html  # Windows
```

这个示例包含：
- ✅ 完整的导航栏
- ✅ Hero section
- ✅ 特性展示区
- ✅ 定价表
- ✅ 页脚
- ✅ 响应式布局计算
- ✅ 复杂的属性运算

---

## 💡 运算技巧

### 间距系统（8px 基准）

```chtl
div {
    style {
        padding: 8px * 1;    // 8px
        margin: 8px * 2;     // 16px
        gap: 8px * 3;        // 24px
    }
}
```

### 字体大小层级

```chtl
div {
    // 基准：16px
    
    h1 {
        style { font-size: 16px * 2.5; }  // 40px
    }
    
    h2 {
        style { font-size: 16px * 2; }    // 32px
    }
    
    h3 {
        style { font-size: 16px * 1.5; }  // 24px
    }
    
    p {
        style { font-size: 16px * 1; }    // 16px
    }
}
```

### 黄金比例

```chtl
div {
    style {
        width: 1000px / 1.618;                   // 618px
        height: 1000px / (1.618 * 1.618);        // 382px
    }
}
```

### 响应式宽度

```chtl
div {
    style {
        // 容器宽度
        width: 1200px;
        
        // 减去左右 padding
        width: 1200px - (20px * 2);              // 1160px
        
        // 三列平分
        width: (1200px - 60px) / 3;              // 380px
    }
}
```

---

## 🎯 最佳实践

1. **保持一致的间距**
   ```chtl
   // 好：使用 8 的倍数
   padding: 8px * 2;   // 16px
   margin: 8px * 3;    // 24px
   
   // 避免：随意的数字
   padding: 13px;
   margin: 27px;
   ```

2. **使用相对字体大小**
   ```chtl
   // 好：基于基准字体
   font-size: 16px * 1.5;  // 24px
   
   // 可以：直接指定
   font-size: 24px;
   ```

3. **计算容器宽度**
   ```chtl
   // 好：显式计算
   width: (1200px - (20px * 2)) / 3;
   
   // 说明：
   // 1200px - 总宽度
   // 20px * 2 - 左右边距
   // / 3 - 三列平分
   ```

4. **利用括号提高可读性**
   ```chtl
   // 好：清晰的计算意图
   height: ((200px + 100px) * 2) / 3;
   
   // 避免：复杂嵌套
   height: 200px + 100px * 2 / 3;
   ```

---

## 📁 示例文件

项目提供的示例文件：

| 文件 | 说明 | 特性 |
|------|------|------|
| `simple.chtl` | 最简单示例 | 基础元素 |
| `style_demo.chtl` | 样式演示 | 内联样式、多种单位 |
| `advanced_expression.chtl` | 高级表达式 | 复杂运算、布局计算 |
| `complete_website.chtl` | 完整网站 | 所有功能综合应用 |
| `template_example.chtl` | 模板示例 | 特殊语法块 |
| `website_demo.chtl` | 网站演示 | 实际应用场景 |
| `showcase.chtl` | 功能展示 | 各种特性演示 |

---

## 🚀 快速尝试

```bash
# 1. 查看所有示例
ls examples/

# 2. 编译任意示例
./build/bin/chtl examples/simple.chtl

# 3. 保存输出
./build/bin/chtl examples/advanced_expression.chtl --output result.html

# 4. 添加 DOCTYPE
./build/bin/chtl examples/complete_website.chtl --doctype --output website.html
```

---

## 9. Script 块基础

**新功能**: v0.5.0

### 按钮点击

```chtl
button {
    id: "alertBtn";
    
    style {
        padding: 10px 20px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 4px;
    }
    
    script {
        this.addEventListener("click", function() {
            alert("Hello from CHTL!");
        });
    }
    
    text: "点击我";
}
```

**输出**:
```html
<button id="alertBtn" style="..." text="点击我">
  <script>
    this.addEventListener("click", function() {
        alert("Hello from CHTL!");
    });
  </script>
</button>
```

---

## 10. 简单 TODO 应用

**文件**: `examples/simple_todo.chtl`

完整的交互式 TODO 应用，展示：
- 输入框
- 添加/删除功能
- DOM 操作
- 事件处理

**编译**:
```bash
./build/bin/chtl examples/simple_todo.chtl --output todo.html
```

**功能**:
- ✅ 添加待办事项
- ✅ 清空列表
- ✅ 实时更新 UI

---

## 11. Script 演示应用

**文件**: `examples/script_demo.chtl`

完整的演示页面，包含：
- 简单按钮点击
- 计数器（状态管理）
- 文本输入交互
- 页面初始化脚本

**编译**:
```bash
./build/bin/chtl examples/script_demo.chtl --doctype --output demo.html
```

---

## 📁 示例文件

项目提供的示例文件（更新至 v0.5.0）：

| 文件 | 说明 | 功能 | 版本 |
|------|------|------|------|
| `simple.chtl` | 最简单示例 | 基础元素 | v0.1+ |
| `style_demo.chtl` | 样式演示 | 内联样式、多种单位 | v0.3+ |
| `advanced_expression.chtl` | 高级表达式 | 复杂运算、布局计算 | v0.4+ |
| `complete_website.chtl` | 完整网站 | 所有功能综合应用 | v0.4+ |
| `simple_todo.chtl` | TODO 应用 | **script 块、交互** | **v0.5+** ⭐ |
| `script_demo.chtl` | Script 演示 | **完整 script 功能** | **v0.5+** ⭐ |
| `template_example.chtl` | 模板示例 | 特殊语法块 | v0.2+ |
| `website_demo.chtl` | 网站演示 | 实际应用场景 | v0.3+ |
| `showcase.chtl` | 功能展示 | 各种特性演示 | v0.2+ |
| `complete_example.chtl` | 综合示例 | 完整页面 | v0.3+ |
| `interactive_app.chtl` | 交互应用 | 高级交互示例 | v0.5+ |

---

**探索更多示例，发现 CHTL 的强大功能！** 🎉

**v0.5.0 新增**: script {} 块 | 优化的样式处理 | 更多交互示例
