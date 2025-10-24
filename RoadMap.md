# CHTL Project Roadmap

This document outlines the development roadmap for the CHTL project.

## Phase 1: Core Language Features

- [x] Lexer and Tokenizer
- [x] Parser (AST)
- [x] Basic HTML Generation
- [x] Comments (`//`, `/**/`, `#`)
- [x] Text Nodes (`text {}` and `text:` attribute)
- [x] Element Nodes
- [x] Attributes (`key: value;`)
- [x] Unquoted literals

## Phase 2: Advanced Styling and Expressions

- [x] Local Style Blocks (`style {}`)
- [x] Inline Styles
- [ ] Automatic class/id generation
- [ ] Contextual `&` selector
- [ ] Attribute arithmetic (+, -, *, /, %, **)
- [ ] Attribute referencing
- [ ] Attribute conditional expressions

## Phase 3: Templates and Customization

- [ ] Style Group Templates (`[Template] @Style`)
- [ ] Element Templates (`[Template] @Element`)
- [ ] Variable Group Templates (`[Template] @Var`)
- [ ] Template Inheritance
- [ ] Custom Style Groups (`[Custom] @Style`)
- [ ] Custom Elements (`[Custom] @Element`)
- [ ] Custom Variable Groups (`[Custom] @Var`)
- [ ] Specialization (delete, insert, etc.)

## Phase 4: Modularity and Imports

- [ ] Raw Embedding (`[Origin]`)
- [ ] File Imports (`[Import]`)
- [ ] Namespaces (`[Namespace]`)
- [ ] Constraints (`except`)

## Phase 5: Configuration and Tooling

- [ ] Configuration Groups (`[Configuration]`)
- [ ] CLI (Command Line Interface)
- [ ] Build System (CMake + Python)
- [ ] VSCode IDE Extension

## Phase 6: CHTL JS Integration

- [ ] CHTL JS Preprocessor
- [ ] Script Loader (`ScriptLoader {}`)
- [ ] Local `script` blocks
- [ ] Enhanced Selectors (`{{selector}}`)
- [ ] Enhanced Event Listeners (`Listen`, `&->`)
- [ ] Event Delegation (`Delegate`)
- [ ] Animations (`Animate`)
- [ ] Virtual Objects (`Vir`)
- [ ] Routing (`Router`)
- [ ] Dynamic Attribute Conditional Expressions
- [ ] Reactive Values (`$variable$`)

## Phase 7: CMOD and CJMOD

- [ ] CMOD (CHTL Modules)
- [ ] CJMOD (CHTL JS Modules)
- [ ] Official Modules (Chtholly, Yuigahama)
