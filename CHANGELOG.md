# Changelog

All notable changes to MarkAmp are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.3.6] - 2026-02-12

### Added

- TabBar component: horizontally scrollable tab bar for multi-file editing with active indicator, modified dot, close buttons, tab cycling, and batch close operations
- File tree context menu: Open, Reveal in Finder, Copy Path, Copy Relative Path
- File tree keyboard navigation: arrow keys, Enter/Space to open, Left/Right to expand/collapse
- Tab events: TabSwitchedEvent, TabCloseRequestEvent, TabSaveRequestEvent, TabSaveAsRequestEvent, FileReloadRequestEvent, GoToLineRequestEvent
- Multi-file buffer management in LayoutManager with cursor/scroll state persistence
- Tab keyboard shortcuts: Cmd+W (close tab), Ctrl+Tab / Ctrl+Shift+Tab (cycle tabs)
- Window title shows filename and modification status (● prefix)
- Auto-save and external file change detection
- Crash regression tests (test_crash_regressions)

### Changed

- Expanded src/CMakeLists.txt with TabBar.cpp source
- Expanded tests/CMakeLists.txt with test_crash_regressions target
- Enhanced LayoutManager with 460+ lines of multi-file tab management
- Extended MainFrame with tab management methods and 3 new shortcuts
- Added file open callback, workspace root, and keyboard/context-menu handlers to FileTreeCtrl

### Fixed

- StartupPanel assertion: SetBackgroundStyle called before Create()
- PreviewPanel redundant FootnotePreprocessor::process() causing heading crash

## [1.2.5] - 2026-02-11

### Added

- 12 core primitives: AdaptiveThrottle, AsyncPipeline, ChunkedStorage, CompilerHints, FrameBudgetToken, GenerationCounter, GraphemeBoundaryCache, IMECompositionOverlay, InputPriorityDispatcher, StableLineId, StyleRunStore, TextSpan
- 8 rendering primitives: CaretOverlay, DoubleBufferedPaint, GlyphAdvanceCache, HitTestAccelerator, IncrementalLineWrap, PrefetchManager, ScrollBlit, SelectionPainter
- test_perf_patterns_21_40 test target (20 test cases)

### Changed

- Expanded tests/CMakeLists.txt with test_perf_patterns_21_40 target

### Fixed

- HitTestAccelerator boundary case in column_at_x for glyph right-edge hit detection

## [1.1.4] - 2026-02-11

### Added

- Performance infrastructure: FrameArena, ObjectPool, SPSCQueue, FrameScheduler, DocumentSnapshot, FrameHistogram
- Data structures: PieceTable, LineIndex, IncrementalSearcher, AsyncFileLoader, AsyncHighlighter
- Rendering: DirtyRegionAccumulator, ViewportCache, LazyCache, LineLayoutCache, ViewportState
- Visual polish: gradient surfaces, drop shadows, micro-animations, 8px spacing grid
- UI components: FloatingFormatBar, LinkPreviewPopover, ImagePreviewPopover, TableEditorOverlay, ThemeTokenEditor, StartupPanel, RecentWorkspaces
- Mermaid diagram rendering (MermaidRenderer, MermaidBlockRenderer)
- CoalescingTask for debounced background task execution
- 6 new test targets: test_visual_polish, test_performance_infra, test_theme_phase4, test_mermaid_phase3, test_split_view_advanced, test_sidebar_filter

### Fixed

- Unsigned char gradient overflow crash on dark themes
- FrameArena::reset() compile error
- test_performance_infra build and nodiscard warnings

## [1.1.3] - 2026-02-11

### Added

- Editor QoL: zoom (mouse wheel + keyboard), trim trailing whitespace, document statistics in status bar, Zen Mode, session restore
- Command Palette (Ctrl+Shift+P) with fuzzy search
- Breadcrumb Bar for document path navigation
- Snippet system insertable from command palette
- Native macOS window controls (traffic-light integration)
- Lucide SVG icons in file tree via nanosvg
- Markdown-based theme loading with YAML frontmatter (ThemeLoader)
- YAML configuration format via yaml-cpp (replaces JSON config)
- Extended builtin theme color tokens (editor_bg, editor_fg, editor_selection, editor_line_number, editor_cursor, editor_gutter, list_hover, list_selected, scrollbar_thumb, scrollbar_track)
- Preview panel zoom controls
- Layout manager zen-mode and sidebar-toggle support
- SplitView programmatic sash positioning
- New events: zoom, zen mode, session restore, theme changes
- StatusBarPanel real-time document statistics
- Startup panel with recent workspace history

### Changed

- Simplified tests/CMakeLists.txt (removed 390+ lines of legacy targets)
- Added yaml-cpp dependency to vcpkg.json
- Refactored FileTreeCtrl for icon-aware rendering
- Expanded EditorPanel with 400+ lines of new functionality
- Expanded MainFrame with 450+ lines for command palette, session restore, toolbar

### Fixed

- Heading rendering: visible `#` characters no longer appear in preview
- Shutdown crash from EventBus/Subscription lifetime issue
- Compilation errors in editor control tests

## [1.0.2] - 2026-02-11

### Added

- Initial release with full feature set.

## [1.0.0] - 2026-02-11

### Added

#### Core Editor

- Cross-platform Markdown editor built with C++23 and wxWidgets 3.2.9
- Three view modes: Source, Split (default), Preview — switchable via toolbar or Ctrl+1/2/3
- GitHub Flavored Markdown support: headings, emphasis, lists, tables, code blocks, task lists, blockquotes, footnotes, horizontal rules, strikethrough
- Syntax highlighting for 15+ programming languages (C, C++, Python, JavaScript, TypeScript, Rust, Go, Java, Ruby, Shell, SQL, JSON, YAML, HTML, CSS)
- Mermaid diagram rendering with inline error overlay for syntax errors
- Find and replace with match highlighting and navigation
- Line numbers, word wrap, bracket matching, auto-indent
- Undo/redo via command pattern
- Custom retro-futuristic window chrome

#### Theme System

- 8 built-in themes: Midnight Neon, Cyber Night, Solarized Dark, Classic Mono, High Contrast Blue, Matrix Core, Classic Amp, Vapor Wave
- Theme Gallery with live preview cards
- Theme import/export (JSON format)
- Custom theme creation with full color token specification
- Theme validation: structural integrity, color parsing, WCAG AA contrast checking

#### File Management

- Sidebar file tree with folder navigation
- Open individual files or entire folders
- Keyboard-navigable file tree (arrow keys, Enter to open)
- Resizable sidebar with persistent width

#### Accessibility

- Screen reader support with ARIA-compatible patterns
- High Contrast Blue theme meets WCAG AA
- Full keyboard navigation (Tab cycles focus zones)
- Scalable UI elements
- F1 keyboard shortcut reference

#### Performance

- Sub-500ms cold start
- Debounced preview updates
- Smooth scrolling for large documents
- Instant theme switching (<100ms)

#### UI & Design

- JetBrains Mono + Rajdhani font pairing
- Custom styled scrollbars
- Status bar: cursor position, encoding, Mermaid status, active theme name
- Responsive layout adapts to window size

#### Platform Support

- macOS: .dmg installer, Mac App Store .pkg with sandbox entitlements
- Windows: NSIS installer, portable ZIP, MSIX for Microsoft Store
- Linux: .deb, AppImage, .tar.gz with desktop integration

#### Build & CI

- CMake 3.30+ with presets (debug, release, release-static)
- GitHub Actions CI: 3-platform matrix, vcpkg caching, automated tests
- GitHub Actions Release: tag-triggered packaging and GitHub Release creation
- Dev scripts: build, test, clean, version bump

### Known Issues

- Mermaid rendering requires WebView component; may not render in headless environments
- Custom scrollbar styling limited on some Linux desktop environments
- Large Mermaid diagrams (>200 nodes) may render slowly
