# CHTL Project Roadmap

## Phase 1: Core Compiler Implementation

- [ ] **Lexer:** Implement the lexical analyzer to tokenize the CHTL source code.
- [ ] **Parser:** Implement the parser to build an Abstract Syntax Tree (AST) from the token stream.
- [ ] **Generator:** Implement the code generator to produce HTML and CSS from the AST.
- [ ] **Testing:** Set up a comprehensive test suite to ensure the correctness of the compiler.

## Phase 2: CHTL JS Integration

- [ ] **CHTL JS Lexer:** Implement the lexer for CHTL JS syntax.
- [ ] **CHTL JS Parser:** Implement the parser for CHTL JS to extend the AST.
- [ ] **CHTL JS Generator:** Implement the generator for CHTL JS to produce JavaScript.
- [ ] **Salt Bridge:** Develop the "Salt Bridge" communication layer between the CHTL and CHTL JS compilers.

## Phase 3: Module System

- [ ] **CMOD:** Implement the CMOD module system for packaging and distributing CHTL modules.
- [ ] **CJMOD:** Implement the CJMOD module system for CHTL JS.
- [ ] **Module Loader:** Create a module loader to handle module imports and dependencies.

## Phase 4: CLI and IDE Support

- [ ] **CLI:** Develop a command-line interface for the CHTL compiler.
- [ ] **VSCode Extension:** Create a VSCode extension with features like syntax highlighting, code completion, and real-time preview.
