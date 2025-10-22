# Contributing to CHTL

First off, thank you for considering contributing to CHTL! It's people like you that make CHTL such a great tool.

## Where to Start?

If you're new to the project, a good place to start is the [ROADMAP.md](ROADMAP.md) file. It outlines the current state of the project and what we're working on next. You can also check the open issues for bugs or feature requests that you might be able to help with.

## Setting Up the Development Environment

CHTL is a C++ project that uses CMake for its build system. To get started, you'll need the following:

- A C++ compiler that supports C++17 (like GCC or Clang)
- CMake (version 3.10 or higher)
- Make
- Python 3 (for the build script)

Once you have these dependencies installed, you can clone the repository and run the build script:

```bash
git clone https://github.com/your-username/CHTL.git
cd CHTL
python3 build.py
```

The `build.py` script will create a `build` directory, run CMake to generate the build files, and then run Make to compile the project. It will also run all the tests to make sure everything is working correctly.

## Running Tests

To run the tests, simply run the `build.py` script:

```bash
python3 build.py
```

The script will automatically find and run all the test executables in the `build/tests` directory.

## Code Style

We use a consistent code style to make the codebase easier to read and maintain. Before submitting a pull request, please make sure your code adheres to the following guidelines:

- Use Doxygen-style comments for all public classes and functions.
- Use 4 spaces for indentation.
- Follow the existing code style in the file you are editing.

## Submitting Changes

When you're ready to submit your changes, please follow these steps:

1.  Create a new branch for your changes:
    ```bash
    git checkout -b my-new-feature
    ```
2.  Make your changes and commit them with a clear and descriptive commit message.
3.  Push your branch to your fork:
    ```bash
    git push origin my-new-feature
    ```
4.  Open a pull request on the main CHTL repository.

Thank you for your contribution!
