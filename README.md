# CHTL: A New Hypertext Language

CHTL is a C++-based hypertext language designed to provide a more intuitive and developer-friendly way to write HTML. It is open-source and licensed under the MIT License.

## Features

- **Simplified Syntax:** Write HTML and CSS with a cleaner, more readable syntax.
- **Advanced Styling:** Use features like nested styles, property arithmetic, and conditional expressions.
- **Component-Based:** Create reusable templates and custom components to build complex UIs.
- **Modular:** Import and export modules to organize your codebase effectively.
- **Extensible:** CHTL JS allows you to write dynamic and interactive scripts with a powerful and expressive syntax.

## Quick Start

Here’s a simple "Hello, World" example in CHTL:

```chtl
// example.chtl
html
{
    body
    {
        text
        {
            "Hello, World!"
        }
    }
}
```

To compile this file, run the `build.py` script:

```bash
python build.py --file example.chtl
```

This will generate an `output.html` file in the same directory.

## Specification

For a complete and detailed language reference, please see the [CHTL Specification](SPECIFICATION.md).

## Contributing

We welcome contributions from the community! If you're interested in helping improve CHTL, please read our [Contributing Guidelines](CONTRIBUTING.md) to get started.

## License

CHTL is released under the [MIT License](LICENSE).
