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

To get started, you'll need the following tools installed on your system:

*   **A C++ compiler that supports C++20:** For example, GCC 10+, Clang 12+, or MSVC v142.
*   **CMake 3.10+:** For managing the C++ build process.
*   **Python 3.6+:** For running the main build script.
*   **Node.js and npm:** For building and packaging the VSCode extension.

### Initial Setup

After cloning the repository, you'll need to set up the dependencies for both the C++ and VSCode components.

#### C++ Dependencies (GTest)

The project uses Google Test for C++ unit testing. This is included as a Git submodule. To download it, run the following commands from the root of the repository:

```bash
git submodule init
git submodule update
```

If for some reason the submodule command does not work, you can manually clone the GTest repository into the `third-party/gtest` directory:

```bash
git clone https://github.com/google/googletest.git third-party/gtest
```

#### VSCode Extension Dependencies (vsce)

The VSCode extension requires the `@vscode/vsce` package for packaging. We recommend installing it locally within the `chtl-vscode` directory to avoid version conflicts.

```bash
npm install --prefix ./chtl-vscode @vscode/vsce
```

The main build script is already configured to use this local installation.

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

## Project Structure and Testing

### Project Structure

The repository is organized into a few key directories:

*   `CHTL/`: Contains the core C++ source code for the CHTL compiler.
    *   `CHTL/include/`: Header files (`.h`).
    *   `CHTL/src/`: Source files (`.cpp`).
    *   `CHTL/tests/`: C++ unit tests.
*   `chtl-vscode/`: Contains the source code for the VSCode extension.
    *   `chtl-vscode/syntaxes/`: TextMate grammar for syntax highlighting.
*   `third-party/`: Contains third-party dependencies like Google Test.

### Running Tests

The easiest way to run all tests is to use the main build script:

```bash
python3 build.py
```

This script will compile the project and then automatically discover and run all test executables located in the `CHTL/build/tests/` directory.

### Adding New Tests

When you add a new feature or fix a bug, you should add a corresponding test.

1.  Create a new `.cpp` file in the `CHTL/tests/` directory (e.g., `MyNewFeatureTest.cpp`).
2.  Write your test using the Google Test framework.
3.  Add your new test file to the `CHTL/tests/CMakeLists.txt` file. You can follow the pattern of the existing tests in that file.
4.  Run the build script to ensure your new test compiles and passes.

## Coding Conventions

### C++

*   We follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with a few exceptions.
*   Use Doxygen-style comments for all public classes and methods.

### Python

*   We follow the [PEP 8](https://www.python.org/dev/peps/pep-0008/) style guide.

We look forward to your contributions!
