# MarkAmp Release History

## v1.5.8 — 2026-02-12

### Highlights

20+ usability fixes bringing MarkAmp's file tree, tab management, status bar, and editor UX to VS Code parity. Adds auto-scroll file tree, indent guide lines, enriched context menus (new file, delete, collapse/expand all), file-type icon expansion for 30+ extensions, tooltips with file size/child count, drag-and-drop file opening, Save All/Revert File/Close All commands, status bar filename/language/file-size display, and "EXPLORER" header branding.

### Added

- **Workspace Events**: `WorkspaceRefreshRequestEvent` and `ShowStartupRequestEvent` in `Events.h` for file tree refresh and startup screen transitions
- **File Tree — Auto-scroll**: `FileTreeCtrl::EnsureNodeVisible()` scrolls to keep the active file node visible in the sidebar
- **File Tree — Indent Guide Lines**: VS Code-style vertical guide lines at each indent level for nested folders
- **File Tree — Enriched Tooltips**: Hover tooltip showing full file path, file size (B/KB/MB), or child count for folders
- **File Tree — Context Menu Enhancements**: New File, Delete File/Folder (with confirmation dialog), Collapse All, Expand All actions via context menu
- **File Tree — 30+ File-Type Icons**: Extended icon matching for `.json`, `.yml`, `.yaml`, `.toml`, `.xml`, `.html`, `.css`, `.js`, `.ts`, `.jsx`, `.tsx`, `.sh`, `.py`, `.rb`, `.go`, `.rs`, `.c`, `.cpp`, `.h`, `.hpp`, `.java`, `.swift`, `.kt`, `.cfg`, `.ini`, `.env`, `.log`, `.csv`, `.sql`, and more
- **File Tree — Empty Folder Placeholder**: Open folders with no visible children display an "(empty)" placeholder
- **File Tree — EXPLORER Header**: Sidebar displays an "EXPLORER" label in the header section
- **File Tree — Footer File Count**: Sidebar footer shows total file count in workspace
- **Drag-and-Drop File Opening**: MainFrame accepts drag-and-drop files from Finder to open in the editor
- **Save All Command**: `LayoutManager::SaveAll()` saves all modified open files (Cmd+Shift+S / Ctrl+Shift+S)
- **Revert File Command**: `LayoutManager::RevertFile()` reloads active file from disk discarding changes (bound to File menu)
- **Close All Tabs Command**: `LayoutManager::CloseAllTabs()` closes every open tab (bound to File menu)
- **File → New**: Creates a new untitled tab and transitions from startup panel to editor
- **File → Open File**: Opens a file dialog and transitions from startup panel to editor
- **File → Save As**: Save the current document under a new name (Cmd+Shift+S)
- **Close Tab shortcut**: Cmd+W / Ctrl+W now closes the current tab
- **Status Bar — Filename**: Status bar shows the active file's basename
- **Status Bar — Language**: Status bar displays inferred language based on file extension
- **Status Bar — File Size**: Status bar shows file size in human-readable format
- **Unsaved Changes Prompt**: On application close, prompts the user if any unsaved files exist with save/discard/cancel options

### Changed

- Updated `FileTreeCtrl` with `EnsureNodeVisible()` method, indent guide rendering, expanded icon matching, empty-folder placeholder, tooltips, and enhanced context menu with 5 new actions
- Updated `LayoutManager` with Save All, Revert File, Close All Tabs, sidebar header/footer, workspace root forwarding, empty-state placeholder, and startup screen return on last tab close
- Updated `MainFrame` with File → New/Open/Save As/Close Tab/Close All menu items, drag-and-drop support, unsaved-changes prompt on close, workspace root propagation, and status bar enrichments
- Updated `TabBar` with improved tab close UX and focus handling
- Updated `StatusBarPanel` with filename, language, and file size segments
- Updated `SplitView` with improved sash handling

### Fixed

- **File tree selection**: Active file highlighting now works on both files and folders (was file-only)
- **Selected file text color**: Uses accent-primary tint instead of plain TextMain for better visibility
- **Normal file text color**: Uses TextMuted for non-selected, non-hovered items for reduced visual noise
- **Context menu click handler**: Refactored to use consistent 4-space indentation and added new menu item IDs
- **Editor focus after file open**: Editor panel now receives focus automatically after opening a file from the file tree
- **Editor focus after tab switch**: Editor panel receives focus automatically when switching tabs
- **Preview refresh on tab switch**: Content change event published on tab switch to refresh the preview panel
- **Status bar cursor position**: Cursor position updates correctly when switching between tabs
- **Window title update**: Title bar updates reliably on both tab switch and tab close events

---

## v1.4.7 — 2026-02-12

### Highlights

Settings panel, notification toasts, activity bar, plugin system, and 60+ VS Code-inspired editor improvements. Adds a fully-featured Settings panel with search/filter, collapsible groups, modified indicators, and export/import. Introduces a toast notification manager, a vertical activity bar rail, and a plugin architecture with manifest-driven contribution points. Implements four phases of editor enhancements covering multi-cursor, auto-closing brackets, folding, line operations, case transforms, smart select, and UX polish. Fixes theme subscription gaps across multiple UI components.

### Added

- **SettingsPanel**: `SettingsPanel.h/.cpp` — VS Code-inspired settings editor with searchable/filterable settings, collapsible category groups, per-setting modified indicator (●), reset-to-default buttons, and settings import/export
- **NotificationManager**: `NotificationManager.h/.cpp` — toast notification system with Info/Warning/Error/Success levels, auto-dismiss with configurable duration, animated fade-in/out, and up to 3 stacked toasts
- **ActivityBar**: `ActivityBar.h/.cpp` — vertical icon rail (File Explorer, Search, Settings, Themes) with painted icons, hover/active highlighting, and click-to-navigate via EventBus
- **Plugin System**: `IPlugin.h` — plugin interface with manifest-driven contribution points (commands, keybindings, snippets, menus, settings, themes), `PluginContext` runtime context, `PluginManager.h/.cpp` — lifecycle management (register, activate, deactivate) with palette/shortcut wiring
- **Events**: `SettingsOpenRequestEvent`, `SettingChangedEvent`, `PluginActivatedEvent`, `PluginDeactivatedEvent`, `NotificationEvent` (with `NotificationLevel` enum), `ActivityBarSelectionEvent` (with `ActivityBarItem` enum)
- **Editor Phase 6 — VS Code Improvements (18 items)**: auto-closing brackets/quotes, multi-cursor above/below, sticky scroll, inline color preview, font ligatures, smooth caret, current-line highlight, font family config, auto-save with delay, insert final newline, whitespace boundary rendering, link auto-complete, drag-and-drop file insertion, edge column ruler, go-to-symbol heading navigation, toggle block comment, smart select expand/shrink
- **Editor Phase 7 — UX/QoL (14 items)**: cursor surrounding lines, scroll beyond last line, smooth scrolling, copy-line-if-no-selection, join lines, reverse lines, delete duplicate lines, transpose characters, move selected text left/right, block indent/outdent, cursor undo/redo, select all occurrences, add selection to next find match, toggle word wrap
- **Editor Phase 8 — More VS Code (20 items)**: fold/unfold current/all regions, expand line selection, delete current line, toggle render whitespace, toggle line numbers, jump-to/select-to/remove-surrounding brackets, duplicate selection or line, transform to upper/lower/title case, sort lines ascending/descending, insert line above, trim trailing whitespace, toggle minimap visibility
- **Editor Phase 9 — Final VS Code Batch (14 items)**: copy line up/down, delete all left/right of cursor, add/remove line comment, toggle auto-indent/bracket-matching/code-folding/indentation-guides, select word at cursor, select current paragraph, toggle read-only mode, convert indentation spaces↔tabs
- **Editor auto-pair markdown emphasis** (`*`, `**`, `` ` ``) and smart backspace for pair deletion

### Changed

- Expanded `src/CMakeLists.txt` with `SettingsPanel.cpp`, `NotificationManager.cpp`, `ActivityBar.cpp`, `PluginManager.cpp` source files and source_group entries
- Expanded `src/core/Events.h` with 104 new lines of event types for settings, plugins, notifications, and activity bar
- Updated `EditorPanel` with 2,000+ lines of new editor improvement implementations and 33 new private state members
- Updated `CommandPalette`, `BreadcrumbBar` to accept `EventBus&` and subscribe to `ThemeChangedEvent`
- Updated `FloatingFormatBar`, `ImagePreviewPopover`, `LinkPreviewPopover`, `TableEditorOverlay` to accept `EventBus&` and subscribe to `ThemeChangedEvent`
- Updated `StartupPanel` to subscribe to `ThemeChangedEvent` for dynamic theme switching

### Fixed

- **Theme subscription gaps**: Added `ThemeChangedEvent` subscriptions to `NotificationManager`, `TableEditorOverlay`, `ImagePreviewPopover`, `LinkPreviewPopover`, `FloatingFormatBar`, `CommandPalette`, `BreadcrumbBar`, and `StartupPanel` ensuring all components respond to runtime theme changes
- **EditorPanel::OnThemeChanged**: Now calls `ThemeAwareWindow::OnThemeChanged(new_theme)` before applying editor-specific theme updates

---

## v1.3.6 — 2026-02-12

### Highlights

Multi-file tab management, file tree QoL improvements, and crash regression tests. Adds a horizontally scrollable TabBar component with themed rendering, close buttons, and modified-dot indicators. Introduces file tree context menus (open, reveal in Finder, copy path), keyboard navigation, multi-file buffer state management with auto-save and external change detection, and window title modification status display.

### Added

- **TabBar Component**: `TabBar.h/.cpp` — horizontally scrollable tab bar for multi-file editing with active indicator, modified dot (●), hoverable close (×) buttons, right-click context menu, tab cycling, and batch close operations (close all, close others, close to left/right)
- **File Tree Context Menu**: Right-click context menu with Open, Reveal in Finder/File Manager, Copy Path, Copy Relative Path actions
- **File Tree Keyboard Navigation**: Arrow key navigation (Up/Down to move, Enter/Space to open/toggle folders, Left/Right to collapse/expand)
- **Tab Events**: `TabSwitchedEvent`, `TabCloseRequestEvent`, `TabSaveRequestEvent`, `TabSaveAsRequestEvent`, `FileReloadRequestEvent`, `GoToLineRequestEvent` in `Events.h`
- **Multi-File Buffer Management**: `LayoutManager` multi-file state with per-file buffers tracking content, cursor position, scroll position, and external modification timestamps
- **Tab Keyboard Shortcuts**: Cmd+W (close tab), Ctrl+Tab / Ctrl+Shift+Tab (cycle tabs)
- **Window Title Tracking**: Dynamic window title showing filename and modification status (● prefix for unsaved changes)
- **Auto-Save Support**: `LayoutManager::StartAutoSave()` / `StopAutoSave()` for periodic auto-save
- **External Change Detection**: `LayoutManager::CheckExternalFileChanges()` for detecting and reloading externally modified files
- **Crash Regression Tests**: `test_crash_regressions.cpp` — regression tests for StartupPanel assertion failure and editor heading crash

### Changed

- Expanded `src/CMakeLists.txt` with `TabBar.cpp` source file
- Expanded `tests/CMakeLists.txt` with `test_crash_regressions` test target
- Enhanced `LayoutManager` with 460+ lines of multi-file tab management, file open/close/save/switch, auto-save timer, and external change detection
- Extended `MainFrame` with tab management methods (`onCloseActiveTab`, `onCycleTab`, `updateWindowTitle`) and 3 new keyboard shortcuts
- Added `FileOpenCallback`, `SetWorkspaceRoot`, and keyboard/context-menu event handlers to `FileTreeCtrl`

### Fixed

- **StartupPanel assertion**: Fixed `SetBackgroundStyle(wxBG_STYLE_TRANSPARENT)` being called before `Create()` in `StartupPanel.cpp`
- **PreviewPanel redundant preprocessing**: Removed duplicate `FootnotePreprocessor::process()` call from `PreviewPanel.cpp` that caused editor crash on heading input

---

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
