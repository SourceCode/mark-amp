# Agent Context for MarkAmp

## Project Overview

MarkAmp is a cross-platform, retro-futuristic Markdown editor built with **C++23** and **wxWidgets 3.2.9**. It features split-view editing, syntax highlighting, Mermaid diagram rendering, a theme system with 8 built-in themes, multi-file tab management, command palette, Zen Mode, performance infrastructure with arena allocators and viewport caching, advanced rendering primitives (glyph caching, hit-testing, double-buffered paint, scroll blitting), visual polish with gradient surfaces and micro-animations, native platform integration, a VS Code-inspired settings panel with 40 VS Code-equivalent editor and syntax highlighting settings, toast notifications, activity bar navigation with press feedback and drag handles, a plugin architecture with manifest-driven contribution points, 80+ editor action events with full Edit/View menu build-out, editor right-click context menu, file tree type-ahead search with folder sort and icon tinting, enriched status bar (EOL mode, encoding cycling, indent mode, zoom level, "SAVED" flash), clickable breadcrumb bar with styled chevrons and heading glyphs, Find/Replace, print support with print-friendly CSS, sticky scroll headings, default Markdown snippets, recent files management, 60+ configuration defaults, scroll-to-top preview button, comprehensive stability hardening (40+ fixes for input validation, thread safety, typed exception handling, filesystem resilience, and arithmetic underflow guards), and R20–R22 UI polish (command palette fuzzy-match highlighting, toolbar zoom slider, window chrome filename truncation, and radial button glow).

**Current Version:** 1.8.11

## Key Workflows

- **Configure:** `cmake --preset debug`
- **Build:** `cmake --build build/debug -j$(sysctl -n hw.ncpu)`
- **Test:** `cd build/debug && ctest --output-on-failure`
- **Release:** Run `cpp-git-commit-version-push` workflow

## Codebase Structure

- `src/app/` — Application entry point (`MarkAmpApp`)
- `src/core/` — Core logic: Config (YAML), Theme, ThemeRegistry, Color, Events, EventBus, BuiltinThemes, RecentWorkspaces, ThemeLoader, PieceTable, LineIndex, FrameArena, FrameScheduler, Profiler, AsyncHighlighter, AsyncFileLoader, IncrementalSearcher, DocumentSnapshot, SPSCQueue, CoalescingTask, AdaptiveThrottle, AsyncPipeline, ChunkedStorage, CompilerHints, FrameBudgetToken, GenerationCounter, GraphemeBoundaryCache, IMECompositionOverlay, InputPriorityDispatcher, StableLineId, StyleRunStore, TextSpan, **IPlugin** (plugin interface), **PluginManager** (lifecycle management)
- `src/ui/` — UI components: MainFrame (50+ Edit/View menu items, command-line file open, print, recent files), EditorPanel (60+ VS Code-inspired improvements, right-click context menu, sticky scroll, 8 Markdown snippets), PreviewPanel, FileTreeCtrl (auto-scroll, indent guides, 30+ file-type icons, type-ahead search, folder sort, collapse/expand all, enriched context menus, tooltips), SplitView, StatusBarPanel (filename, language, file size, EOL mode, encoding cycling, indent mode, zoom level, SAVED flash, clickable Go-To-Line), LayoutManager (Save All, Revert File, Close All Tabs, drag-and-drop, 1,500+ lines of editor action wiring), TabBar, CommandPalette, BreadcrumbBar (clickable segments), StartupPanel, FloatingFormatBar, LinkPreviewPopover, ImagePreviewPopover, TableEditorOverlay, ThemeTokenEditor, SplitterBar, ShortcutOverlay, Toolbar, CustomChrome, **SettingsPanel** (settings editor), **NotificationManager** (toast notifications), **ActivityBar** (vertical icon rail)
- `src/rendering/` — HtmlRenderer, CodeBlockRenderer, MermaidBlockRenderer, DirtyRegion, ViewportCache, CaretOverlay, DoubleBufferedPaint, GlyphAdvanceCache, HitTestAccelerator, IncrementalLineWrap, PrefetchManager, ScrollBlit, SelectionPainter
- `src/platform/` — Platform abstractions: MacPlatform (Objective-C++ bridge)
- `tests/unit/` — Catch2 unit tests (10 test targets, 100% pass rate)
- `scripts/` — Build, test, and packaging helpers
- `themes/` — Markdown-based theme definitions (YAML frontmatter)
- `resources/icons/lucide/` — Lucide SVG icons for file tree
- `docs/` — Architecture, API reference, user guide, security audit, performance patterns

## Key Systems

### Plugin Architecture (`IPlugin.h`, `PluginManager.h/.cpp`)

- `IPlugin` — Abstract interface: `manifest()`, `activate(ctx)`, `deactivate()`
- `PluginManifest` — Declares contribution points: commands, keybindings, snippets, menus, settings, themes
- `PluginContext` — Runtime context providing `EventBus*`, `Config*`, and `register_command_handler`
- `PluginManager` — Manages lifecycle (register → activate → deactivate), processes contributions, wires to CommandPalette and ShortcutManager

### Settings System (`SettingsPanel.h/.cpp`)

- Searchable/filterable settings with category grouping
- `SettingDefinition` struct: id, label, description, category, type (Boolean/Integer/Double/String/Choice), default, choices
- Import/export settings to Markdown YAML frontmatter
- Collapsible category groups with modified indicators (●)

### Notification System (`NotificationManager.h/.cpp`)

- Toast notifications: Info, Warning, Error, Success levels
- Auto-dismiss with configurable duration; sticky (duration=0) support
- Animated fade-in/out with up to 3 stacked toasts

### Activity Bar (`ActivityBar.h/.cpp`)

- Vertical icon rail: File Explorer, Search, Settings, Themes
- Fires `ActivityBarSelectionEvent` on click for sidebar panel switching

### Event System (`Events.h`)

- Settings events: `SettingsOpenRequestEvent`, `SettingChangedEvent`
- Plugin events: `PluginActivatedEvent`, `PluginDeactivatedEvent`
- Notification events: `NotificationEvent` (with `NotificationLevel`)
- Activity bar events: `ActivityBarSelectionEvent` (with `ActivityBarItem`)
- Theme events: `ThemeChangedEvent`
- Tab events: `TabSwitchedEvent`, `TabCloseRequestEvent`, `TabSaveRequestEvent`
- Workspace events: `WorkspaceRefreshRequestEvent`, `ShowStartupRequestEvent`
- Editor action events (80+): Find, Replace, DuplicateLine, ToggleComment, DeleteLine, WrapToggle, MoveLineUp/Down, JoinLines, SortLinesAsc/Desc, TransformUpper/Lower/Title, SelectAllOccurrences, ExpandLineSelection, InsertLineAbove/Below, FoldAll/UnfoldAll, ToggleLineNumbers/Whitespace, CopyLineUp/Down, RevertLines, DeleteDuplicateLines, TransposeChars, IndentSelection, SelectWord/Paragraph, ToggleReadOnly, JumpToBracket, ToggleMinimap, Print, and many more

### Editor Improvements (EditorPanel — 60+ features)

- **Phase 6**: Auto-closing brackets, multi-cursor, sticky scroll, inline color preview, font ligatures, smooth caret, auto-save, link auto-complete, drag-and-drop, edge ruler, go-to-symbol, block comment, smart select
- **Phase 7**: Cursor surrounding lines, scroll beyond last line, smooth scrolling, copy line, join/reverse/dedup lines, transpose chars, move text, block indent, cursor undo/redo, select all occurrences, add next match, toggle word wrap
- **Phase 8**: Fold/unfold regions, expand line selection, delete line, toggle whitespace/line numbers, bracket operations, duplicate line, case transforms, sort lines, insert line above, trim whitespace, toggle minimap
- **Phase 9**: Copy line up/down, delete left/right, add/remove line comment, toggle editor features, select word/paragraph, read-only mode, indentation conversion

## Dependencies (vcpkg)

wxWidgets, nlohmann-json, md4c, catch2, fmt, spdlog, yaml-cpp, nanosvg

## Standards

- C++ Standard: C++23 (C++26-ready patterns)
- Build: CMake 3.30+ with presets (debug, release, release-static)
- Documentation: Markdown in `docs/`
- Versioning: Semantic Versioning (managed in `CMakeLists.txt`, `vcpkg.json`)
- Linting: clang-format, clang-tidy (configs in project root)
- Testing: 10 test targets covering core, rendering, UI, theme, performance primitives, and crash regressions
