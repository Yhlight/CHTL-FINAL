# CHTL Development Roadmap

This document outlines the planned development phases for the CHTL programming language and its associated tooling.

## Phase 1: Project Setup & Core Infrastructure (Current)
- [x] Set up project structure, build system (CMake, Python script), and version control.
- [x] Establish the basic compiler architecture (Lexer, Parser, AST, Generator).
- [x] Implement a simple "Hello, World" equivalent to verify the end-to-end toolchain.
- [x] Develop a robust testing framework (TDD).

## Phase 2: Basic CHTL Features
- [x] Implement core syntax: comments, text nodes, and literals.
- [x] Implement element nodes and attributes.
- [x] Implement basic `style` blocks for inline styles.

## Phase 3: Advanced CHTL Styling
- [x] Implement automated class/id generation from `style` blocks.
- [ ] Implement context deduction (`&`) for selectors.
- [ ] Implement attribute arithmetic (`+`, `-`, `*`, `/`, `%`, `**`).
- [ ] Implement referenced attributes (e.g., `width: #box.width + 10px;`).
- [ ] Implement conditional attribute expressions (e.g., `color: width > 50px ? "red" : "blue";`).

## Phase 4: Templating and Customization
- [ ] Implement `[Template]` blocks (`@Style`, `@Element`, `@Var`).
- [ ] Implement template inheritance (`inherit` and composition).
- [ ] Implement `[Custom]` blocks for specialization.
- [ ] Implement `delete` and `insert` operations for customization.

## Phase 5: Modularity and Imports
- [ ] Implement `[Origin]` blocks for raw code embedding.
- [ ] Implement `[Import]` for CHTL, HTML, CSS, and JS files.
- [ ] Implement `[Namespace]` for code organization and conflict resolution.

## Phase 6: Configuration and Tooling
- [ ] Implement `[Configuration]` groups to customize language behavior.
- [ ] Develop the Command Line Interface (CLI) for compiling CHTL files.
- [ ] Begin development of the VSCode IDE extension (syntax highlighting, basic snippets).

## Phase 7: CHTL JS Integration
- [ ] Set up the CHTL JS compiler architecture.
- [ ] Implement the "salt bridge" (`SharedCore`) for communication between CHTL and CHTL JS compilers.
- [ ] Implement local `script` blocks and the `[__CHTLJS__]` preprocessor.
- [ ] Implement enhanced selectors (`{{...}}`).
- [ ] Implement responsive values (`$js_var$`).

## Phase 8: Advanced CHTL JS Features
- [ ] Implement `Listen` blocks, event binding operators (`&->`), and `Delegate` for event handling.
- [ ] Implement the `Animate` function for complex animations.
- [ ] Implement the `Router` for Single Page Application (SPA) development.
- [ ] Implement `Vir` (virtual objects) for metaprogramming capabilities.

## Phase 9: Module System (CMOD & CJMOD)
- [ ] Develop the CMOD packaging system for distributing CHTL modules.
- [ ] Develop the CJMOD API and packaging system for extending CHTL JS.
- [ ] Implement the official modules (`Chtholly`, `Yuigahama`).

## Phase 10: Refinement and Beta Release
- [ ] Conduct extensive testing, performance profiling, and bug fixing.
- [ ] Write comprehensive documentation for all features.
- [ ] Gather community feedback and iterate towards a stable 1.0 release.
