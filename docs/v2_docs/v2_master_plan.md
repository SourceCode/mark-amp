# MarkAmp v2 Master Improvement Plan

**Date:** 2026-02-14
**Current Version:** 1.9.12
**Scope:** Full repository review -- 20 phases covering all identified issues

## Executive Summary

After a thorough review of every source file in the MarkAmp codebase (~160 files across core/, ui/, rendering/, platform/, app/, and tests/), this plan documents all inconsistencies, incomplete implementations, stub services, dead code, architectural gaps, and standardization opportunities.

The most critical finding is a **two-tier codebase**: the UI layer (wxWidgets-based panels, 60+ editor actions, 8 themes, full tab/file management) is mature and functionally complete, while the extension/plugin infrastructure layer (30+ service files added in v1.9.12) is structurally present but largely **unwired** -- services exist as data models and pass tests, but are never instantiated in the running application and have no UI integration.

## Key Findings

### Critical Issues
1. **15 P1-P4 extension services never instantiated** -- SnippetEngine, WorkspaceService, TextEditorService, ProgressService, ExtensionEventBus, EnvironmentService, GrammarEngine, TerminalService, TaskRunnerService, NotificationServiceAPI, StatusBarItemService, InputBoxService, QuickPickService are declared in PluginContext but never created in MarkAmpApp.cpp or wired to any UI
2. **PluginContext fields always null** -- activate_plugin() creates a PluginContext but only sets event_bus, config, and register_command_handler; all 15 service pointers remain nullptr
3. **OutputPanel, ProblemsPanel, TreeViewHost, WebviewHostPanel, WalkthroughPanel** are data-only classes with no wxWidgets rendering -- they have no OnPaint, no Create(), no parent window
4. **SettingsBatchChangedEvent** is documented in HISTORY.md but the event type does not exist in Events.h
5. **publish_fast()** method on EventBus still takes a mutex lock (identical to publish()), defeating its purpose
6. **process_queued() / drain_fast_queue()** are never called from any application idle handler

### High-Priority Issues
7. **Orphan snippet files** -- `snippet_mousewheel.cpp` and `snippet_preview_zoom.cpp` are loose function definitions not included in any header or CMakeLists.txt build
8. **Duplicate event types** -- Multiple events describe the same action (e.g., `ToggleMinimapRequestEvent` + `ToggleMinimapR11RequestEvent`, `ReverseLinesRequestEvent` + `ReverseSelectedLinesRequestEvent`, `FoldCurrentRequestEvent` + `FoldCurrentRegionRequestEvent`)
9. **Events.h is 1500+ lines** of boilerplate -- every event type is 8 lines of identical structure differing only in the type_name() string
10. **BuiltInPlugins activate() is a no-op** -- they set `active_ = true` and log, but register zero command handlers or behavior
11. **FeatureRegistry checked nowhere** -- built-in features are registered but no UI code calls `feature_registry.is_enabled()` to guard feature visibility
12. **EnvironmentService.clipboard_write()** writes to a local std::string, not the system clipboard
13. **EnvironmentService.open_external()** returns true without actually opening anything
14. **InputBoxService.show() and QuickPickService.show()** store callbacks but never trigger UI

### Medium-Priority Issues
15. **No test coverage** for the 8 new UI panels (ExtensionsBrowserPanel, ExtensionCard, ExtensionDetailPanel, OutputPanel, ProblemsPanel, TreeViewHost, WalkthroughPanel, WebviewHostPanel)
16. **source_group in CMakeLists.txt** is incomplete -- only lists files through the original extension panels, missing 20+ new source files
17. **ServiceRegistry** is a singleton used for only one service (MermaidRenderer) -- inconsistent with the dependency-injection pattern used everywhere else
18. **MarkdownDocument.h/.cpp** has a `.cpp` file in the build but no matching `.h` in the header list
19. **RecentWorkspaces** not included in CMakeLists.txt source_group
20. **Platform stubs** (WinPlatform, LinuxPlatform) are almost entirely empty with TODO comments

### Low-Priority / Polish Issues
21. **Inconsistent naming**: `BuiltinThemes` vs `BuiltInPlugins` (casing of "In")
22. **Config.cpp hardcodes 60+ defaults** that could be data-driven from a JSON/YAML defaults file
23. **Commented-out step 10 numbering** -- MarkAmpApp.cpp has two "step 10" comments
24. **PreviewPanel line 594** references "KaTeX / math placeholder" -- math rendering not implemented
25. **WinPlatform.cpp line 136** has a TODO for UIA accessibility

## Phase Dependency Graph

```
Phase 01 (Event System) --> Phase 02 (Service Wiring)
Phase 02 --> Phase 03 (PluginContext Completion)
Phase 03 --> Phase 04 (Feature Guards)
Phase 01 --> Phase 05 (Event Deduplication)
Phase 06 (Panel UI) depends on Phase 02
Phase 07 (InputBox/QuickPick UI) depends on Phase 02
Phase 08 (BuiltIn Plugin Behavior) depends on Phase 03, 04
Phase 09 (Environment Platform Integration) standalone
Phase 10 (publish_fast Fix) standalone
Phase 11 (Orphan Code Cleanup) standalone
Phase 12 (CMake Standardization) standalone
Phase 13 (Test Coverage) depends on Phase 02, 06
Phase 14 (Naming Consistency) standalone
Phase 15 (Config Defaults Externalization) standalone
Phase 16 (Platform Stubs) standalone
Phase 17 (ServiceRegistry Refactor) standalone
Phase 18 (Math/KaTeX Rendering) standalone
Phase 19 (Documentation Sync) depends on all
Phase 20 (Performance & Polish) depends on Phase 10
```

## Phase Summary

| Phase | Title | Priority | Files | Dependencies |
|-------|-------|----------|-------|--------------|
| 01 | Event System Macro Refactor & Deduplication | Critical | ~5 | None |
| 02 | Extension Service Instantiation & Wiring | Critical | ~8 | 01 |
| 03 | PluginContext Full Population | Critical | ~4 | 02 |
| 04 | Feature Guard Integration | Critical | ~15 | 03 |
| 05 | Event Type Deduplication | High | ~8 | 01 |
| 06 | Panel wxWidgets Rendering | High | ~10 | 02 |
| 07 | InputBox & QuickPick UI Integration | High | ~6 | 02 |
| 08 | BuiltIn Plugin Behavioral Activation | High | ~4 | 03, 04 |
| 09 | EnvironmentService Platform Integration | High | ~3 | None |
| 10 | EventBus publish_fast & Queue Processing | High | ~4 | None |
| 11 | Orphan Code & Dead File Cleanup | Medium | ~6 | None |
| 12 | CMake Source Group & Build Standardization | Medium | ~3 | None |
| 13 | Test Coverage for New Services & Panels | Medium | ~15 | 02, 06 |
| 14 | Naming Convention Standardization | Medium | ~20 | None |
| 15 | Config Defaults Externalization | Medium | ~4 | None |
| 16 | Platform Stub Completion | Medium | ~4 | None |
| 17 | ServiceRegistry Singleton Elimination | Low | ~5 | None |
| 18 | Math/KaTeX Rendering Foundation | Low | ~6 | None |
| 19 | Documentation & AGENTS.md Sync | Low | ~8 | All |
| 20 | Performance Optimization & Final Polish | Low | ~10 | 10 |

## Files Scanned

### Core (93 files)
All `.h` and `.cpp` files in `src/core/`, `src/core/loader/`

### UI (72 files)
All `.h` and `.cpp` files in `src/ui/`

### Rendering (16 files)
All `.h` and `.cpp` files in `src/rendering/`

### Platform (7 files)
All files in `src/platform/`

### App (3 files)
`src/main.cpp`, `src/app/MarkAmpApp.h`, `src/app/MarkAmpApp.cpp`

### Tests (57 files)
All test files in `tests/unit/`, `tests/integration/`, `tests/performance/`

### Build (4 files)
`CMakeLists.txt`, `src/CMakeLists.txt`, `tests/CMakeLists.txt`, `CMakePresets.json`

### Documentation (40+ files)
`AGENTS.md`, `README.md`, `HISTORY.md`, all files in `docs/`
