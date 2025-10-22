# CHTL Project Roadmap

This document aims to track the development progress of the CHTL project and provide a clear, iterative guide for future work.

## Phase 0: Project Stabilization (Completed)
- **Goal:** Ensure the project's foundation is stable and the development environment is fully functional.
- **Accomplishments:**
    - **Dependency Fix:** Resolved a critical build failure by adding the missing `nlohmann/json.hpp` dependency required by the language server.
    - **Build Verification:** Successfully ran the complete build and testing script (`build.py`), confirming that all existing tests pass and the project compiles without errors.
    - **Code Review:** Conducted a comprehensive review of the codebase, `CHTL.md` specification, and the existing roadmap to assess the current state of the project.

## Phase 1: Advanced Language Features
- **Goal:** Implement CHTL's advanced features to enhance its power and flexibility.
- **Tasks:**
    - **1. Configuration & Scoping:**
        - `[Configuration]` Groups: Implement configuration blocks to allow developers to customize language behavior (e.g., keywords, auto-add features).
        - `use` Syntax: Implement the `use` keyword for specifying which configuration group or document type is active.
    - **2. Constraints:**
        - `except` Keyword: Add support for defining domain constraints on elements to restrict their children.
    - **3. Conditional Rendering:**
        - `if`/`else if`/`else`: Implement static conditional rendering within elements based on attribute expressions.

## Phase 2: Advanced CHTL JS Features
- **Goal:** Enhance interactivity and dynamic capabilities by implementing advanced CHTL JS features.
- **Tasks:**
    - **1. Data Binding:**
        - **Reactive Values:** Implement reactive values (`$jsVariable$`) to bridge CHTL attributes and CHTL JS variables.
    - **2. Dynamic Expressions:**
        - **Dynamic Attribute Conditionals:** Add support for conditional expressions that change based on runtime values.
    - **3. Single-Page Applications (SPA):**
        - **Router:** Implement the `Router` feature for client-side routing.

## Phase 3: Developer Experience & Tooling
- **Goal:** Improve the developer ecosystem by finalizing APIs and enhancing editor support.
- **Tasks:**
    - **1. CJMOD API:**
        - **API Implementation:** Complete the development of the `Syntax`, `Arg`, and `CJMODGenerator` APIs as specified in `CHTL.md`.
        - **Documentation:** Write detailed documentation and usage examples for the CJMOD API.
    - **2. VSCode Extension:**
        - **Syntax Highlighting:** Create and refine syntax highlighting rules for both CHTL and CHTL JS.
        - **IntelliSense:** Provide code completion and suggestions by leveraging the language server and module query tables.
        - **Code Formatting:** Implement a code formatter to ensure consistent style.
    - **3. Build & CLI Enhancements:**
        - **Build Monitor:** Create a compile monitor to track build times and memory usage.
        - **CLI Features:** Add new options to the CLI, such as `--inline` for embedding CSS/JS and project scaffolding commands.

## Phase 4: Ecosystem & Official Modules
- **Goal:** Develop and release official modules to showcase the language's capabilities and provide reusable components.
- **Tasks:**
    - **1. `Chtholly` Module (CMOD + CJMOD):**
        - Implement CMOD components (e.g., Accordion, Photo Album, Progress Bar).
        - Implement CJMOD features (`PrintMylove`, `INeverAway`).
    - **2. `Yuigahama` Module (CMOD):**
        - Implement CMOD components (e.g., Music Player, Sakura Rain Effect).
