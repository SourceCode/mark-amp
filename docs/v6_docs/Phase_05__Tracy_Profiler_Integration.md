# Phase 05: Tracy Profiler Integration

## Metadata

| Field | Value |
|---|---|
| Phase ID | 05 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 7 modified |
| PRD Sections | 6.2 Profiling Stack |

---

## Objective

Integrate Tracy profiler for real-time frame analysis, gated behind a compile-time flag. Tracy provides zero-cost instrumentation when disabled and rich real-time visualization when enabled.

---

## Background

The PRD recommends Tracy for frame analysis and Perfetto for deep tracing. Tracy is particularly valuable for identifying frame-time spikes, lock contention, and allocation patterns in real time. The integration must be zero-cost when disabled (no-op macros) and provide comprehensive instrumentation when enabled.

---

## Scope

### Tasks

1. **Add `tracy` to `vcpkg.json`**:
   - Add Tracy as a dependency
   - Specify version constraints if needed

2. **Create `cmake/Tracy.cmake` module**:
   - `MARKAMP_ENABLE_TRACY` option (OFF by default)
   - When enabled: find and link `tracy::TracyClient`
   - Define `MARKAMP_TRACY_ENABLED` compile definition
   - When disabled: define nothing (macros will no-op)

3. **Create `src/core/TracyIntegration.h`**:
   - When `MARKAMP_TRACY_ENABLED` is defined:
     - `MARKAMP_TRACY_ZONE(name)` -> `ZoneScopedN(name)`
     - `MARKAMP_TRACY_ZONE_COLOR(name, color)` -> `ZoneScopedNC(name, color)`
     - `MARKAMP_TRACY_FRAME_MARK` -> `FrameMark`
     - `MARKAMP_TRACY_PLOT(name, value)` -> `TracyPlot(name, value)`
     - `MARKAMP_TRACY_ALLOC(ptr, size)` -> `TracyAlloc(ptr, size)`
     - `MARKAMP_TRACY_FREE(ptr)` -> `TracyFree(ptr)`
     - `MARKAMP_TRACY_MESSAGE(text, len)` -> `TracyMessage(text, len)`
   - When disabled: all macros expand to nothing (zero cost)

4. **Insert Tracy zone markers in critical paths**:
   - `MarkAmpApp::OnInit()` — each initialization phase
   - `MarkAmpApp::OnIdle()` — idle processing loop
   - `EventBus::publish()` — event dispatch
   - `EventBus::publish_fast()` — fast event dispatch
   - Frame mark at end of each paint cycle

5. **Update `profile` preset** (from Phase 04):
   - Set `MARKAMP_ENABLE_TRACY=ON`
   - Ensure Tracy client is linked

6. **Create test: `test_tracy_integration.cpp`**:
   - Compile-only validation that macros expand correctly in both modes
   - Verify zone markers compile to no-ops when disabled
   - Verify Tracy types are available when enabled

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `vcpkg.json` |
| Create | `cmake/Tracy.cmake` |
| Create | `src/core/TracyIntegration.h` |
| Modify | `CMakeLists.txt` |
| Modify | `cmake/Dependencies.cmake` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/core/EventBus.h` |
| Create | `tests/unit/test_tracy_integration.cpp` |
| Modify | `tests/CMakeLists.txt` |
| Modify | `CMakePresets.json` |

---

## Implementation Notes

- Tracy uses a client-server model: the instrumented app (client) sends data to the Tracy profiler GUI (server). The client library must be linked.
- Include `tracy/Tracy.hpp` only inside `TracyIntegration.h` to avoid polluting the global namespace.
- Tracy macros are designed to be zero-cost when `TRACY_ENABLE` is not defined. Our `MARKAMP_TRACY_*` macros add a project-specific namespace.
- The `FrameMark` call should be placed at the end of each logical frame (idle handler completion or paint completion).
- For memory tracking, Tracy can intercept `new`/`delete` — but this should only be enabled for profiling builds.

---

## Acceptance Criteria

- [ ] Debug build compiles without Tracy (no-op macros, no linker errors)
- [ ] Profile build links Tracy client and produces zone data
- [ ] `MARKAMP_TRACY_ZONE` compiles to zero-cost when disabled
- [ ] `test_tracy_integration` compiles and passes under both Tracy-on and Tracy-off modes
- [ ] Tracy zone markers visible in `MarkAmpApp::OnInit()`, `EventBus::publish()`, `EventBus::publish_fast()`
- [ ] `MARKAMP_TRACY_FRAME_MARK` is called once per logical frame
- [ ] No build warnings from Tracy integration

---

## Testing Strategy

- Build with `debug` preset (Tracy off): verify compilation, no linker errors
- Build with `profile` preset (Tracy on): verify compilation, verify Tracy client linked
- Run test suite under both configurations
- Optionally connect Tracy profiler GUI to a running profile build to verify data flow
