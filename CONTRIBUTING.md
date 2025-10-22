# Contributing to CHTL

First off, thank you for considering contributing to CHTL! It's people like you that make CHTL such a great tool.

## Code of Conduct

This project and everyone participating in it is governed by the [CHTL Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to the project team.

## How Can I Contribute?

### Reporting Bugs

If you find a bug, please open an issue on our GitHub repository. When you are creating a bug report, please include as many details as possible. Fill out the required template, the information it asks for helps us resolve issues faster.

### Suggesting Enhancements

If you have an idea for an enhancement, please open an issue on our GitHub repository. Clearly describe the enhancement you're proposing and why it would be a valuable addition to CHTL.

### Pull Requests

We welcome pull requests! If you're planning to make a larger change, please open an issue first to discuss it with the maintainers.

1.  Fork the repository and create your branch from `main`.
2.  Make your changes and ensure that the test suite passes.
3.  Add tests for any new functionality.
4.  Issue that pull request!

## Development Setup

To get started with CHTL development, you'll need to have the following installed:

-   C++ compiler (supporting C++17)
-   CMake (version 3.10 or higher)
-   Git

Once you have these prerequisites, you can clone the repository and build the project:

```bash
git clone https://github.com/your-username/CHTL.git
cd CHTL
python build.py
```

## Running Tests

To run the test suite, use the `build.py` script with the `--test` flag:

```bash
python build.py --test
```

All tests must pass before a pull request will be merged.
