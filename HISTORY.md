# MarkAmp Release History

## v1.2.5 — 2026-02-11

### Highlights

Advanced performance patterns #21-#40: Sublime Text-inspired rendering and editing primitives covering glyph caching, hit-testing, caret/selection painting, double-buffered paint, scroll blitting, incremental line wrapping, IME composition, and input priority dispatch. Adds comprehensive unit tests for all new primitives.

### Added

- **Core Primitives**: `AdaptiveThrottle` for frame-rate-adaptive update coalescing, `AsyncPipeline` for staged background processing, `ChunkedStorage` for cache-friendly contiguous block allocation, `CompilerHints` for branch-prediction and prefetch intrinsics, `FrameBudgetToken` for frame-time budget accounting, `GenerationCounter` for lock-free generation-based invalidation, `GraphemeBoundaryCache` for Unicode grapheme cluster lookups, `IMECompositionOverlay` for IME inline composition rendering, `InputPriorityDispatcher` for prioritized input event dispatch, `StableLineId` for content-addressed line identity across edits, `StyleRunStore` for run-length encoded style spans, `TextSpan` for zero-copy text range references
- **Rendering Primitives**: `CaretOverlay` for phase-animated cursor blinking, `DoubleBufferedPaint` for flicker-free paint with damage-rect merging, `GlyphAdvanceCache` for per-font glyph width memoization, `HitTestAccelerator` for O(log n) column/line hit-testing, `IncrementalLineWrap` for incremental soft-wrap recomputation, `PrefetchManager` for viewport-aware line prefetching, `ScrollBlit` for bit-copy scroll blitting with damage tracking, `SelectionPainter` for multi-selection range rendering
- **Test Coverage**: `test_perf_patterns_21_40` — 20 test cases covering all new core and rendering primitives

### Changed

- Expanded `tests/CMakeLists.txt` with the new `test_perf_patterns_21_40` test target and source linkage

### Fixed

- **HitTestAccelerator boundary case** — Fixed `column_at_x` to correctly handle x-coordinates landing exactly on a glyph's right edge using `lower_bound` with proper boundary condition checks

---

## v1.1.4 — 2026-02-11

### Highlights

Performance infrastructure, visual polish, and crash fixes. Adds 20+ performance primitives (arena allocators, object pools, SPSC queues, frame schedulers, viewport caching, dirty region tracking), visual polish with gradient surfaces and micro-animations, and comprehensive test coverage improvements.

### Added

- **Performance Infrastructure**: FrameArena monotonic allocator, ObjectPool with free-list recycling, SPSCQueue lock-free single-producer-single-consumer queue, FrameScheduler with priority-based task execution, DocumentSnapshot atomic publish/consume, FrameHistogram for latency percentile tracking
- **Data Structures**: PieceTable for O(log n) text editing, LineIndex for offset↔line/column mapping, IncrementalSearcher for progressive text search, AsyncFileLoader and AsyncHighlighter for background processing
- **Rendering Primitives**: DirtyRegionAccumulator for rect-based invalidation, LRUCache (ViewportCache) for viewport line caching, LazyCache for deferred computation, LineLayoutCache for per-line measurement caching, ViewportState with prefetch margin support
- **Visual Polish (Phase 8)**: Layered surface gradients on Toolbar, StatusBar, and Sidebar panels; graduated drop shadows on sidebar and ShortcutOverlay; soft alpha-blended separators; SplitterBar hover micro-animation with wxTimer; 8px spacing grid normalization
- **UI Components**: FloatingFormatBar, LinkPreviewPopover, ImagePreviewPopover, TableEditorOverlay, ThemeTokenEditor, StartupPanel, RecentWorkspaces
- **Mermaid Diagram Rendering**: MermaidRenderer and MermaidBlockRenderer for inline diagram support
- **CoalescingTask**: Debounced background task execution for UI update coalescing
- **Test Coverage**: test_visual_polish (14 cases), test_performance_infra (20+ cases), test_theme_phase4, test_mermaid_phase3, test_split_view_advanced, test_sidebar_filter
- **Documentation**: WINDOW_VISUAL_FX.md, sublime_performance_patterns.md

### Changed

- Expanded `src/CMakeLists.txt` with new source files for all added components
- Refactored `tests/CMakeLists.txt` for correct linking of all 8 test targets
- Enhanced `EditorPanel` with dwell-start previews, format bar, and table editor integration
- Extended `ThemeEngine` with brush/pen helpers and font token support
- Expanded `Events.h` with new event types for all added features
- Expanded `EventBus` with thread-safe subscription management

### Fixed

- **Crash: Unsigned char gradient overflow** — Fixed arithmetic underflow in gradient paint code (`Toolbar.cpp`, `StatusBarPanel.cpp`, `LayoutManager.cpp`) that caused crashes on dark themes. Replaced raw unsigned subtraction with safe int lerp + `std::clamp`
- **FrameArena::reset() compile error** — Fixed deleted `operator=` on `std::pmr::monotonic_buffer_resource` by using placement new destroy-reconstruct pattern
- **test_performance_infra build failures** — Switched to direct source linking instead of markamp_core to avoid unresolved UI symbols
- **test_editor_qol linker errors** — Added missing UI source files (FloatingFormatBar, LinkPreviewPopover, TableEditorOverlay, ImagePreviewPopover)
- **test_performance_infra nodiscard warnings** — Fixed unused `[[nodiscard]]` return values

---

## v1.1.3 — 2026-02-11

### Highlights

Major feature release adding editor QoL improvements, command palette, YAML-based configuration, Lucide icon integration, native macOS window controls, and markdown-based theme loading.

### Added

- **Editor QoL Features**: Zoom in/out (mouse wheel + keyboard shortcuts), trim trailing whitespace, accurate word/line/character counts in status bar, Zen Mode (distraction-free editing), session restore on startup
- **Command Palette**: Fuzzy-searchable command launcher (Ctrl+Shift+P) for quick access to all editor actions
- **Breadcrumb Bar**: Document path navigation breadcrumbs above the editor
- **Snippet System**: Insertable code/text snippets from the command palette
- **Native macOS Window Controls**: Replaced custom window chrome with macOS native traffic-light controls; UI layout adapts to control positions
- **File Tree with Lucide Icons**: SVG icon rendering via nanosvg for file-type icons in the sidebar tree
- **Markdown Theme Loading**: Themes can now be defined in Markdown files with YAML frontmatter via `ThemeLoader`
- **YAML Configuration**: Migrated configuration format from JSON to Markdown/YAML frontmatter using `yaml-cpp`
- **Builtin Theme Color Tokens**: Extended all 8 built-in themes with editor-specific and UI tokens (editor_bg, editor_fg, editor_selection, editor_line_number, editor_cursor, editor_gutter, list_hover, list_selected, scrollbar_thumb, scrollbar_track)
- **Preview Panel Zoom**: Text size in preview panel adjusts via zoom controls
- **Layout Manager Enhancements**: Sidebar toggle and zen-mode layout support
- **SplitView Improvements**: Programmatic sash positioning and minimum pane sizes
- **Event System Expansion**: New events for zoom, zen mode, session restore, and theme changes
- **StatusBarPanel Enhancements**: Real-time document statistics (words, lines, characters) and encoding display
- **Startup Panel & Recent Workspaces**: New landing panel with recent workspace history

### Changed

- Simplified `tests/CMakeLists.txt` — removed 390+ lines of legacy test targets in favor of streamlined test definitions
- Updated `vcpkg.json` to include `yaml-cpp` dependency
- Refactored `FileTreeCtrl` for icon-aware rendering with `nanosvg`
- Enhanced `EditorPanel` with 400+ lines of new functionality
- Expanded `MainFrame` with 450+ lines for command palette, session restore, and toolbar integration

### Fixed

- Heading rendering bug: visible `#` characters no longer appear in preview
- Shutdown crash caused by `EventBus`/`Subscription` lifetime issue
- Compilation errors in editor control tests

---

## v1.0.2 — 2026-02-11

- Initial release with full feature set (see CHANGELOG.md for details)

## v1.0.0 — 2026-02-11

- Project inception and initial commit
