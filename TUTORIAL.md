# CHTL 快速教程

## 📚 目录

1. [Hello World](#hello-world)
2. [元素和属性](#元素和属性)
3. [内联样式](#内联样式)
4. [属性运算](#属性运算)
5. [实战示例](#实战示例)

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

**Happy Coding with CHTL!** 🎉
