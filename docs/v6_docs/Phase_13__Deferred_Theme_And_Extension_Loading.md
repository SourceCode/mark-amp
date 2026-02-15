# Phase 13: Deferred Theme & Extension Loading

## Metadata

| Field | Value |
|---|---|
| Phase ID | 13 |
| Prerequisites | Phase 03 |
| Estimated Complexity | High |
| Estimated File Count | 1 created, 7 modified |
| PRD Sections | 3.2 Lazy Initialization, 3.3 Reduce I/O During Startup |

---

## Objective

Move ThemeRegistry filesystem scanning and extension scanning off the startup critical path, using AsyncPipeline for background loading. Built-in themes and plugins are available immediately; user themes and extensions load asynchronously after the first frame.

---

## Background

ThemeRegistry::initialize() scans the filesystem for theme files synchronously during startup, blocking the UI. Similarly, PluginManager::activate_all() scans for extensions and activates them synchronously. The PRD mandates: "Startup should load only last workspace and last open file. Defer extension scanning, marketplace requests, telemetry initialization, syntax highlighting full scan." AsyncPipeline already exists in the codebase for background work.

---

## Scope

### Tasks

1. **Refactor `ThemeRegistry`** (`src/core/ThemeRegistry.h`, `ThemeRegistry.cpp`):
   - Split `initialize()` into two methods:
     - `initialize_builtin()`: synchronous, fast — loads only built-in themes from constexpr data (Phase 11)
     - `scan_user_themes()`: async — scans filesystem for user theme files via AsyncPipeline
   - Built-in themes are immediately available after `initialize_builtin()`
   - User themes become available when background scan completes
   - Publish `UserThemesLoadedEvent` when scan finishes
   - ThemeEngine starts with built-in themes, updates its registry when user themes arrive

2. **Refactor `PluginManager`** (`src/core/PluginManager.h`, `PluginManager.cpp`):
   - Split `activate_all()` into:
     - `activate_builtin()`: synchronous — activates only built-in feature plugins (Mermaid, Table Editor, etc.)
     - `scan_and_activate_extensions()`: deferred — scans extension directories and activates user extensions
   - Built-in plugins activate immediately (fast path)
   - Extension scanning deferred to post-first-frame using idle handler or event trigger
   - Extensions activate in dependency order (topological sort preserved)

3. **Add events to `Events.h`**:
   - `StartupDeferralEvent`: published after first frame rendered, triggers deferred loading
   - `UserThemesLoadedEvent`: published when background theme scan completes
   - `ExtensionsScanCompleteEvent`: published when extension scanning finishes

4. **Wire deferred trigger in `MarkAmpApp`**:
   - After first frame paint: publish `StartupDeferralEvent`
   - ThemeRegistry subscribes and starts `scan_user_themes()`
   - PluginManager subscribes and starts `scan_and_activate_extensions()`

5. **Instrument with StartupTimer checkpoints**:
   - Checkpoint: `builtin_themes_loaded` (synchronous)
   - Checkpoint: `builtin_plugins_activated` (synchronous)
   - Checkpoint: `deferred_loading_triggered` (post-first-frame)

6. **Create `tests/unit/test_deferred_loading.cpp`**:
   - Test that built-in themes are available before deferred loading
   - Test that user themes become available after async completion
   - Test that extensions activate after deferral trigger
   - Test error handling (missing extension directory, corrupt theme file)

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/ThemeRegistry.h` |
| Modify | `src/core/ThemeRegistry.cpp` |
| Modify | `src/core/PluginManager.h` |
| Modify | `src/core/PluginManager.cpp` |
| Modify | `src/core/Events.h` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Create | `tests/unit/test_deferred_loading.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- AsyncPipeline provides `submit()` for background work and completion callbacks on the main thread. Use this for both theme scanning and extension scanning.
- The first-frame detection from Phase 03 provides the trigger point. The idle handler after first paint is the ideal location.
- Built-in themes should be compiled into the binary as constexpr data (from Phase 11), requiring zero filesystem I/O.
- Extension scanning involves: reading directory entries, parsing JSON manifests, resolving dependencies. All of this should happen in background.
- The UI must remain responsive during background loading. Extensions that modify the UI (contribute menu items, sidebar panels) should apply their contributions on the main thread after loading.
- Error handling: if a user theme file is corrupt, log a warning and skip it. If an extension fails to load, disable it and log the error.

---

## Acceptance Criteria

- [ ] Built-in themes available before first frame (verified by test)
- [ ] User theme scan completes in background without blocking UI
- [ ] Extension scanning starts after first frame rendered
- [ ] `bench_startup` (Phase 08) shows reduced time-to-first-frame
- [ ] `test_deferred_loading` validates async completion for themes and extensions
- [ ] `StartupDeferralEvent` is published exactly once after first frame
- [ ] No visual glitches when user themes load (theme switch is smooth)
- [ ] Extensions contribute UI elements after activation without crashes

---

## Testing Strategy

- Unit test for deferred loading sequence
- Run application and verify first frame appears before extension scanning
- Verify theme switcher shows built-in themes immediately, user themes appear later
- Run full test suite to verify no regressions
