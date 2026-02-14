# Claude Code Instructions for MarkAmp

## Build & Test

- Configure: `cmake --preset debug`
- Build: `cmake --build build/debug -j$(sysctl -n hw.ncpu)`
- Test: `cd build/debug && ctest --output-on-failure`
- Release: Run `cpp-git-commit-version-push` workflow

## Code Style

- C++23 with trailing return types
- 4-space indentation, Allman brace style
- Use `auto` with trailing return type for function declarations
- Namespace: `markamp::core`, `markamp::ui`, `markamp::rendering`, `markamp::platform`, `markamp::app`
- Use `MARKAMP_LOG_*` macros for logging (DEBUG, INFO, WARN, ERROR)
- Event types declared in Events.h using `MARKAMP_DECLARE_EVENT` macros
- Tests use Catch2 with descriptive section names
- Never use `any` or `unknown` types — always define precise types, interfaces, or generics

## Key Architecture

- **EventBus** is the backbone — all inter-component communication goes through it
- Services are passed via **constructor injection** (NOT ServiceRegistry singleton)
- `PluginContext` aggregates all 24 service pointers for extension access
- Config uses **YAML frontmatter** in Markdown files
- `ThemeEngine` provides cached wxBrush/wxPen/wxFont objects
- `IMathRenderer` / `MathRenderer` handles LaTeX→Unicode math rendering
- `IMermaidRenderer` / `MermaidRenderer` handles Mermaid diagram rendering

## Important Conventions

- Never use `catch(...)` — always use typed exception handlers
- Guard filesystem operations with `std::error_code` overloads
- Use `[[nodiscard]]` on all query methods
- Check pointer validity before dereferencing service pointers from PluginContext
- Platform abstractions: MacPlatform (Objective-C++), WinPlatform (Win32), LinuxPlatform (GTK/GDK)

## Project Structure

- `src/app/` — Application entry point (`MarkAmpApp`)
- `src/core/` — Core logic, events, config, plugins, extension services, math/mermaid renderers
- `src/ui/` — wxWidgets UI components (MainFrame, EditorPanel, PreviewPanel, etc.)
- `src/rendering/` — HtmlRenderer, CodeBlockRenderer, rendering primitives
- `src/platform/` — Platform abstractions (Mac/Win/Linux)
- `tests/unit/` — Catch2 unit tests (27 targets)
- `docs/` — Architecture, API reference, user guide
- `themes/` — Markdown-based theme definitions (YAML frontmatter)
