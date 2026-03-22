# Agent Context for MarkAmp

> **Technical specification optimized for AI agent parsing and execution.**
> Hierarchical structure with consistent terminology, input/output contracts, and operational constraints.

---

## 1 · Project Identity

| Field           | Value                                                                                                                      |
| --------------- | -------------------------------------------------------------------------------------------------------------------------- |
| Name            | **MarkAmp**                                                                                                                |
| Version         | **2.29.79**                                                                                                                |
| Language        | C++23 (C++26-ready patterns)                                                                                               |
| UI Toolkit      | wxWidgets 3.2.9                                                                                                            |
| Build System    | CMake 3.30+ with presets                                                                                                   |
| Package Manager | vcpkg (manifest mode)                                                                                                      |
| Test Framework  | Catch2                                                                                                                     |
| Platforms       | macOS (primary), Windows, Linux                                                                                            |
| Description     | Cross-platform, retro-futuristic Markdown editor with canvas, AI, collaboration, plugin system, and 50-phase feature scope |

---

## 2 · Build & Run Commands

### 2.1 Configuration

```bash
# Debug build (default for development)
cmake --preset debug

# Release build
cmake --preset release

# Release with static linking
cmake --preset release-static
```

### 2.2 Build

```bash
# Full build (all targets including tests)
cmake --build build/debug -j$(sysctl -n hw.ncpu)

# Build only the main binary
cmake --build build/debug --target markamp -j$(sysctl -n hw.ncpu)

# Reconfigure after CMakeLists changes
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j$(sysctl -n hw.ncpu)
```

### 2.3 Testing

```bash
# Run all tests
cd build/debug && ctest --output-on-failure

# Run a specific test target
cd build/debug && ctest -R test_phase12_canvas_advanced --output-on-failure

# List all test targets
cd build/debug && ctest -N
```

**Test Stats:** 800 test targets, 797 test source files in `tests/unit/`.

### 2.4 Release Workflow

```bash
# Version bump → commit → tag → push
# Run the cpp-git-commit-version-push workflow
```

Version is managed in two files:

- `CMakeLists.txt` → `project(MarkAmp VERSION X.Y.Z LANGUAGES CXX)`
- `vcpkg.json` → `"version-semver": "X.Y.Z"`

### 2.5 Linting & Formatting

```bash
# Format all source files
scripts/format.sh

# Run linter
scripts/lint.sh
```

Configuration files: `.clang-format`, `.clang-tidy` (project root).

### 2.6 Scripts Reference

| Script                                 | Purpose                     |
| -------------------------------------- | --------------------------- |
| `scripts/build_debug.sh`               | Debug build                 |
| `scripts/build_release.sh`             | Release build               |
| `scripts/build_packages.sh`            | Build distribution packages |
| `scripts/run_tests.sh`                 | Run all tests               |
| `scripts/run_fuzz.sh`                  | Run fuzz testing harness    |
| `scripts/run_chaos_tests.sh`           | Run chaos/resilience tests  |
| `scripts/run_load_tests.sh`            | Run load testing framework  |
| `scripts/bump_version.sh`              | Bump version number         |
| `scripts/format.sh`                    | clang-format all sources    |
| `scripts/lint.sh`                      | clang-tidy all sources      |
| `scripts/clean.sh`                     | Clean build artifacts       |
| `scripts/smoke_test.sh`                | Quick smoke test            |
| `scripts/dead_code_report.sh`          | Detect unused code          |
| `scripts/audit_static_init.sh`         | Audit static initializers   |
| `scripts/check_catch_all.sh`           | Verify test coverage        |
| `scripts/benchmark_compare.py`         | Compare benchmark results   |
| `scripts/benchmark_update_baseline.sh` | Update benchmark baselines  |
| `scripts/generate_icons.sh`            | Generate icon assets        |
| `scripts/package_macos.sh`             | Package for macOS           |
| `scripts/package_macos_appstore.sh`    | Package for Mac App Store   |
| `scripts/package_linux.sh`             | Package for Linux           |
| `scripts/package_windows.ps1`          | Package for Windows         |
| `scripts/package_windows_msix.ps1`     | Package as Windows MSIX     |

---

## 3 · Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                      src/app/ (Entry Point)                     │
│                    MarkAmpApp → MainFrame                       │
├─────────────┬──────────────┬────────────┬───────────────────────┤
│  src/ui/    │ src/canvas/  │ src/render │    src/platform/      │
│  88 headers │ 98 headers   │ 36 headers │  4 platform impls     │
├─────────────┴──────────────┴────────────┴───────────────────────┤
│                    src/core/ (512 headers)                       │
│  EventBus · Config · Theme · Plugin · Extension · AI · Export   │
│  Canvas · Search · Graph · Notebook · Tasks · Cloud · Security  │
│  AV Database · FSRS · Presentation · WYSIWYG · Localization     │
└─────────────────────────────────────────────────────────────────┘
```

### 3.1 Directory Structure

| Directory                 | Contents                                                       | Count                  |
| ------------------------- | -------------------------------------------------------------- | ---------------------- |
| `src/app/`                | Application entry point (`MarkAmpApp.h/.cpp`)                  | 2 files                |
| `src/core/`               | Core business logic, services, engines                         | 512 headers, 383 cpp   |
| `src/core/av/`            | Attribute View / Database subsystem                            | 16 headers             |
| `src/core/fsrs/`          | Flashcard & spaced repetition (FSRS algorithm)                 | 9 headers              |
| `src/core/loader/`        | Theme loading (`ThemeLoader`)                                  | 1 header               |
| `src/canvas/`             | Infinite canvas system (boards, objects, tools, collaboration) | 98 headers             |
| `src/ui/`                 | wxWidgets UI components and panels                             | 88 headers             |
| `src/ui/av/`              | Attribute View UI (table, gallery, kanban, filter)             | 14 headers             |
| `src/rendering/`          | Rendering pipeline, FX engine, block renderers                 | 36 headers             |
| `src/platform/`           | Platform abstraction (Mac/Win/Linux)                           | 4 headers              |
| `tests/unit/`             | Catch2 unit tests                                              | 613 files, 634 targets |
| `benchmarks/`             | Performance benchmarks                                         | 25 files               |
| `fuzz/`                   | Fuzz testing harnesses                                         | 6 files                |
| `scripts/`                | Build, test, packaging automation                              | 23 scripts             |
| `themes/`                 | Markdown-based theme definitions (YAML frontmatter)            | 64 files               |
| `resources/icons/lucide/` | Lucide SVG icons for file tree                                 | SVG icons              |
| `docs/`                   | Architecture, API reference, user guides                       | 539 files              |

---

## 4 · Core Systems

### 4.1 Event System (`EventBus`, `Events.h`)

**Purpose:** Decoupled pub/sub communication backbone. ~1200+ event type declarations.

**Key APIs:**

- `EventBus::subscribe<EventType>(callback)` → `Subscription` (RAII-managed)
- `EventBus::publish(event)` — Standard publish (mutex-locked)
- `EventBus::publish_fast(event)` — Lock-free hot-path publish
- `EventBus::queued_publish(event)` — Cross-thread deferred publish

**Event Categories:**

- Settings: `SettingsOpenRequestEvent`, `SettingChangedEvent`, `SettingsBatchChangedEvent`
- Plugin: `PluginActivatedEvent`, `PluginDeactivatedEvent`
- Notification: `NotificationEvent` (with `NotificationLevel`)
- Activity bar: `ActivityBarSelectionEvent` (with `ActivityBarItem`)
- Theme: `ThemeChangedEvent`
- Tab: `TabSwitchedEvent`, `TabCloseRequestEvent`, `TabSaveRequestEvent`
- Workspace: `WorkspaceRefreshRequestEvent`, `ShowStartupRequestEvent`
- Feature: `FeatureToggledEvent`
- Extension: `ExtensionInstalledEvent`, `ExtensionUninstalledEvent`, `ExtensionEnablementChangedEvent`
- Editor actions (80+): Find, Replace, DuplicateLine, ToggleComment, DeleteLine, MoveLineUp/Down, SortLines, TransformCase, FoldAll/UnfoldAll, CopyLine, Print, and many more
- Canvas events, AI events, Export events, Search events, Graph events, Cloud sync events

**Constraints:**

- Each subscriber callback is wrapped in try-catch to prevent cascade failures
- `Subscription` destructor auto-unsubscribes — DO NOT hold raw callback pointers

---

### 4.2 Configuration (`Config`, `ScopedConfig`)

**Purpose:** YAML-based configuration with hierarchical scoping and caching.

**Key APIs:**

- `Config::load(path)` → `bool` (`[[nodiscard]]`)
- `Config::save(path)` → `bool` (`[[nodiscard]]`)
- `Config::get_string(key)`, `get_int(key)`, `get_bool(key)`, `get_double(key)`
- `Config::set(key, value)`
- `CachedValues` struct for O(1) hot-path access
- `ScopedConfig` — Workspace-scoped overrides with fallback to global

**File Format:** Markdown with YAML frontmatter (parsed via `yaml-cpp`)

**Constraints:**

- 60+ default configuration keys registered in `Config::set_defaults()`
- `ConfigProfile` — Named configuration profiles
- `ConfigMigration` — Schema migration between versions
- `ConfigAuditTrail` — Tracks configuration changes
- `ConfigFileWatcher` — Watches config file for external changes

---

### 4.3 Theme System

**Purpose:** 8 built-in themes + extensible theme loading/creation ecosystem.

**Components:**
| Component | Purpose |
|-----------|---------|
| `Theme` | Color tokens, FX settings, font settings |
| `ThemeEngine` | Runtime theme management with `color()` / `color_fast()` APIs |
| `ThemeRegistry` | Theme discovery, registration, and listing |
| `ThemeLoader` | Loads themes from Markdown YAML frontmatter |
| `BuiltInThemes` | 8 built-in color themes |
| `VsCodeThemeAdapter` | Imports VS Code color themes |
| `ThemeValidator` | Validates theme definitions |
| `ThemeExporter` | Exports themes to shareable format |
| `ThemeAutoRepair` | Fixes missing/invalid theme tokens |
| `ThemeCompatibility` | Cross-version theme compatibility |
| `ThemeOverrideService` | Per-workspace theme overrides |
| `ThemeScopeMapper` | Maps TextMate scopes to theme tokens |
| `ThemePerformanceMonitor` | Tracks theme rendering performance |
| `ThemeMarketplace` | Theme marketplace integration |

**Color Access:**

- `theme_engine().color(ThemeColorToken::EditorBg)` — Standard lookup
- `theme_engine().color_fast(index)` — O(1) array-indexed lookup (hot path)

---

### 4.4 Plugin Architecture

**Purpose:** VS Code-inspired plugin system with lazy activation, dependency resolution, and sandboxing.

**Core Interfaces:**
| Component | Purpose |
|-----------|---------|
| `IPlugin` | Abstract interface: `manifest()`, `activate(ctx)`, `deactivate()` |
| `PluginManifest` | Declares contribution points: commands, keybindings, snippets, menus, settings, themes |
| `PluginContext` | Runtime context with 24 fields (EventBus, Config, FeatureRegistry, all service pointers) |
| `PluginManager` | Lifecycle: register → activate → deactivate, with lazy activation events, topological sort |
| `BuiltInPlugins` | Converts 15 built-in features into `IPlugin` instances |

**15 Built-in Plugins:**
Mermaid, Table Editor, Format Bar, Theme Gallery, Link Preview, Image Preview, Breadcrumb, FX Engine, Text FX Renderer, VSCode Theme Adapter, Canvas Collaboration, Canvas Apps & Widgets, Kanban Board, Mind Map, Diagram Library

**Extension Infrastructure:**
| Component | Purpose |
|-----------|---------|
| `ExtensionManifest` | JSON parser for VS Code-compatible `package.json` |
| `ExtensionScanner` | Discovers installed extensions on disk |
| `GalleryService` / `HttpClient` | Marketplace search and download |
| `VsixService` | VSIX package install/uninstall (via libzip) |
| `ExtensionEnablement` | Enable/disable with global and workspace scopes |
| `ExtensionManagement` | High-level lifecycle orchestrator |
| `ExtensionSandbox` | Isolation with permission grants and resource limits |
| `ExtensionHostRecovery` | Crash detection and automatic restart |
| `ExtensionRecommendations` | File-type-based recommendation engine |
| `ExtensionTelemetry` | Activation/deactivation telemetry |
| `ExtensionStorage` | Persistent per-extension key-value storage |
| `ExtensionResourceTracker` | Per-extension resource usage monitoring |
| `PluginHotReload` | Hot-reload extensions without restart |
| `PluginQuarantine` | Quarantine misbehaving plugins |
| `PluginMemoryTracker` | Track plugin memory usage |
| `PluginSafeCall` | Exception-safe plugin invocation |

**Extension Services (P1-P4):**
| Service | Purpose |
|---------|---------|
| `SnippetEngine` | Snippet expansion with tab stops, placeholders, choice lists |
| `WorkspaceService` | Workspace file access and configuration |
| `TextEditorService` | Editor decoration and text manipulation API |
| `ProgressService` | Progress bar/notification lifecycle |
| `EnvironmentService` | Clipboard, URI opening, machine ID |
| `NotificationService` | Extension-facing notifications (Info/Warning/Error with actions) |
| `StatusBarItemService` | Status bar item creation/management |
| `InputBoxService` | Modal text input prompts |
| `QuickPickService` | Filterable selection lists (single/multi-select) |
| `ExtensionEvents` | Extension-scoped event pub/sub |
| `GrammarEngine` | TextMate grammar support (stub) |
| `TerminalService` | Terminal integration (stub) |
| `TaskRunnerService` | Task runner integration (stub) |

**Contribution Point Registries:**
`OutputChannelService`, `DiagnosticsService`, `TreeDataProviderRegistry`, `WebviewService`, `DecorationService`, `FileSystemProviderRegistry`, `LanguageProviderRegistry`

**Feature & Context:**

- `FeatureRegistry` — Runtime feature toggle with enable/disable/query and `FeatureToggledEvent`
- `ContextKeyService` — VS Code-style context key registry for conditional UI
- `WhenClause` — Boolean expression evaluator (AND, OR, NOT, ==, !=, regex match)
- `ContributionValidator` — Validates extension contribution points

---

### 4.5 AI Integration

**Purpose:** AI-powered writing assistance, code explanation, content generation.

| Component             | Purpose                                                                  |
| --------------------- | ------------------------------------------------------------------------ |
| `AIService`           | Core completion/streaming API with session management                    |
| `AIInlineCompleter`   | Inline text completion suggestions                                       |
| `AIDocumentGenerator` | Full document generation from prompts                                    |
| `AILinkSuggester`     | AI-powered link suggestions                                              |
| `AITagSuggester`      | AI-powered tag suggestions                                               |
| `AIWritingAssistant`  | Writing assistance (summarize, translate, expand, simplify, fix grammar) |
| `AIProviderConfig`    | Multi-provider configuration (OpenAI, Anthropic, Local/Ollama)           |

**AIActions:** `Summarize`, `Translate`, `ExpandContent`, `SimplifyContent`, `FixGrammar`, `GenerateOutline`, `ExplainCode`, `ContinueWriting`, `ChangeTone`, `ExtractKeywords`, `GenerateTitle`, `Chat`

**Constraints:** API integration requires libcurl (currently stub). Session-based chat with conversation history. Provider-specific endpoint routing (OpenAI, Anthropic, Local).

---

### 4.6 Export & Import System

**Purpose:** Multi-format document export/import with template support.

| Component                               | Purpose                                           |
| --------------------------------------- | ------------------------------------------------- |
| `ExportService`                         | Core export orchestration with progress callbacks |
| `IExportFormat` / `IImportFormat`       | Format interfaces                                 |
| `HtmlExporter`                          | HTML export with embedded/external assets         |
| `PdfExporter`                           | PDF export via Pandoc                             |
| `PandocExporter` / `PandocImporter`     | Pandoc-based conversion                           |
| `MarkdownExporter` / `MarkdownImporter` | Markdown format                                   |
| `HtmlImporter`                          | HTML import                                       |
| `DocumentImporter`                      | Multi-format import orchestration                 |
| `ExportTemplateEngine`                  | Customizable export templates                     |
| `ExportValidator`                       | Validates export output                           |
| `ExportDialog` / `ImportDialog`         | UI dialogs                                        |
| `BatchExportEngine`                     | Batch document export                             |
| `PublishingProfileManager`              | Publishing profile management                     |

---

### 4.7 Canvas System (`src/canvas/`)

**Purpose:** Infinite whiteboard with objects, tools, collaboration, and extensibility. 98 header files.

**Core Components:**
| Component | Purpose |
|-----------|---------|
| `Board` | Object container, spatial queries, add/remove/get objects |
| `BoardSerializer` | Save/load boards (format version 2) |
| `CanvasRenderer` | Renders board contents (wxWidgets-dependent) |
| `ViewportTransform` | Pan/zoom viewport management |
| `Quadtree` | Spatial indexing for hit-testing |
| `SelectionManager` | Object selection (single, multi, box) |
| `SnapEngine` | Snapping to grid, guides, objects |
| `UndoRedoStack` | Canvas-level undo/redo |

**Canvas Objects:**
`StickyNote`, `TextBox`, `ImageObject`, `ShapeData`, `FrameObject`, `GroupObject`, `IconObject`, `VideoEmbedObject`, `ConnectorData`, `TableObject`, `SectionObject`, `CommentObject`, `DiagramShapeObject`, `FreehandPath`, `AppWidgetObject`, `PDFPageObject`, `BookmarkCardObject`, `KanbanObjects`, `MindMapNode`

**Tools:**
`SelectTool`, `PanTool`, `DrawTool`, `CanvasToolHost`

**Canvas Services:**
| Service | Purpose |
|---------|---------|
| `CanvasClipboardService` | Copy/paste/duplicate canvas objects |
| `CanvasExportService` | Export canvas to image/PDF |
| `CanvasSearch` | Search across canvas objects |
| `CanvasStylePresetService` | Style preset management |
| `CanvasTemplateEngine` | Board templates |
| `CanvasObjectFactory` | Object creation factory |
| `CanvasObjectAligner` | Object alignment (left, right, center, distribute) |
| `ConnectorRoutingService` | Connector path routing |
| `GroupingService` | Object grouping/ungrouping |
| `LayeringService` | Z-order management |
| `LockingService` | Object locking |
| `TagManager` / `TagPanel` | Object tagging |
| `CanvasPerformanceMonitor` | Canvas rendering performance |
| `CanvasAccessibility` | Accessibility support |
| `CustomObjectTypeRegistry` | Register custom canvas object types |
| `BoardNavigator` | Board-level navigation |
| `BoardTemplate` | Board templates |
| `BoardLockController` | Board-level locking |
| `TileCache` | Tile-based canvas caching |

**Collaboration:**
| Component | Purpose |
|-----------|---------|
| `CanvasCollabService` | Real-time collaboration protocol |
| `CollabEventLogger` | Collaboration event logging |
| `CollabUndoRedoManager` | Collaborative undo/redo |
| `RemoteCursorOverlay` | Remote user cursor display |
| `RemoteSelectionOverlay` | Remote user selection display |
| `FollowModeController` | Follow another user's viewport |
| `PrivateRevealController` | Progressive reveal in presentations |
| `OfflineSyncQueue` | Offline operation queue |
| `CrossBoardLink` | Cross-board linking |

**Specialized Engines:**
| Engine | Purpose |
|--------|---------|
| `KanbanController` | Kanban board management |
| `MindMapController` | Mind map with auto-layout |
| `DiagramCoordinator` | Diagram editing coordination |
| `ShapeRecognizer` | Freehand shape recognition |
| `LayoutEngine` | Auto-layout algorithms |
| `ForceDirectedLayout` | Force-directed graph layout |
| `MetadataScraper` | Extract metadata from links |

---

### 4.8 Attribute View Database (`src/core/av/`)

**Purpose:** Notion-style database views with tables, galleries, kanban, and timeline.

| Component                              | Purpose                         |
| -------------------------------------- | ------------------------------- |
| `AttributeView` / `AttributeViewStore` | Core data model and persistence |
| `AVColumnType`                         | Column type definitions         |
| `AVFilter` / `AVSort`                  | Filtering and sorting           |
| `AVFormulaEngine`                      | Formula evaluation in cells     |
| `AVRollupEngine`                       | Rollup aggregations             |
| `AVQueryEngine`                        | Structured queries across views |
| `AVRelationService`                    | Cross-database relations        |
| `AVLinkedDatabase`                     | Linked database instances       |
| `AVImportExport`                       | CSV/JSON import/export          |
| `AVUndoManager`                        | Database-level undo/redo        |
| `AVValidators`                         | Data validation rules           |
| `AVFormatters`                         | Cell value formatting           |
| `AVTimelineView`                       | Timeline visualization          |
| `AVValue`                              | Typed cell values               |
| `AVAggregateRow`                       | Aggregate/summary rows          |

**UI Components (`src/ui/av/`):**
`AVTablePanel`, `AVGalleryPanel`, `AVKanbanPanel`, `AVCellEditor`, `AVCellRenderer`, `AVHeaderRenderer`, `AVFilterBar`, `AVGalleryCard`, `AVGalleryConfig`, `AVKanbanCard`, `AVKanbanLane`, `AVRelationEditor`, `AVRollupConfigDialog`

---

### 4.9 Flashcard & Spaced Repetition (`src/core/fsrs/`)

**Purpose:** FSRS-algorithm-based spaced repetition with flashcard management.

| Component                  | Purpose                           |
| -------------------------- | --------------------------------- |
| `FSRSEngine`               | FSRS scheduling algorithm         |
| `FSRSReviewSession`        | Review session management         |
| `FlashcardStore`           | Flashcard persistence             |
| `FlashcardExtractor`       | Extract flashcards from documents |
| `FlashcardImportExport`    | Import/export flashcard sets      |
| `ClozeParser`              | Cloze deletion parsing            |
| `LeechDetector`            | Detect poorly-learned cards       |
| `StudyStreakTracker`       | Track study streaks               |
| `FlashcardCommandProvider` | Command palette integration       |

---

### 4.10 Search System

**Purpose:** Multi-modal search across documents, canvas, and knowledge graph.

| Component               | Purpose                               |
| ----------------------- | ------------------------------------- |
| `SearchEngine`          | Core full-text search                 |
| `SearchContext`         | Dynamic pre/post match context buffer |
| `SearchService`         | Search orchestration                  |
| `SearchQueryParser`     | Query syntax parser                   |
| `ContentIndexer`        | Document content indexing             |
| `SearchIndexWatcher`    | Watch for indexable changes           |
| `IncrementalSearcher`   | Progressive text search               |
| `SearchResultNavigator` | Navigate search results               |
| `SearchReplacePreview`  | Search/replace with preview           |
| `SearchHistory`         | Search history tracking               |
| `SavedSearches`         | Persistent saved searches             |
| `SearchExporter`        | Export search results                 |
| `SearchAccessibility`   | Accessible search UI                  |
| `SearchCommands`        | Command palette integration           |
| `CanvasSearch`          | Search within canvas objects          |
| `NotebookSearchIndex`   | Search within notebooks               |

---

### 4.11 Knowledge Graph Engine

**Purpose:** Document relationship visualization and navigation.

| Component                           | Purpose                       |
| ----------------------------------- | ----------------------------- |
| `GraphEngine`                       | Core graph computation        |
| `GraphService`                      | Graph data management         |
| `GraphAnalytics`                    | Graph metrics and analysis    |
| `GraphSearch`                       | Search within graph           |
| `GraphExporter`                     | Export graph data             |
| `GraphLayoutAlgorithms`             | Layout computation            |
| `GraphAnnotations`                  | Graph annotations             |
| `GraphEvolution`                    | Track graph changes over time |
| `GraphAccessibility`                | Accessible graph UI           |
| `BacklinkIndex` / `BacklinkService` | Backlink tracking             |
| `LocalGraphEngine`                  | Local backlink graph          |
| `WikiLinkParser` / `WikiLink`       | Wiki-style link parsing       |
| `RefParser` / `RefResolver`         | Reference parsing/resolution  |
| `ReferenceScanner`                  | Scan documents for references |

---

### 4.12 Notebook System

**Purpose:** Jupyter-style computational notebooks.

| Component                          | Purpose                                |
| ---------------------------------- | -------------------------------------- |
| `Notebook`                         | Notebook data model                    |
| `NotebookCellManager`              | Cell lifecycle management              |
| `NotebookSerializer`               | Notebook serialization                 |
| `NotebookExecutionPipeline`        | Cell execution pipeline                |
| `NotebookExportEngine`             | Notebook export (markdown, HTML, PDF)  |
| `NotebookDiffEngine`               | Cell-level diffing and three-way merge |
| `NotebookSessionManager`           | Session management                     |
| `NotebookSearchIndex`              | Search across notebooks                |
| `NotebookToolbar`                  | Notebook-specific toolbar              |
| `KernelManager` / `KernelAdapters` | Compute kernel management              |
| `CellDependencyTracker`            | Cell dependency tracking               |

---

### 4.13 Task & Calendar System

**Purpose:** Task management with recurrence, reminders, and Gantt charts.

| Component             | Purpose                          |
| --------------------- | -------------------------------- |
| `TaskService`         | Core task management             |
| `TaskBoardEngine`     | Kanban-style task boards         |
| `TaskGanttEngine`     | Gantt chart visualization        |
| `TaskRecurrence`      | Recurring task scheduling        |
| `TaskReminder`        | Task reminders and notifications |
| `TaskAggregator`      | Aggregate tasks across documents |
| `TaskScheduler`       | Background task scheduling       |
| `TaskCommandProvider` | Command palette integration      |
| `CalendarEngine`      | Calendar view (in `src/ui/`)     |

---

### 4.14 Version Control Integration

**Purpose:** Git integration for document versioning.

| Component               | Purpose                    |
| ----------------------- | -------------------------- |
| `GitService`            | Core Git operations        |
| `GitBlameEngine`        | Blame/annotation           |
| `GitStashService`       | Stash management           |
| `GitRemoteService`      | Remote operations          |
| `CommitGraphEngine`     | Commit graph visualization |
| `DiffEngine`            | Unified diff computation   |
| `MergeConflictResolver` | Conflict resolution        |
| `HistoryService`        | File history               |
| `RepositoryService`     | Repository management      |

---

### 4.15 Cloud Sync

**Purpose:** Multi-provider cloud synchronization.

| Component                        | Purpose               |
| -------------------------------- | --------------------- |
| `CloudSyncService`               | Sync orchestration    |
| `S3Client` / `IS3Client`         | AWS S3 integration    |
| `WebDavClient` / `IWebDavClient` | WebDAV integration    |
| `SyncEngine`                     | Sync protocol         |
| `SyncScheduler`                  | Scheduled sync        |
| `SyncConflictManager`            | Conflict resolution   |
| `SyncHistoryLogger`              | Sync history          |
| `SelectiveSyncFilter`            | Selective sync rules  |
| `EncryptionService`              | End-to-end encryption |

---

### 4.16 Security & Privacy

**Purpose:** Encryption, vault management, and privacy controls.

| Component                                 | Purpose                                  |
| ----------------------------------------- | ---------------------------------------- |
| `EncryptionService`                       | AES-256 encryption                       |
| `KeyManager`                              | Key management                           |
| `VaultService` / `VaultEncryptionManager` | Encrypted vault support                  |
| `PrivacyManager`                          | Privacy controls                         |
| `ContentSecurityPolicy`                   | Content security enforcement             |
| `DataRedactionEngine`                     | PII redaction                            |
| `SecurityAuditLog`                        | Audit logging                            |
| `UrlSanitizer` / `ClipboardSanitizer`     | Input sanitization                       |
| `SafeMode`                                | Multi-tier safe mode with crash recovery |
| `RuntimePolicy`                           | Enterprise runtime protections           |

---

### 4.17 Presentation System

**Purpose:** Slide-based presentations from Markdown.

| Component                   | Purpose                     |
| --------------------------- | --------------------------- |
| `PresentationManager`       | Presentation lifecycle      |
| `SlideEngine`               | Slide processing            |
| `SlideExporter`             | Export to PDF/HTML          |
| `SlideThemeProvider`        | Presentation themes         |
| `SlideCommandProvider`      | Command palette integration |
| `DeckManager` / `DeckStore` | Deck management             |

---

### 4.18 WYSIWYG Editing

**Purpose:** Rich-text editing mode.

| Component                 | Purpose                           |
| ------------------------- | --------------------------------- |
| `WysiwygEngine`           | WYSIWYG rendering engine          |
| `WysiwygModeController`   | Mode switching (source ↔ WYSIWYG) |
| `WysiwygStatePersistence` | State persistence                 |
| `WysiwygAccessibility`    | Accessibility for WYSIWYG mode    |
| `LivePreviewRenderer`     | Real-time preview rendering       |

---

### 4.19 Internationalization

**Purpose:** Multi-language support.

| Component                             | Purpose                     |
| ------------------------------------- | --------------------------- |
| `LocaleManager`                       | Locale management           |
| `TranslationCatalog`                  | Translation strings         |
| `TextDirectionEngine`                 | RTL/LTR support             |
| `InternationalizationCommandProvider` | Command palette integration |
| `LocalizationCommandProvider`         | Localization commands       |

---

### 4.20 Reliability & Observability

**Purpose:** Application health monitoring, crash recovery, and diagnostics.

| Component                     | Purpose                      |
| ----------------------------- | ---------------------------- |
| `Watchdog`                    | Stall detection              |
| `HealthPanel`                 | Health metrics dashboard     |
| `CrashReporter`               | Crash reporting              |
| `SafeMode`                    | Safe mode with recovery      |
| `SubsystemReset`              | Subsystem reset coordination |
| `FaultDomain`                 | Fault domain management      |
| `ChaosEngine` / `ChaosPlugin` | Chaos testing                |
| `ErrorReportingService`       | Error aggregation            |
| `ObservabilityService`        | Distributed tracing          |
| `TracingService`              | Trace span management        |
| `OtlpExporter`                | OpenTelemetry export         |
| `StructuredLogger`            | Structured logging           |
| `SystemHealthValidator`       | System health checks         |
| `PanicPolicy`                 | Panic handling policy        |
| `IntegrationTestRunner`       | Integration test harness     |
| `RegressionTestEngine`        | Regression test automation   |
| `LoadTestHarness`             | Load testing                 |
| `CrossModuleTestSuite`        | Cross-module testing         |

---

### 4.21 Release Preparation (Phase 50)

**Purpose:** Release automation and deprecation tracking.

| Component                | Purpose                                    |
| ------------------------ | ------------------------------------------ |
| `ReleaseNotesGenerator`  | Generate release notes from commit history |
| `VersionManager`         | Version number management                  |
| `ChangelogEngine`        | Changelog generation                       |
| `DeprecationTracker`     | Track deprecated APIs                      |
| `ReleaseCommandProvider` | Release management commands                |
| `PolishCommandProvider`  | Code polish commands                       |

---

## 5 · Rendering System (`src/rendering/`)

**Purpose:** Document rendering pipeline, FX engine, and specialized block renderers.

### 5.1 Core Rendering

| Component              | Purpose                                           |
| ---------------------- | ------------------------------------------------- |
| `HtmlRenderer`         | Main HTML rendering with math and Mermaid support |
| `RenderPipeline`       | Rendering pipeline orchestration                  |
| `RenderingSafety`      | Rendering safety limits                           |
| `CodeBlockRenderer`    | Syntax-highlighted code blocks                    |
| `MermaidBlockRenderer` | Mermaid diagram rendering                         |
| `DiffRenderer`         | Diff visualization                                |

### 5.2 FX Engine

| Component              | Purpose                                 |
| ---------------------- | --------------------------------------- |
| `FxPass`               | Individual FX pass unit                 |
| `FxEngine`             | Pass-based compositing pipeline         |
| `FxMotionPreset`       | Motion preset definitions               |
| `TextFxRenderer`       | Text-specific FX rendering              |
| `FxPresetRegistry`     | FX preset management                    |
| `FxSafetyController`   | FX safety limits and performance guards |
| `FxTransitionEngine`   | Transition animations                   |
| `FxCssGenerator`       | Generate CSS from FX settings           |
| `FxAccessibility`      | Accessibility for FX features           |
| `FxDiagnostics`        | FX debugging and diagnostics            |
| `FxProfilePersistence` | FX profile save/load                    |
| `FxCommandProvider`    | Command palette integration             |

### 5.3 Block Renderers

`CalloutBlockRenderer`, `EmbedBlockRenderer`, `AudioBlockRenderer`, `VideoBlockRenderer`, `IFrameBlockRenderer`, `SuperBlockRenderer`, `InlineCalloutRenderer`, `InlineCheckboxRenderer`, `InlineCodeBlockRenderer`, `InlineFrontmatterRenderer`, `InlineHeadingRenderer`, `InlineImageRenderer`, `InlineLinkHandler`, `InlineMathRenderer`, `InlineTableRenderer`

### 5.4 Performance Primitives

| Component               | Purpose                                       |
| ----------------------- | --------------------------------------------- |
| `CaretOverlay`          | Phase-animated cursor blinking                |
| `DoubleBufferedPaint`   | Flicker-free paint with damage-rect merging   |
| `GlyphAdvanceCache`     | Per-font glyph width memoization              |
| `HitTestAccelerator`    | O(log n) column/line hit-testing              |
| `IncrementalLineWrap`   | Incremental soft-wrap recomputation           |
| `PrefetchManager`       | Viewport-aware line prefetching               |
| `ScrollBlit`            | Bit-copy scroll blitting with damage tracking |
| `SelectionPainter`      | Multi-selection range rendering               |
| `DirtyRegion`           | Dirty region accumulator                      |
| `ViewportCache`         | LRU viewport line caching                     |
| `ReadingProfileManager` | Reading preferences and accessibility         |

---

## 6 · UI Components (`src/ui/`)

### 6.1 Main Application Frame

| Component           | Purpose                                                                            |
| ------------------- | ---------------------------------------------------------------------------------- |
| `MainFrame`         | Main window: 50+ Edit/View menu items, print, recent files, command-line file open |
| `LayoutManager`     | Panel layout orchestration, 1500+ lines of editor action wiring                    |
| `CustomChrome`      | Native window chrome with traffic-light controls (macOS)                           |
| `NavigationService` | Multi-surface navigation                                                           |

### 6.2 Editor & Preview

| Component                 | Purpose                                                                                                             |
| ------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| `EditorPanel`             | Scintilla-based editor: 60+ VS Code-inspired features, right-click context menu, sticky scroll, 8 Markdown snippets |
| `MinimapPanel`            | Bespoke custom-rendered token-aware minimap with bitmap caching, viewport slider, and semantic marker overlays      |
| `OverviewRulerPanel`      | Dedicated right-edge vertical lane displaying code markers (errors, warnings, breakpoints) and editor states        |
| `PreviewPanel`            | HTML preview with scroll-to-top, print-friendly CSS                                                                 |
| `SplitView`               | Editor/preview split layout                                                                                         |
| `EditorGroupManager`      | Multi-editor group support                                                                                          |
| `EditorPerformanceBudget` | Editor performance monitoring                                                                                       |
| `EditorStateSerializer`   | Editor state persistence                                                                                            |

### 6.3 Sidebar & Navigation

| Component              | Purpose                                                                                               |
| ---------------------- | ----------------------------------------------------------------------------------------------------- |
| `FileTreeCtrl`         | File tree with type-ahead, indent guides, 30+ icons, context menus                                    |
| `ActivityBar`          | Vertical icon rail (Explorer, Search, Settings, Themes, Extensions)                                   |
| `BreadcrumbBar`        | Interactive vector-rendered path breadcrumbs with drag/drop, context menus, and workspace Quick Picks |
| `SidebarHeader`        | Top-level sidebar header with workspace name and global actions                                       |
| `PanelHeader`          | Reusable header for individual sidebar panels                                                         |
| `SidebarSection`       | Collapsible section container for grouping panel content                                              |
| `SidebarFooter`        | Optional footer for contextual status information                                                     |
| `EmptyPanelState`      | Helpful empty state visualization when a panel has no content                                         |
| `SidebarPanelRegistry` | Register sidebar panels                                                                               |
| `SidebarMode`          | Sidebar display modes                                                                                 |
| `SecondarySidebar`     | Secondary sidebar wrapper framework                                                                   |
| `SecondaryTabStrip`    | Horizontal drag-and-drop tab strip for secondary layout payload                                       |
| `BacklinksPanel`       | Sidebar backlinks view                                                                                |
| `SearchSidebarPanel`   | Search sidebar                                                                                        |
| `GraphSidebarPanel`    | Knowledge graph sidebar                                                                               |
| `PaneManager`          | Multi-pane layout management                                                                          |

### 6.4 Status & Notification

| Component             | Purpose                                                                                      |
| --------------------- | -------------------------------------------------------------------------------------------- |
| `StatusBarPanel`      | Status bar (filename, language, file size, EOL, encoding, indent, zoom, cursor, SAVED flash) |
| `TabBar`              | Tab management with pin, drag, close, modified indicators                                    |
| `NotificationManager` | Toast notifications (Info/Warning/Error/Success)                                             |
| `CommandPalette`      | Fuzzy-searchable command launcher (Cmd+Shift+P)                                              |
| `FloatingFormatBar`   | Inline formatting toolbar                                                                    |

### 6.5 Settings & Configuration UI

| Component            | Purpose                                                              |
| -------------------- | -------------------------------------------------------------------- |
| `SettingsPanel`      | VS Code-style settings editor with search, categories, import/export |
| `SettingsDialog`     | Modal settings dialog                                                |
| `SettingsJsonEditor` | Raw JSON/YAML settings editor                                        |
| `ShortcutEditor`     | Keyboard shortcut editor                                             |
| `ShortcutOverlay`    | Keyboard shortcut overlay                                            |
| `ThemeGallery`       | Theme browser and preview                                            |
| `ThemePreviewCard`   | Theme preview cards                                                  |
| `ThemeTokenEditor`   | Theme token editor                                                   |
| `FirstRunWizard`     | First-run onboarding wizard                                          |

### 6.6 Extension UI

| Component                | Purpose                           |
| ------------------------ | --------------------------------- |
| `ExtensionsBrowserPanel` | Extension marketplace browser     |
| `ExtensionCard`          | Extension list item card          |
| `ExtensionDetailPanel`   | Extension detail view             |
| `OutputPanel`            | Extension output channels         |
| `ProblemsPanel`          | Diagnostics display               |
| `TreeViewHost`           | Custom tree views from extensions |
| `WalkthroughPanel`       | Extension walkthroughs            |
| `WebviewHostPanel`       | Webview content hosting           |

### 6.7 Specialized Panels

| Component                                                       | Purpose                       |
| --------------------------------------------------------------- | ----------------------------- |
| `CanvasPanel`                                                   | Canvas board editing panel    |
| `CanvasWorkspacePanel`                                          | Canvas workspace management   |
| `CanvasCommentsPanel`                                           | Canvas comments panel         |
| `CanvasFacilitationPanel`                                       | Facilitation tools            |
| `GraphPanel` / `GraphViewPanel`                                 | Knowledge graph visualization |
| `GraphFilterBar` / `GraphMiniMap`                               | Graph filtering and minimap   |
| `DiffPanel`                                                     | Diff/compare view             |
| `HistoryPanel`                                                  | File history panel            |
| `SearchPanel`                                                   | Full search panel             |
| `PDFViewerPanel` / `PDFAnnotationSidebar` / `PDFThumbnailStrip` | PDF viewing                   |
| `FlashcardBrowserPanel` / `FlashcardReviewPanel`                | Flashcard management          |
| `ToolWindowHost`                                                | Dockable tool windows         |
| `WorkspaceLayout`                                               | Workspace layout management   |

### 6.8 Shared UI Primitives

| Component                      | Purpose                                           |
| ------------------------------ | ------------------------------------------------- |
| `ThemeAwareWindow`             | Base class for theme-responsive windows           |
| `ThemedScrollbar`              | Themed scrollbar rendering                        |
| `BevelPanel`                   | Bevel-style panel                                 |
| `SplitterBar`                  | Draggable splitter with hover animation           |
| `SurfaceTransitionCoordinator` | Animated surface transitions                      |
| `Toolbar`                      | Main toolbar with zoom slider and format dropdown |
| `StartupPanel`                 | Landing page with recent workspaces               |
| `TableEditorOverlay`           | Table editing overlay                             |
| `ImagePreviewPopover`          | Image hover preview                               |
| `LinkPreviewPopover`           | Link hover preview                                |

### 6.9 Accessibility Features (`src/ui/accessibility/`)

| Component                 | Purpose                                                      |
| ------------------------- | ------------------------------------------------------------ |
| `AccessibilityController` | Broad coordinator for focus events and screen reader updates |
| `SkipToContentButton`     | Hidden keyboard-navigable landmark to skip to editor         |

---

## 7 · Platform Layer (`src/platform/`)

| Component                     | Purpose                                              |
| ----------------------------- | ---------------------------------------------------- |
| `PlatformAbstraction`         | Platform detection and dispatch                      |
| `MacPlatform`                 | macOS-specific (Objective-C++ bridge)                |
| `WinPlatform`                 | Windows-specific (Win32 API)                         |
| `LinuxPlatform`               | Linux-specific (GTK/GDK)                             |
| `ScreenReaderBridge`          | Base screen reader integration                       |
| `MacScreenReaderBridge`       | NSAccessibility-based screen reader                  |
| `LinuxScreenReaderBridge`     | Linux screen reader stub                             |
| `WinScreenReaderBridge`       | Windows screen reader stub                           |
| `AccessibilityIdentifier`     | wxWidgets↔NSAccessibility bridge with hierarchy walk |
| `StubAccessibilityIdentifier` | No-op stub for non-macOS platforms                   |

Platform services implemented in `PlatformServices.h/.cpp`: `FileWatcher` (polling-based), clipboard, shell operations.

---

## 8 · Performance Infrastructure

| Component                                  | Purpose                                         |
| ------------------------------------------ | ----------------------------------------------- |
| `FrameArena`                               | Monotonic arena allocator                       |
| `FrameScheduler`                           | Priority-based task execution                   |
| `FrameBudgetToken` / `FrameBudgetEnforcer` | Frame-time budget accounting                    |
| `SPSCQueue`                                | Lock-free single-producer/single-consumer queue |
| `AdaptiveThrottle`                         | Frame-rate-adaptive update coalescing           |
| `AsyncPipeline`                            | Staged background processing                    |
| `CoalescingTask`                           | Debounced background task execution             |
| `ChunkedStorage`                           | Cache-friendly contiguous block allocation      |
| `CompilerHints`                            | Branch-prediction and prefetch intrinsics       |
| `GenerationCounter`                        | Lock-free generation-based invalidation         |
| `GraphemeBoundaryCache`                    | Unicode grapheme cluster lookups                |
| `DocumentSnapshot`                         | Atomic publish/consume                          |
| `Profiler`                                 | Performance profiling and metric tracking       |
| `PerformanceProfiler`                      | Extended performance profiling                  |
| `StartupTimer`                             | Application startup timing                      |
| `LazyService`                              | Deferred service initialization                 |
| `BackpressureController`                   | Backpressure management                         |
| `SoAStorage`                               | Struct-of-Arrays storage                        |
| `RingBuffer`                               | Ring buffer implementation                      |
| `BoundedContainer`                         | Bounds-checked containers                       |
| `MemoryBudget` / `MemoryBudgetEnforcer`    | Memory budget tracking                          |
| `AllocatorConfig`                          | Custom allocator configuration                  |
| `SafeAllocation`                           | Safe memory allocation                          |

---

## 9 · Math Rendering

| Component       | Purpose                                     |
| --------------- | ------------------------------------------- |
| `IMathRenderer` | Abstract math rendering interface           |
| `MathRenderer`  | Pure C++ LaTeX-to-Unicode (~120 symbol map) |
| `LatexEngine`   | Extended LaTeX processing                   |

**Inline/Display Math:** `$...$` (inline), `$$...$$` (display) via md4c `MD_FLAG_LATEXMATHSPANS`.  
**Constructor Chain:** `MarkAmpApp → MainFrame → LayoutManager → SplitView → PreviewPanel → HtmlRenderer`

---

## 10 · Markdown Processing

| Component                                  | Purpose                        |
| ------------------------------------------ | ------------------------------ |
| `MarkdownParser` / `IMarkdownParser`       | Markdown parsing to AST        |
| `Md4cWrapper`                              | md4c library wrapper           |
| `SyntaxHighlighter`                        | Syntax highlighting engine     |
| `IncrementalTokenizer`                     | Incremental tokenization       |
| `IncrementalParsePolicy`                   | Incremental parsing strategy   |
| `Outline` / `OutlineService`               | Document outline extraction    |
| `DocumentModel`                            | Document data model            |
| `Block` / `BlockDatabase` / `BlockService` | Block-level document structure |
| `BlockDiffEngine`                          | Block-level diffing            |
| `BlockTreeIndex`                           | Block tree indexing            |
| `BlockTypeDetector`                        | Block type detection           |
| `BlockReferenceResolver`                   | Block reference resolution     |
| `IALParser`                                | Inline Attribute List parser   |
| `Tag` / `TagService`                       | Document tagging               |
| `HtmlSanitizer`                            | HTML sanitization              |
| `MimeValidator`                            | MIME type validation           |

---

## 11 · Dependencies (vcpkg)

| Dependency      | Purpose                                   |
| --------------- | ----------------------------------------- |
| `wxwidgets`     | Cross-platform UI toolkit                 |
| `nlohmann-json` | JSON parsing                              |
| `md4c`          | Markdown parsing library                  |
| `catch2`        | Unit testing framework                    |
| `fmt`           | String formatting                         |
| `spdlog`        | Logging                                   |
| `yaml-cpp`      | YAML parsing (config files)               |
| `nanosvg`       | SVG icon rendering                        |
| `libzip`        | ZIP handling (VSIX packages)              |
| `openssl`       | Cryptography                              |
| `cpp-httplib`   | HTTP client                               |
| `sqlite3`       | Database (with FTS5 for full-text search) |
| `tracy`         | Profiling                                 |
| `benchmark`     | Google Benchmark                          |
| `mimalloc`      | High-performance allocator                |

---

## 12 · Coding Standards & Constraints

### 12.1 Language & Compiler

- **Standard:** C++23 (`-std=c++23`); C++26-ready patterns used where supported
- **Compiler:** Apple Clang 21+ (macOS), GCC 14+ (Linux), MSVC 2022+ (Windows)
- **Warnings:** `-Wall -Wextra -Werror` — ALL warnings are errors
- **Linting:** clang-tidy with project `.clang-tidy` config

### 12.2 Style Rules

- **No `any` or `unknown` types** — Always use precise types, interfaces, or generics
- **No raw `new`/`delete`** — Use `std::unique_ptr`, `std::shared_ptr`, or arena allocators
- **Header guards:** `#pragma once`
- **Namespace:** `markamp::core`, `markamp::canvas`, `markamp::ui`, `markamp::rendering`
- **Naming:** `snake_case` for functions/variables, `PascalCase` for classes, `kPascalCase` for enum values
- **Member variables:** trailing underscore (`event_bus_`, `config_`)
- **Return type syntax:** Trailing return types preferred (`auto foo() -> ReturnType`)
- **Error handling:** `std::expected<T, std::string>` for fallible operations
- **`[[nodiscard]]`** on all pure query methods
- **`[[maybe_unused]]`** for stored-for-future-use members
- **Event declaration:** `MARKAMP_DECLARE_EVENT` macro

### 12.3 Build System

- **CMake presets:** `debug`, `release`, `release-static`
- **Version management:** `CMakeLists.txt` + `vcpkg.json` (must stay in sync)
- **Source organization:** `target_sources()` in `src/CMakeLists.txt`, `source_group()` for IDE layout
- **Test targets:** Each test file gets its own executable in `tests/CMakeLists.txt`
- **Dependencies:** Link via `markamp_dependencies` interface target

### 12.4 Common Pitfalls

| Pitfall                                  | Solution                                                                                                                          |
| ---------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| Duplicate source files in CMakeLists     | Check with `sort \| uniq -d` — some implementations exist in aggregate files (e.g., `PlatformServices.cpp`, `TracingService.cpp`) |
| `-Werror` unused parameters              | Use `(void)param_name;` for stub methods                                                                                          |
| `-Werror` unused private fields          | Mark with `[[maybe_unused]]` attribute                                                                                            |
| `-Werror` sign conversion                | Use `size_t` for container indexing, not `int`                                                                                    |
| wxWidgets in tests                       | Exclude wx-dependent source files from test targets using GLOB + `list(FILTER EXCLUDE)`                                           |
| Linker errors in tests                   | Link `markamp_dependencies` and include required transitive source files                                                          |
| Two `FileWatcher` classes                | `PlatformServices.h` defines one; `FileWatcher.h` (Phase 20) defines another — do NOT compile both                                |
| Two `ExtensionTelemetry` implementations | `TracingService.cpp` provides implementation — do NOT also compile `ExtensionTelemetry.cpp`                                       |

### 12.5 Documentation

- **Architecture:** `docs/` directory (539 files)
- **API Reference:** `docs/api_reference.md`
- **Changelog:** `CHANGELOG.md` (versioned release notes)
- **Theme definitions:** Markdown files in `themes/` with YAML frontmatter
- **Agent context:** This file (`AGENTS.md`)
- **Claude Code context:** `CLAUDE.md`

---

## 13 · File Quick Reference

### Key Entry Points

| File                       | Purpose                            |
| -------------------------- | ---------------------------------- |
| `src/main.cpp`             | Application entry point            |
| `src/app/MarkAmpApp.h`     | wxApp subclass                     |
| `src/ui/MainFrame.h`       | Main window                        |
| `src/ui/LayoutManager.h`   | Panel layout and event wiring      |
| `src/core/EventBus.h`      | Event system                       |
| `src/core/Events.h`        | All event type declarations        |
| `src/core/Config.h`        | Configuration system               |
| `src/core/Theme.h`         | Theme data and FX settings         |
| `src/core/ThemeEngine.h`   | Theme runtime management           |
| `src/core/IPlugin.h`       | Plugin interface and context       |
| `src/core/PluginManager.h` | Plugin lifecycle                   |
| `CMakeLists.txt`           | Top-level CMake (version, presets) |
| `src/CMakeLists.txt`       | Source file registration           |
| `tests/CMakeLists.txt`     | Test target registration           |
| `vcpkg.json`               | Dependency manifest                |

### Configuration Files

| File                        | Purpose                      |
| --------------------------- | ---------------------------- |
| `.clang-format`             | Code formatting rules        |
| `.clang-tidy`               | Static analysis rules        |
| `CMakePresets.json`         | CMake preset definitions     |
| `vcpkg-configuration.json`  | vcpkg registry configuration |
| `cppcheck-suppressions.txt` | cppcheck suppressions        |

---

## 14 · CLI Tools Available

The following tools are available on the development CLI for use in automation, analysis, and agent operations:

| Tool         | Purpose                                                 |
| ------------ | ------------------------------------------------------- |
| `ast-grep`   | Structural code search and transformation via AST rules |
| `difftastic` | Structural diff tool (syntax-aware, language-aware)     |
| `shellcheck` | Static analysis for shell scripts                       |
| `sd`         | Find-and-replace CLI (sed alternative with regex)       |
| `scc`        | Source lines of code counter (fast, language-aware)     |
| `yq`         | YAML/JSON/XML processor (jq-like)                       |
| `comby`      | Structural code search and rewriting                    |
| `hyperfine`  | CLI benchmarking tool                                   |
| `watchexec`  | File watcher that re-runs commands on change            |
| `git-delta`  | Syntax-highlighting diff pager for Git                  |
