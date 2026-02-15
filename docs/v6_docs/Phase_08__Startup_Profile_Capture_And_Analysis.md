# Phase 08: Startup Profile Capture & Analysis

## Metadata

| Field | Value |
|---|---|
| Phase ID | 08 |
| Prerequisites | Phase 05 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 2 modified |
| PRD Sections | 3.1-3.5 Startup Optimization, 7.3 Startup Budget |

---

## Objective

Use Tracy instrumentation from Phase 05 to capture a detailed startup profile, identify the top 5 time sinks, and document findings. Create a startup benchmark that measures each initialization phase independently.

---

## Background

The PRD's startup targets are cold start <150ms, warm start <50ms. Before optimizing, we must measure. This phase adds fine-grained instrumentation to every startup sub-phase and captures baseline data to guide subsequent optimization phases (12, 13).

---

## Scope

### Tasks

1. **Create `src/core/StartupPhase.h`**:
   - `enum class StartupPhase : uint8_t` listing all startup phases:
     - `LoggerInit`, `ConfigLoad`, `RecentWorkspaces`, `AppStateManager`, `CommandHistory`, `PlatformCreation`, `ThemeRegistryScan`, `ThemeEngineInit`, `ExtensionServiceCreation` (x21 individual services), `PluginManagerActivation`, `MermaidRendererInit`, `MathRendererInit`, `MainFrameCreation`, `FirstShow`, `FirstFrameRendered`
   - `constexpr` array of string names for each phase
   - Helper: `phase_name(StartupPhase) -> std::string_view`

2. **Instrument `MarkAmpApp::OnInit()` with fine-grained Tracy zones**:
   - Add `MARKAMP_TRACY_ZONE` at the start of each identifiable sub-phase
   - Use `StartupTimer::checkpoint()` (from Phase 03) at each boundary
   - Each of the 21 extension service constructions should be individually timed
   - Font cache initialization, grammar engine init — all individually timed

3. **Create startup benchmark: `benchmarks/bench_startup.cpp`**:
   - Constructs core services in isolation (no wxWidgets GUI)
   - Measures each phase independently:
     - Config load time
     - ThemeRegistry scan time (filesystem)
     - PluginManager activation time
     - EventBus construction + initial subscriber setup
   - Reports per-phase timings via Google Benchmark

4. **Run and capture baseline startup timings**:
   - Execute the benchmark and record results
   - Document findings

5. **Document findings in `docs/v6_docs/startup_profile.md`**:
   - Top 5 time sinks with measured durations
   - Breakdown of time spent in each startup phase
   - Identification of optimization targets for Phases 12-13
   - Comparison against 150ms target

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/StartupPhase.h` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Create | `benchmarks/bench_startup.cpp` |
| Create | `docs/v6_docs/startup_profile.md` |
| Modify | `benchmarks/CMakeLists.txt` |

---

## Implementation Notes

- The startup benchmark cannot easily test the full wxWidgets initialization path without a running GUI. Focus on the non-GUI portions: Config loading, ThemeRegistry scanning, service construction, PluginManager activation.
- Use `StartupTimer` (Phase 03) for the actual app instrumentation and Google Benchmark for the isolated benchmarks.
- The startup_profile.md will be updated as optimization phases execute — this is the initial baseline.
- Consider separating "time in constructors" vs "time in I/O" for each service.

---

## Acceptance Criteria

- [ ] Every startup sub-phase has a named Tracy zone
- [ ] Every startup sub-phase has a StartupTimer checkpoint
- [ ] `bench_startup` produces per-phase timings
- [ ] `startup_profile.md` documents top 5 time sinks with measured durations
- [ ] All 21 extension services are individually timed
- [ ] No functional regression (all tests pass)
- [ ] StartupPhase enum covers all phases in OnInit()

---

## Testing Strategy

- Run bench_startup and verify output
- Run the application with StartupTimer logging enabled, verify log output
- Compare benchmark results against expected ranges
- Verify startup_profile.md content is complete
