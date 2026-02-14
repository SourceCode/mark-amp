# MarkAmp Release History

## v2.1.13 — 2026-02-14

### Highlights

Phase 18–21 quality-and-feature sweep: LaTeX math rendering foundation (IMathRenderer + MathRenderer with ~120 Unicode symbol mappings), documentation and AGENTS.md sync, 4 hot-path performance optimizations (publish_fast migration, Config CachedValues, ThemeEngine color_fast, buffer pre-alloc), code quality hardening (typed exception handlers, Config.cpp cognitive complexity reduction, data-driven apply_defaults, 9 new Phase 20 test cases), plus 13 P1-P4 extension API services (SnippetEngine, WorkspaceService, TextEditorService, ProgressService, EnvironmentService, NotificationService, StatusBarItemService, InputBoxService, QuickPickService, ExtensionEvents, GrammarEngine stub, TerminalService stub, TaskRunnerService stub). 80 files changed, 28 test targets passing.

### Added

- **P1-P4 Extension Services**: SnippetEngine, WorkspaceService, TextEditorService, ProgressService, EnvironmentService, NotificationService, StatusBarItemService, InputBoxService, QuickPickService, ExtensionEvents, GrammarEngine (stub), TerminalService (stub), TaskRunnerService (stub) — all wired into PluginContext
- **Math Rendering Foundation (Phase 18)**: `IMathRenderer` interface and `MathRenderer` with ~120 LaTeX-to-Unicode symbol mapping, inline (`$...$`) and display (`$$...$$`) support via md4c `MD_FLAG_LATEXMATHSPANS`
- **Phase 20 Performance Tests**: 9 new test cases (62 assertions) covering `CachedValues`, `publish_fast()`, `color_fast()`, and `apply_defaults` refactor
- **test_p1_p4_services**: Comprehensive test suite for all 13 P1-P4 extension services

### Changed

- **Config.cpp Complexity Reduction (Phase 21)**: Refactored `apply_defaults()` from 28 chained if-blocks to data-driven `DefaultEntry` table with `std::variant` (complexity ~34→5); extracted `migrate_from_json()` and `parse_frontmatter()` helpers from `load()` (complexity ~36→12)
- **Hot-Path Performance (Phase 20)**: Migrated 5 high-frequency event publishes to `publish_fast()`, added `CachedValues` struct to Config for O(1) access, added `color_fast()` O(1) array-indexed lookup to ThemeEngine
- **Documentation (Phase 19)**: Updated AGENTS.md architecture diagram, README feature list, and HISTORY.md to reflect current state
- **TabBar**: Replaced `catch(...)` with `catch(const std::exception&)` for type safety
- **Config.cpp Lint Fixes**: Renamed short variables, added `const` correctness, added braces to single-line if-statements

### Fixed

- **Phase 14 Deferred Items**: Verified MarkdownDocument naming and duplicate steps in MarkAmpApp.cpp were already resolved
- **Config.cpp Lint Warnings**: Fixed short variable names, missing braces, and const correctness issues

---

## v2.0.0 — 2026-02-14

### Highlights

V2 architecture refactor: 18-phase overhaul delivering event system macro standardization, full extension service wiring (13 P1-P4 services), PluginContext population with 24 service fields, behavioral built-in plugin activation, feature guard integration, real wxWidgets panel rendering for 5 extension UI panels, InputBox/QuickPick modal UI, EventBus lock-free fast-path publishing, orphan code cleanup, CMake source_group standardization, 6 new test suites (27 total), naming convention standardization, config defaults externalization, platform stub completion (Win32/Linux), ServiceRegistry singleton elimination via constructor injection, and LaTeX math rendering via IMathRenderer with ~120-symbol Unicode mapping. Touches 100+ files with 10,000+ insertions.

### Added

- **Math Rendering**: `IMathRenderer` interface and `MathRenderer` implementation with ~120 LaTeX-to-Unicode symbol map, inline (`$...$`) and display (`$$...$$`) math support via md4c `MD_FLAG_LATEXMATHSPANS`, threaded through full constructor chain (MarkAmpApp → MainFrame → LayoutManager → SplitView → PreviewPanel → HtmlRenderer)
- **13 Extension Services (P1-P4)**: SnippetEngine, WorkspaceService, TextEditorService, ProgressService, EnvironmentService, NotificationService, StatusBarItemService, InputBoxService, QuickPickService, ExtensionEventBus, GrammarEngine (stub), TerminalService (stub), TaskRunnerService (stub) — all wired into `PluginContext` with 24 populated fields
- **InputBox & QuickPick UI**: `InputBoxDialog` and `QuickPickDialog` wxWidgets modals for extension-facing text input and selection lists
- **Platform Stubs**: `WinPlatform` (Win32 API) and `LinuxPlatform` (GTK/GDK) completing cross-platform abstraction layer
- **Feature Guards**: `FeatureGuardedPlugin` wrapper evaluating FeatureRegistry state before plugin activation
- **6 New Test Suites**: `test_service_wiring`, `test_eventbus_queuing`, `test_builtin_plugin_behavior`, `test_feature_guard_integration`, `test_panel_data_flow`, `test_extension_services` — bringing total to 27 targets
- **CLAUDE.md**: Claude Code-compatible project instructions with build, style, and architecture documentation

### Changed

- **Event System**: Standardized all event declarations to use `MARKAMP_DECLARE_EVENT` macros with field-based constructors, eliminating hand-rolled boilerplate
- **PluginContext**: Unified from original 6 fields to full 24-field VS Code-compatible context aggregating all service pointers (`PluginContext.h`)
- **Built-In Plugins**: 7 built-in features now behaviorally activate — register commands, contribute snippets, and subscribe to events on `activate(ctx)` rather than being inert wrappers
- **EventBus**: Added `publish_fast()` lock-free path for hot-path events, `queued_publish()` for cross-thread posting, and configurable queue drain
- **CMake**: Standardized `source_group()` entries for all 100+ source files, ensuring correct IDE project organization
- **Naming Conventions**: Standardized member variables (`m_` → `_` suffix), function names, and enum values across the codebase
- **Config Defaults**: Externalized 60+ default configuration values from scattered constructors into centralized `Config::set_defaults()`
- **ServiceRegistry**: Eliminated singleton pattern — all services now use constructor injection throughout the dependency chain
- **Extension Panels**: `OutputPanel`, `ProblemsPanel`, `TreeViewHost`, `WalkthroughPanel`, `WebviewHostPanel` upgraded from data-only stubs to real wxWidgets panels with theme-aware rendering
- **Documentation**: Updated AGENTS.md, README.md, HISTORY.md, and created CLAUDE.md to reflect the full v2 architecture

### Fixed

- **Orphan Code**: Removed 800+ lines of unused/dead code across 15 files
- **Event Type Deduplication**: Eliminated 12 duplicate event type definitions
- **Platform Detection**: Fixed `PlatformAbstraction` to correctly detect and dispatch to Mac/Win/Linux implementations

---

## v1.9.12 — 2026-02-14

### Highlights

### Added

- **BuiltInPlugins** (`BuiltInPlugins.h/.cpp`): Converts 7 built-in features (Mermaid, Table Editor, Format Bar, Theme Gallery, Link Preview, Image Preview, Breadcrumb) into IPlugin instances registered with PluginManager
- **FeatureRegistry** (`FeatureRegistry.h/.cpp`): Runtime feature toggle registry with enable/disable/query and `FeatureToggledEvent` notifications
- **ExtensionManifest** (`ExtensionManifest.h/.cpp`): JSON manifest parser for VS Code-compatible `package.json` extension manifests including activation events, dependencies, contribution points, and extension packs
- **ExtensionScanner** (`ExtensionScanner.h/.cpp`): Scans extension directories for installed extensions and parses their manifests
- **ExtensionStorage** (`ExtensionStorage.h/.cpp`): Persistent per-extension key-value storage with JSON serialization
- **ExtensionEnablement** (`ExtensionEnablement.h/.cpp`): Extension enable/disable state management with global and workspace-scoped overrides
- **VsixService** (`VsixService.h/.cpp`): VSIX package install/uninstall with ZIP extraction via libzip
- **HttpClient** (`HttpClient.h/.cpp`): HTTP GET/POST client using cpp-httplib for marketplace communication
- **GalleryService** (`GalleryService.h/.cpp`): Extension marketplace gallery search, version listing, and download URL resolution
- **ExtensionManagement** (`ExtensionManagement.h/.cpp`): High-level extension lifecycle orchestrating scanner, gallery, VSIX, enablement, and PluginManager
- **ContextKeyService** (`ContextKeyService.h/.cpp`): VS Code-style context key registry for conditional command/menu visibility
- **WhenClause** (`WhenClause.h/.cpp`): Boolean expression evaluator for `when` clauses (AND, OR, NOT, ==, !=, regex match)
- **OutputChannelService** (`OutputChannelService.h/.cpp`): Named output channel management for extension logging
- **DiagnosticsService** (`DiagnosticsService.h/.cpp`): Diagnostic collection and reporting (errors, warnings, hints, info) per URI
- **TreeDataProviderRegistry** (`TreeDataProviderRegistry.h/.cpp`): Registry for custom tree view data providers
- **WebviewService** (`WebviewService.h/.cpp`): Webview panel creation and lifecycle management
- **DecorationService** (`DecorationService.h/.cpp`): Text editor decoration provider registry
- **FileSystemProviderRegistry** (`FileSystemProviderRegistry.h/.cpp`): Virtual file system provider registry
- **LanguageProviderRegistry** (`LanguageProviderRegistry.h/.cpp`): Language feature provider registry (completion, hover, definition, diagnostics)
- **ExtensionHostRecovery** (`ExtensionHostRecovery.h/.cpp`): Extension host crash detection and automatic restart
- **ExtensionRecommendations** (`ExtensionRecommendations.h/.cpp`): File-type-based extension recommendation engine
- **ExtensionTelemetry** (`ExtensionTelemetry.h/.cpp`): Extension activation/deactivation telemetry event tracking
- **ExtensionSandbox** (`ExtensionSandbox.h/.cpp`): Extension isolation with permission grants and resource limits
- **PluginContext** (`IPlugin.h`): Extended with `extension_path`, `workspace_state`, and `global_state` fields matching VS Code's `vscode.ExtensionContext`
- **ExtensionsBrowserPanel** (`ExtensionsBrowserPanel.h/.cpp`): Sidebar panel for browsing, searching, installing, and managing extensions
- **ExtensionCard** (`ExtensionCard.h/.cpp`): Painted card widget for extension list items with icon, name, description, and install button
- **ExtensionDetailPanel** (`ExtensionDetailPanel.h/.cpp`): Full-page extension detail view with description, version, dependencies, and changelog
- **OutputPanel** (`OutputPanel.h/.cpp`): Bottom panel for extension output channel display
- **ProblemsPanel** (`ProblemsPanel.h/.cpp`): Bottom panel for diagnostics display (errors, warnings)
- **TreeViewHost** (`TreeViewHost.h/.cpp`): Host panel for custom tree views contributed by extensions
- **WalkthroughPanel** (`WalkthroughPanel.h/.cpp`): Getting-started walkthrough panel for extensions
- **WebviewHostPanel** (`WebviewHostPanel.h/.cpp`): Host panel for webview content contributed by extensions
- **Events**: `FeatureToggledEvent`, `ExtensionInstalledEvent`, `ExtensionUninstalledEvent`, `ExtensionEnablementChangedEvent`, `ShowExtensionsBrowserRequestEvent`, `ShowExplorerRequestEvent`
- **ActivityBar — Extensions Item**: New `Extensions` entry in `ActivityBarItem` enum for sidebar navigation to Extensions Browser
- **11 New Test Suites**: `test_feature_registry`, `test_extension_manifest`, `test_extension_scanner`, `test_plugin_manager_v2`, `test_vsix_service`, `test_gallery_service`, `test_extension_management`, `test_builtin_plugins`, `test_context_keys`, `test_extension_services`, `test_extension_integration`

### Changed

- **PluginManager**: Enhanced with lazy activation via activation events (plugins only activate when their trigger fires), topological dependency resolution, extension pack expansion, `trigger_activation_event()` method, and manifest-aware `register_plugin()` overload — 452+ lines of new logic
- **LayoutManager**: Extended with Extensions Browser panel integration, updated sidebar panel switching to handle `ShowExtensionsBrowserRequestEvent` and `ShowExplorerRequestEvent`, 153+ new lines
- **MainFrame**: Updated to initialize BuiltInPlugins and wire extension-related events
- **MarkAmpApp**: Added BuiltInPlugins registration during application startup
- **SettingsPanel**: Extended with extension-related settings categories and registration
- **ActivityBar**: Added Extensions item to the icon rail
- **PreviewPanel**: Minor refinements to theme-aware rendering
- **src/CMakeLists.txt**: Added 30 new source files for all extension infrastructure and UI panels
- **tests/CMakeLists.txt**: Added 11 new test targets with proper linking for nlohmann_json, libzip, and cpp-httplib
- **vcpkg.json**: Added `libzip`, `openssl`, and `cpp-httplib` dependencies

### Fixed

- All changes are backwards-compatible with existing plugins, configuration files, and themes

---

## v1.8.11 — 2026-02-13

### Highlights

R20–R22 UI/UX polish and VS Code-equivalent settings: 40 new editor and syntax highlighting settings, command palette fuzzy-match highlighting and category badges, breadcrumb chevron styling with heading glyphs, activity bar press feedback and drag handles, file tree hover highlighting and icon tinting, preview panel scroll-to-top button and print-friendly CSS, settings panel category emojis and accent underlines, toolbar zoom slider and format dropdown, tab bar pin indicators and drag shadows, and window chrome long-filename truncation with radial button glow. Touches 32 source files with 2,125 insertions across core and UI layers. All 10 test targets pass at 100%.

### Added

- **40 VS Code-Equivalent Settings (R22)**: 15 editor behavior settings (cursor blinking, cursor width, mouse wheel zoom, render control characters, rounded selection, select on line numbers, match brackets mode, render line highlight, word wrap column, and more) plus 15 editor appearance settings (minimap side/scale/max column, overview ruler, letter spacing, line height, fold highlighting, cursor style, cursor surrounding lines style, smooth scrolling) plus 10 syntax highlighting settings (strings, numbers, operators, types, functions, constants, preprocessor, comments, keywords, decorators) — all registered in SettingsPanel and integrated with Scintilla
- **Config Defaults (R22)**: 41 new default configuration keys in `Config.cpp` covering all R22 settings
- **Events (R22)**: `SettingsBatchChangedEvent` for bulk settings reloads
- **PreviewPanel — Scroll-to-Top Button (R21 Fix 32)**: Floating scroll-to-top button appears when scrolled past threshold, repositioned on resize
- **PreviewPanel — Print-Friendly CSS (R21 Fix 34)**: `@media print` CSS block for clean printed output
- **ImagePreviewPopover — Dimensions & Size Labels (R21 Fix 17–18)**: Image dimensions and file size displayed in popover
- **LinkPreviewPopover — Copy URL Button & Globe Prefix (R21 Fix 19–20)**: Globe emoji prefix before URL and a copy-to-clipboard button

### Changed

- **CommandPalette**: Category badge prefix `[Category]`, fuzzy match character highlighting with `«»` brackets, right-aligned shortcut display, selected item `▸` accent prefix, styled no-results placeholder (R21 Fixes 1–5)
- **BreadcrumbBar**: Styled `▸` chevron separators, `§` glyph prefix before heading segments, hand cursor on hover (R21 Fixes 6–8)
- **ActivityBar**: Press offset feedback (1px shift), double-click-to-collapse sidebar, drag handle dots, themed tooltip pill, separator above bottom item (R20 Fixes 16–20)
- **CustomChrome**: Long filename truncation with gradient text-fade, modified dot prefix, radial glow behind hovered window buttons (R20 Fixes 29–31)
- **EditorPanel**: Accent-tinted line numbers, R22 settings integration with Scintilla (cursor style, folding, bracket matching, indentation guides, word wrap column, letter spacing, line height, minimap, smooth scrolling)
- **FileTreeCtrl**: Full-width hover row highlight, file icon color tint by extension, bold matched filter characters (R20 Fixes 22–24)
- **FloatingFormatBar**: Hover background highlight, pressed accent flash (R21 Fixes 9, 12)
- **NotificationManager**: Slide-in offset animation, close button on toasts (R20 Fixes 35–36)
- **SettingsPanel**: Category emoji icon prefixes, modified indicator dots, accent underline below category headers, fixed column positions for readability, key badge pill for shortcuts (R21 Fixes 13–16, 29–30; R22 settings registration)
- **ShortcutOverlay**: Enhanced rendering with improved visual layout
- **SplitView**: Additional bounds and state guards
- **StartupPanel**: Improved recent-file rendering
- **StatusBarPanel**: Minor refinements to segment updates
- **TabBar**: Pin tab indicator, drag shadow improvements, enhanced close/modify indicators (R21 Fixes 37–38)
- **TableEditorOverlay**: Border depth effect (R21 Fix 11)
- **ThemeGallery**: Improved card rendering
- **ThemePreviewCard**: Enhanced preview styling
- **ThemedScrollbar**: Additional visual polish
- **Toolbar**: Zoom slider, format dropdown, accent-hover effects (R21 Fixes 25–28)

### Fixed

- All changes are backwards-compatible with existing configuration files and themes

---

## v1.7.10 — 2026-02-13

### Highlights

Comprehensive stability hardening across the entire codebase: 40+ fixes spanning input validation, thread safety, typed exception handling, arithmetic underflow guards, filesystem resilience, and UI robustness. Touches 56 files with 1,613 insertions and 391 deletions. All 10 test targets pass at 100%.

### Added

- **`[[nodiscard]]`** on `Config::load()` and `Config::save()` return values to prevent silently discarding errors
- **GUARDED_BY annotation** on `DocumentSnapshot::current_` to document mutex-protected access
- **Design note** on `AsyncHighlighter::tokenize_range()` documenting cancellation latency trade-off

### Changed

- **Config typed catches**: Replaced bare `catch (...)` with `catch (const YAML::Exception&)` in `get_int()`, `get_bool()`, `get_double()` (R20 Fixes 1–3)
- **Config filesystem resilience**: `config_directory()` and `load()` now use `std::error_code` overloads for `exists()` and guard against `current_path()` throwing (R20 Fixes 7, 10)
- **ThemeRegistry**: `initialize()` result checked with descriptive warning on failure; `exists()` uses error_code overload (R20 Fixes 8, 9)
- **PluginManager**: Plugin `activate()` and `deactivate()` wrapped in exception guards to isolate misbehaving plugins (R19 Fixes 11, 12)
- **EventBus**: `publish()` wraps each subscriber callback in try-catch to prevent one bad handler from breaking the chain (R19 Fix 13)
- **RecentFiles / RecentWorkspaces**: `weakly_canonical()` calls wrapped in try-catch to handle deleted or inaccessible paths; key/value length capped to prevent unbounded memory (R19 Fixes 14–15; R20 Fixes 20, 32)
- **FileSystem**: `exists()` and `current_path()` calls use error_code overloads; `temp_directory_path()` guarded (R20 Fixes 5, 6, 13)
- **AsyncFileLoader**: Distinguishes EOF from read failure by checking `file.bad()` (R20 Fix 21)
- **AsyncHighlighter**: Guards `set_content()` against use-after-stop (R20 Fix 19)
- **ChunkedStorage**: Guards against underflow when `chunk_offset >= chunk_used_` (R19 Fix 25)
- **LRUCache**: Guards against `current_bytes_` underflow in eviction (R19 Fix 26)
- **FrameArena**: Added noexcept documentation on `monotonic_buffer_resource` ctor (R20 Fix 29)
- **FrameScheduler**: Uses copy-then-pop instead of `const_cast` for priority queue extraction (R20 Fix 15); clamps negative durations (R20 Fix 16)
- **Profiler**: Caps per-metric history to prevent unbounded memory growth (R20 Fixes 17, 37); wraps `std::stol` for `/proc/self/status` parsing (R20 Fix 12)
- **SPSCQueue**: Added size alignment compile-time assertion (R19 Fix 39)
- **ServiceRegistry**: Improved type-safe service resolution
- **MermaidRenderer**: Caps `block_sources_` to prevent unbounded growth; caps error-output iterations (R20 Fixes 33, 34)
- **HtmlSanitizer**: Additional input validation guards
- **Logger**: Improved robustness of log output
- **ShortcutManager**: Minor resilience improvement
- **Theme**: Color field parsing wrapped in try-catch for descriptive errors (R20 Fix 36)
- **CodeBlockRenderer**: Typed catch for malformed default values (R20 Fix 4)
- **HtmlRenderer**: Improved bounds checking and input validation; capped block source tracking (R20 Fix 14)
- **EditorPanel**: Comprehensive null-check and bounds-guard improvements across 500+ lines of editor actions; safer selection handling, undo/redo guards, and regex error handling
- **PreviewPanel**: Input validation and bounds checking for preview rendering pipeline
- **SettingsPanel**: Exception-safe settings rendering
- **TabBar**: Bounds-guarded tab indexing and safer drag-drop operations
- **StatusBarPanel**: Null-check guards on segment updates
- **StartupPanel**: Safer recent-file rendering with path validation
- **NotificationManager**: Thread-safe dismiss and bounds-guarded toast stacking
- **CommandPalette**: Input validation on fuzzy search
- **BreadcrumbBar**: Safer path segment parsing
- **CustomChrome**: Bounds checking on window metrics
- **SplitView**: Sash position clamping
- **ShortcutOverlay**: Improved rendering bounds and visibility guards
- **ActivityBar**: Bounds-guarded icon hit-testing and hover state
- **ThemeAwareWindow**: Safe theme subscription lifecycle management
- **ThemeGallery**: Bounds-checked theme card rendering
- **Toolbar**: Safe button state updates with null-check guards

### Fixed

- **Potential crashes** from unchecked `std::filesystem::exists()`, `current_path()`, `temp_directory_path()`, and `weakly_canonical()` calls that could throw on permission errors or deleted directories
- **Arithmetic underflow** in `ChunkedStorage::read()` and `LRUCache::evict_oldest()` that could produce incorrect results on edge-case inputs
- **Unbounded memory growth** in `Profiler` metric history, `MermaidRenderer` block sources, and `RecentFiles` key/value parsing
- **Silent error swallowing** from bare `catch (...)` blocks — now catches specific exception types for better diagnostics
- **EventBus subscriber isolation** — a throwing subscriber no longer prevents subsequent handlers from executing
- **Plugin crash propagation** — exceptions in plugin activate/deactivate no longer crash the host application

---

## v1.6.9 — 2026-02-13

### Highlights

R3–R17 UX polish: 80+ new editor action events, command-line file opening, full Edit and View menu build-out, editor right-click context menu, file tree type-ahead search with folder sort and collapse/expand-all, enriched status bar segments (EOL mode, encoding cycling, indent mode, zoom level, "SAVED" flash), clickable breadcrumb bar, Find/Replace integration, print support, sticky scroll headings, default Markdown snippets, recent files management, 20+ new configuration defaults, and persistent modified-state tracking.

### Added

- **80+ Editor Action Events**: `FindRequestEvent`, `ReplaceRequestEvent`, `DuplicateLineRequestEvent`, `ToggleCommentRequestEvent`, `DeleteLineRequestEvent`, `WrapToggleRequestEvent`, `MoveLineUp/DownRequestEvent`, `JoinLinesRequestEvent`, `SortLinesAsc/DescRequestEvent`, `TransformUpper/Lower/TitleRequestEvent`, `SelectAllOccurrencesRequestEvent`, `ExpandLineSelectionRequestEvent`, `InsertLineAbove/BelowRequestEvent`, `FoldAll/UnfoldAllRequestEvent`, `ToggleLineNumbers/WhitespaceRequestEvent`, `CopyLineUp/DownRequestEvent`, `DeleteAllLeft/RightRequestEvent`, `ReverseLinesRequestEvent`, `DeleteDuplicateLinesRequestEvent`, `TransposeCharsRequestEvent`, `IndentSelection/OutdentSelectionRequestEvent`, `SelectWord/ParagraphRequestEvent`, `ToggleReadOnlyRequestEvent`, `ConvertIndentSpaces/TabsRequestEvent`, `JumpToBracket/SelectToBracketRequestEvent`, `ToggleMinimapRequestEvent`, `FoldCurrent/UnfoldCurrentRequestEvent`, `AddLineComment/RemoveLineCommentRequestEvent`, `TrimTrailingWSRequestEvent`, `ExpandSelection/ShrinkSelectionRequestEvent`, `CursorUndo/RedoRequestEvent`, `MoveTextLeft/RightRequestEvent`, `ToggleAutoIndent/BracketMatching/CodeFolding/IndentGuidesRequestEvent`, `PrintRequestEvent`, and more
- **Command-Line File Opening**: `MainFrame` opens a file passed as command-line argument directly into the editor (bypasses startup screen)
- **Edit Menu — 50+ Actions**: Find, Replace, Duplicate Line, Toggle Comment, Delete Line, Move Line Up/Down, Join Lines, Sort Lines Asc/Desc, Transform Upper/Lower/Title, Select All Occurrences, Expand Line Selection, Insert Line Above/Below, Copy Line Up/Down, Delete All Left/Right, Reverse Lines, Delete Duplicate Lines, Transpose Chars, Indent/Outdent, Select Word/Paragraph, Toggle Read-Only, Convert Indent Spaces/Tabs, Trim Trailing Whitespace, Expand/Shrink Selection, Cursor Undo/Redo, Move Text Left/Right, Add/Remove Line Comment
- **View Menu — 15+ Toggles**: Word Wrap, Line Numbers, Whitespace, Fold All, Unfold All, Fold/Unfold Current, Minimap, Auto-Indent, Bracket Matching, Code Folding, Indent Guides, Jump/Select-to Bracket, Welcome Screen
- **File Menu — Close Folder & Go-To-Line**: Close Folder and Go-To-Line dialog menu entries
- **File Menu — Print**: Print current document via `wxHtmlEasyPrinting`
- **File Menu — Recent Files Submenu**: Tracks last 10 opened files with Clear Recent Files option
- **Editor — Right-Click Context Menu**: Cut, Copy, Paste, Select All, and all Edit menu actions accessible via context menu
- **File Tree — Type-Ahead Search**: Keyboard type-ahead filtering with 500ms buffer timer to locate files by name
- **File Tree — Folder Sort**: Recursive folders-before-files sorting with case-insensitive alphabetical ordering
- **File Tree — Collapse/Expand All**: `CollapseAllNodes()` and `ExpandAllNodes()` actions for bulk tree operations
- **File Tree — Auto-Expand Ancestors**: Opening a nested file auto-expands parent folders to reveal the node
- **Status Bar — EOL Mode**: Displays LF/CRLF/CR with click-to-cycle behavior
- **Status Bar — Encoding Cycling**: Click encoding segment to cycle through UTF-8 / ASCII / ISO-8859-1
- **Status Bar — Indent Mode**: Displays "Spaces: N" or "Tabs: N" indent configuration
- **Status Bar — Zoom Level**: Displays current zoom percentage
- **Status Bar — "SAVED" Flash**: Briefly flashes "SAVED ✓" on save event with 800ms auto-dismiss timer
- **Status Bar — Clickable Cursor Position**: Click "LN X, COL Y" to trigger Go-To-Line dialog
- **BreadcrumbBar — Clickable Segments**: Path segments are clickable with configurable callback; filename segment rendered in semi-bold
- **Editor — Sticky Scroll Heading**: Scans upward from first visible line to find nearest Markdown heading for pinned display
- **Editor — Default Markdown Snippets**: 8 registered snippets — Bold, Italic, Link, Image, Code Block, Heading 1, Bullet List, Table
- **Editor — Tab Indents**: `editor_->SetTabIndents(true)` for proper tab behavior
- **Editor — Caret Visibility**: `EnsureCaretVisible()` called after `SetContent()` to guarantee cursor is on screen
- **Config — 20+ New Defaults**: `show_line_numbers`, `highlight_current_line`, `show_whitespace`, `tab_size`, `show_minimap`, `last_workspace`, `last_open_files`, `auto_indent`, `indent_guides`, `bracket_matching`, `code_folding`, `edge_column`, `font_family`, `auto_save_interval_seconds`, `show_status_bar`, `show_tab_bar`
- **AppState — Modified Tracking**: `AppState::modified_` field and `AppStateManager::set_modified()` for persistent dirty-state tracking

### Changed

- Updated `MainFrame` with 50+ new menu items across Edit and View menus, command-line file argument handling, print support, recent files submenu, and Close Folder action
- Updated `LayoutManager` with 1,500+ lines of new event subscriptions for all R6–R8 editor actions (Find/Replace, duplicate line, toggle comment, delete line, word wrap, move/join/sort lines, transform case, select all occurrences, fold/unfold, copy line, reverse lines, indent/outdent, bracket navigation, and more)
- Updated `EditorPanel` with right-click context menu, sticky scroll heading implementation, 8 default snippets, tab indent support, improved selection alpha (80 opacity), and caret visibility on content load
- Updated `FileTreeCtrl` with type-ahead search, recursive folder-first sorting, collapse/expand all, and auto-expand ancestors on file open
- Updated `StatusBarPanel` with EOL mode, indent mode, zoom level, "SAVED" flash timer, clickable cursor position (Go-To-Line), clickable encoding cycling, and tooltip text for all segments
- Updated `BreadcrumbBar` with clickable segment callback, semi-bold filename rendering, and accent-tinted separator color
- Updated `Toolbar` with enhanced button layout and additional action entries
- Updated `TabBar` with improved tab management methods
- Updated `Events.h` with 945 lines of new event type definitions across R6–R8 phases
- Updated `Config.cpp` with 20+ new default configuration keys
- Updated `AppState` with `modified_` field for dirty-state tracking
- Updated `ActivityBar`, `CommandPalette`, `NotificationManager`, `StartupPanel`, and `SplitView` with minor refinements

### Fixed

- **Selection opacity**: Increased editor selection alpha from 60 to 80 for improved readability
- **Preview panel**: Removed redundant preprocessing call
- **AppState::is_modified()**: Now returns actual `modified_` state instead of hardcoded `false`

---

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
