# Phase 12: Lazy Service Construction & Startup Deferral

## Metadata

| Field | Value |
|---|---|
| Phase ID | 12 |
| Prerequisites | Phase 03 |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 6 modified |
| PRD Sections | 3.2 Lazy Initialization Everywhere |

---

## Objective

Convert the 21 eagerly-constructed extension services in `MarkAmpApp` to lazy-initialized, demand-constructed instances using a `LazyService<T>` wrapper. This is the single highest-impact optimization for startup time.

---

## Background

`MarkAmpApp::OnInit()` creates all 21 extension API services synchronously before showing the window. Services like `context_key_service`, `output_channel_service`, `file_decoration_service`, `tree_data_service`, `webview_service`, `walkthrough_service`, etc. are constructed eagerly but may never be used in a given session. Each construction involves heap allocations, EventBus subscriptions, and internal state setup.

The PRD mandates: "Defer ThemeRegistry loading, Extension scanning, Plugin activation, Font glyph caching, Grammar engine initialization, Marketplace HTTP setup" and "On-demand initialization using std::optional, std::unique_ptr, function-scope static initialization."

---

## Scope

### Tasks

1. **Create `src/core/LazyService.h`**:
   - Template class: `LazyService<T>`
   - Thread-safe lazy construction using `std::call_once`
   - `get_or_create()` returns `T*` — constructs on first call
   - Construction arguments captured via `std::tuple` or factory lambda
   - `is_created()` query (no construction side-effect)
   - `reset()` for test teardown
   - Zero overhead after first construction (just a pointer dereference + flag check)

2. **Refactor `MarkAmpApp` to use `LazyService`**:
   - Convert all 21 extension API services from eager to lazy construction:
     - `context_key_service_`, `output_channel_service_`, `extension_telemetry_service_`, `file_decoration_service_`, `tree_data_service_`, `webview_service_`, `walkthrough_service_`, `workspace_storage_service_`, `global_storage_service_`, `language_service_`, `file_system_provider_service_`, `scm_service_`, `terminal_service_`, `debug_service_`, `comments_service_`, `notebook_service_`, `testing_service_`, `task_provider_service_`, `timeline_service_`, `authentication_service_`, `localization_service_`
   - Also defer: `MermaidRenderer`, `MathRenderer`, `RecentWorkspaces`
   - **Keep eagerly initialized**: `EventBus`, `Config`, `AppStateManager`, `ThemeEngine`, `PluginManager` (needed before first frame)

3. **Update `PluginContext` to support lazy resolution**:
   - `PluginContext` holds `LazyService<T>*` pointers instead of `T*`
   - Accessor methods call `get_or_create()` transparently
   - Plugin code is unaware of lazy construction
   - Check pointer validity before dereferencing (per CLAUDE.md convention)

4. **Create `tests/unit/test_lazy_service.cpp`**:
   - Test lazy construction: service not created until `get_or_create()` called
   - Test thread safety: multiple threads calling `get_or_create()` concurrently
   - Test `is_created()` returns correct state
   - Test with various service types

5. **Create `benchmarks/bench_lazy_startup.cpp`**:
   - Measure startup time with eager vs lazy construction
   - Measure first-access latency for lazy services
   - Measure total time when all services eventually needed vs subset

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/LazyService.h` |
| Modify | `src/app/MarkAmpApp.h` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/core/PluginContext.h` |
| Create | `tests/unit/test_lazy_service.cpp` |
| Create | `benchmarks/bench_lazy_startup.cpp` |
| Modify | `tests/CMakeLists.txt` |
| Modify | `benchmarks/CMakeLists.txt` |

---

## Implementation Notes

- `std::call_once` with `std::once_flag` provides thread-safe lazy initialization with minimal overhead after first call.
- The factory lambda pattern allows capturing constructor arguments without requiring default constructibility:
  ```cpp
  LazyService<OutputChannelService> output_channel_{[this]() {
      return std::make_unique<OutputChannelService>(event_bus_.get());
  }};
  ```
- PluginContext changes must be backwards-compatible — plugin code should not need to change.
- Order of lazy construction matters: if service A's constructor depends on service B, B must be constructed first. The `get_or_create()` approach handles this naturally (B is constructed when A's constructor calls B's accessor).
- Be careful of circular dependencies between lazy services.

---

## Acceptance Criteria

- [ ] Services are constructed on first access, not at startup
- [ ] `bench_lazy_startup` shows measurable startup time reduction (target: >30% reduction in service construction time)
- [ ] PluginContext still provides valid pointers when accessed
- [ ] All existing tests pass (services still work when eventually constructed)
- [ ] No null pointer dereferences in test suite
- [ ] `test_lazy_service` validates thread safety and lazy construction
- [ ] `is_created()` correctly reports construction state
- [ ] Startup timer shows reduced time in "services_created" checkpoint

---

## Testing Strategy

- Run test_lazy_service for unit-level validation
- Run full test suite to verify no regressions from lazy construction
- Run bench_lazy_startup to measure improvement
- Manually test application startup and verify core functionality works
