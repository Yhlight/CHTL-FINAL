# Contributing to CHTL

First off, thank you for considering contributing to CHTL! It's people like you that make CHTL such a great tool.

## How Can I Contribute?

### Reporting Bugs

If you find a bug, please open an issue on our GitHub repository. Be sure to include a clear title and description, as much relevant information as possible, and a code sample or an executable test case demonstrating the expected behavior that is not occurring.

### Suggesting Enhancements

If you have an idea for a new feature or an improvement to an existing one, please open an issue on our GitHub repository. This allows for a discussion of the idea before any code is written.

### Pull Requests

If you've fixed a bug or implemented a new feature, you can submit a pull request.

1.  Fork the repository and create your branch from `main`.
2.  Make your changes.
3.  Add tests for your changes.
4.  Ensure the test suite passes.
5.  Make sure your code lints.
6.  Issue that pull request!

## Setting Up Your Development Environment

To get started, you'll need the following:

*   A C++ compiler that supports C++20 (e.g., GCC 10+, Clang 12+)
*   CMake 3.10+
*   Python 3.6+
*   Node.js and npm (for the VSCode extension)

## Building the Project

The project uses a Python script to automate the build process. To build the project, run the following command from the root directory:

```bash
python3 build.py
```

This will:

1.  Create a `build` directory inside the `CHTL` directory.
2.  Run CMake to configure the project.
3.  Run `make` to compile the C++ code.
4.  Run all of the tests.
5.  Package the VSCode extension into a `.vsix` file.

## Coding Conventions

### C++

*   We follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with a few exceptions.
*   Use Doxygen-style comments for all public classes and methods.

### Python

*   We follow the [PEP 8](https://www.python.org/dev/peps/pep-0008/) style guide.

We look forward to your contributions!
