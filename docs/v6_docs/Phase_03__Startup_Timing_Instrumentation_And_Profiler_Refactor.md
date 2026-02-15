# Phase 03: Startup Timing Instrumentation & Profiler Refactor

## Metadata

| Field | Value |
|---|---|
| Phase ID | 03 |
| Prerequisites | Phase 02 |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 5 modified |
| PRD Sections | 3.1-3.3, 7.3 Startup Budget, 10 Observability |

---

## Objective

Insert precise timing probes at every startup phase and refactor the Profiler from a singleton to an injectable instance (per CLAUDE.md convention: services are passed via constructor injection, NOT ServiceRegistry singleton).

---

## Background

The PRD specifies startup must be measured via "timestamp at main()" and "timestamp at first frame rendered," targeting cold start <150ms and warm start <50ms. The current `Profiler` class uses `static instance()` (singleton pattern) which violates the project's constructor injection convention. This phase creates the measurement infrastructure that all subsequent optimization phases depend on.

---

## Scope

### Tasks

1. **Refactor `Profiler` class** (`src/core/Profiler.h`, `src/core/Profiler.cpp`):
   - Remove `static instance()` singleton method
   - Make the class constructible and movable
   - Add to PluginContext for extension access (pointer field)
   - Update `MARKAMP_PROFILE_SCOPE` macro to accept a Profiler reference or use thread-local pointer
   - Preserve all existing measurement functionality

2. **Create `StartupTimer` utility** (`src/core/StartupTimer.h`, `src/core/StartupTimer.cpp`):
   - Records high-resolution timestamps for named checkpoints
   - Checkpoint names include: `main_entry`, `wxapp_oninit_start`, `config_loaded`, `theme_initialized`, `services_created`, `plugins_activated`, `first_frame_rendered`
   - `checkpoint(name)` records current `std::chrono::steady_clock::now()`
   - `report_json()` produces JSON string with all checkpoints and deltas
   - `report_log()` logs each checkpoint duration via MARKAMP_LOG_INFO
   - Uses `[[nodiscard]]` on query methods per CLAUDE.md convention

3. **Instrument `MarkAmpApp::OnInit()`**:
   - Create `StartupTimer` at the beginning of `OnInit()`
   - Insert `checkpoint()` calls between each initialization phase
   - Log the final report before returning from `OnInit()`
   - Store the StartupTimer for later access (e.g., from Health Panel)

4. **Add first-frame-rendered detection**:
   - Add mechanism to detect first paint completion (e.g., flag in idle handler after first `wxPaintEvent`)
   - Record final checkpoint when first frame completes
   - Publish `StartupCompleteEvent` with total duration

5. **Create test: `test_startup_timer.cpp`**:
   - Validate checkpoint recording and ordering
   - Validate JSON output format
   - Validate delta calculations

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/Profiler.h` |
| Modify | `src/core/Profiler.cpp` |
| Create | `src/core/StartupTimer.h` |
| Create | `src/core/StartupTimer.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/app/MarkAmpApp.h` |
| Create | `tests/unit/test_startup_timer.cpp` |
| Modify | `src/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- Use `std::chrono::steady_clock` for all timing (monotonic, not affected by system clock changes).
- The Profiler refactor must update all existing call sites that use `Profiler::instance()`. Search for `Profiler::instance` across the codebase.
- Consider using a thread-local pointer for the `MARKAMP_PROFILE_SCOPE` macro to avoid passing Profiler everywhere: set it once per thread at creation.
- The JSON report should use the format: `{"checkpoints": [{"name": "...", "timestamp_us": N, "delta_us": N}], "total_us": N}`

---

## Acceptance Criteria

- [ ] Profiler no longer has `static instance()` method
- [ ] Profiler is injectable via constructor/pointer
- [ ] `MARKAMP_PROFILE_SCOPE` macro works with non-singleton Profiler
- [ ] StartupTimer produces valid JSON with all checkpoint names
- [ ] MarkAmpApp::OnInit() logs timing for each initialization phase
- [ ] `StartupCompleteEvent` is published with total startup duration
- [ ] `test_startup_timer` passes with all assertions
- [ ] All existing tests that use Profiler still compile and pass

---

## Testing Strategy

- Unit test for StartupTimer: checkpoint recording, JSON output, delta correctness
- Integration verification: build and run app, check log output for startup timing
- Regression: ensure all existing Profiler-dependent tests still pass
