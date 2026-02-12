# Agent Context for MarkAmp

## Project Overview

MarkAmp is a cross-platform, retro-futuristic Markdown editor built with **C++23** and **wxWidgets 3.2.9**. It features split-view editing, syntax highlighting, Mermaid diagram rendering, a theme system with 8 built-in themes, command palette, Zen Mode, performance infrastructure with arena allocators and viewport caching, visual polish with gradient surfaces and micro-animations, and native platform integration.

**Current Version:** 1.1.4

## Key Workflows

- **Configure:** `cmake --preset debug`
- **Build:** `cmake --build build/debug -j$(sysctl -n hw.ncpu)`
- **Test:** `cd build/debug && ctest --output-on-failure`
- **Release:** Run `cpp-git-commit-version-push` workflow

## Codebase Structure

- `src/app/` — Application entry point (`MarkAmpApp`)
- `src/core/` — Core logic: Config (YAML), Theme, ThemeRegistry, Color, Events, BuiltinThemes, RecentWorkspaces, ThemeLoader, PieceTable, LineIndex, FrameArena, FrameScheduler, Profiler, AsyncHighlighter, AsyncFileLoader, IncrementalSearcher, DocumentSnapshot, SPSCQueue, CoalescingTask
- `src/ui/` — UI components: MainFrame, EditorPanel, PreviewPanel, FileTreeCtrl, SplitView, StatusBarPanel, LayoutManager, CommandPalette, BreadcrumbBar, StartupPanel, FloatingFormatBar, LinkPreviewPopover, ImagePreviewPopover, TableEditorOverlay, ThemeTokenEditor, SplitterBar, ShortcutOverlay, Toolbar, CustomChrome
- `src/rendering/` — HtmlRenderer, CodeBlockRenderer, MermaidBlockRenderer, DirtyRegion, ViewportCache
- `src/platform/` — Platform abstractions: MacPlatform (Objective-C++ bridge)
- `tests/unit/` — Catch2 unit tests (8 test targets, 100% pass rate)
- `scripts/` — Build, test, and packaging helpers
- `themes/` — Markdown-based theme definitions (YAML frontmatter)
- `resources/icons/lucide/` — Lucide SVG icons for file tree
- `docs/` — Architecture, API reference, user guide, security audit, performance patterns

## Dependencies (vcpkg)

wxWidgets, nlohmann-json, md4c, catch2, fmt, spdlog, yaml-cpp, nanosvg

## Standards

- C++ Standard: C++23 (C++26-ready patterns)
- Build: CMake 3.30+ with presets (debug, release, release-static)
- Documentation: Markdown in `docs/`
- Versioning: Semantic Versioning (managed in `CMakeLists.txt`, `vcpkg.json`)
- Linting: clang-format, clang-tidy (configs in project root)
- Testing: 8 test targets covering core, rendering, UI, theme, and performance primitives
