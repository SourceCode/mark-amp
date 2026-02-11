# Contributing to MarkAmp

Thank you for your interest in contributing to MarkAmp! This guide covers everything you need to know to get started.

---

## Development Environment Setup

### Prerequisites

| Tool         | Version | Install                                      |
| ------------ | ------- | -------------------------------------------- |
| CMake        | ≥ 3.30  | `brew install cmake` / `choco install cmake` |
| Ninja        | latest  | `brew install ninja` / `choco install ninja` |
| C++ compiler | C++23   | Clang 17+, GCC 14+, MSVC 17.8+               |
| Git          | latest  | Standard installation                        |

### Building

```bash
# Clone
git clone https://github.com/markamp/markamp.git
cd markamp

# Debug build with tests
./scripts/build_debug.sh

# Run tests
./scripts/run_tests.sh

# Release build
./scripts/build_release.sh
```

See `docs/v1_docs/build_guide.md` for detailed platform-specific instructions.

---

## Code Style

### Conventions

- **C++23** standard — use modern features (structured bindings, `std::expected`, concepts) where they improve clarity
- **No `any` or `unknown` types** — all interfaces fully typed
- **Naming:**
  - Classes/structs: `PascalCase`
  - Functions/methods: `snake_case`
  - Constants: `kPascalCase`
  - Member variables: `snake_case_` (trailing underscore)
  - Namespaces: `lower::case`
- **Headers:** `#pragma once`, include guards not used
- **Include order:** own header → project headers → third-party → standard library (each group alphabetically)
- **Formatting:** `clang-format` with `.clang-format` config at repo root

### Formatting

```bash
# Format all source files
find src tests -name '*.cpp' -o -name '*.h' -o -name '*.mm' | xargs clang-format -i
```

---

## How to Add New Features

### 1. Core logic first

Add functionality to `src/core/` without wx dependencies. This keeps the logic testable.

### 2. Define events

If your feature needs to communicate with other components, add an event to `src/core/Events.h`:

```cpp
struct MyNewEvent : Event
{
    std::string data;
    [[nodiscard]] auto type_name() const -> std::string override
    {
        return "MyNewEvent";
    }
};
```

### 3. Wire up UI

Connect the core logic to the UI layer in `src/ui/`. Subscribe to events and update the interface.

### 4. Write tests

Add tests in `tests/unit/` (unit) or `tests/integration/` (integration). Use Catch2.

---

## How to Add New Themes

1. Create a JSON file in `resources/themes/`:

```json
{
  "id": "my-theme",
  "name": "My Theme",
  "author": "Your Name",
  "description": "A custom theme",
  "colors": {
    "--bg-app": "#1a1a2e",
    "--bg-panel": "#1f1f3a",
    "--bg-header": "#16162b",
    "--bg-input": "#12122a",
    "--text-main": "#e0e0e0",
    "--text-muted": "#808090",
    "--accent-primary": "#00d4ff",
    "--accent-secondary": "#ff6b9d",
    "--border-light": "#3a3a5a",
    "--border-dark": "#0a0a1a"
  }
}
```

2. Register it in `ThemeRegistry::load_builtin_themes()` if it should be built-in
3. Run the theme validator to check contrast ratios:
   - All text/background combinations must meet WCAG AA (4.5:1)

---

## How to Add Syntax Highlighting Languages

1. Open `src/rendering/CodeBlockRenderer.cpp`
2. Add the language keywords and patterns to the language registry
3. Add tests in `tests/unit/test_syntax_highlighter.cpp`

---

## Testing Requirements

- All new features **must** have unit tests
- Minimum 80% code coverage for new files
- Integration tests for cross-component interactions
- Run the full suite before submitting a PR:

```bash
./scripts/run_tests.sh
```

---

## Pull Request Process

1. **Fork** the repository
2. **Create a branch** from `develop`: `git checkout -b feature/my-feature`
3. **Make changes** following the coding conventions above
4. **Add tests** for your changes
5. **Run tests:** `./scripts/run_tests.sh`
6. **Format code:** `clang-format` all modified files
7. **Commit** with a clear message: `feat: add mermaid error overlay`
8. **Push** and open a PR against `develop`

### Commit Message Convention

```
type: short description

Longer description if needed.

Types: feat, fix, docs, style, refactor, perf, test, chore
```

### PR Checklist

- [ ] Tests pass on all platforms (CI will verify)
- [ ] Code formatted with clang-format
- [ ] Documentation updated if applicable
- [ ] No new compiler warnings
- [ ] No `any` or `unknown` types introduced
