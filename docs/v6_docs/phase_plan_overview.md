# MarkAmp v6 Performance & Stability -- 40-Phase Implementation Plan

## Document Purpose

This document defines 40 implementation phases for the MarkAmp v6 Performance & Stability
initiative. Each phase is sized for execution by a single AI coding agent session
(typically 3-12 new/modified files, 200-800 lines of production code, plus tests).

Phases are strictly ordered by dependency. No phase references code that does not
yet exist unless the prerequisite phase is listed.

---

## PRD Gap Analysis

Before defining phases, the following gaps and concerns in the v6 PRD were identified
and are addressed within the plan.

### Gaps Identified

1. **No custom allocator (mimalloc/jemalloc) integration** -- The PRD mentions heap
   profiling with mimalloc/jemalloc but never specifies replacing the default allocator.
   For a desktop app targeting <150MB idle memory and <150ms cold start, a custom global
   allocator is a high-impact, low-effort win. Phase 10 addresses this.

2. **No startup timestamp instrumentation** -- The PRD says "measured via timestamp at
   main() / timestamp at first frame rendered" but gives no implementation detail for
   inserting these probes. Phase 3 addresses this.

3. **No clang-tidy configuration file (.clang-tidy)** -- The PRD mandates clang-tidy as
   a CI gate but the project has no .clang-tidy file. Phase 2 addresses this.

4. **Missing LSan preset** -- Sanitizers.cmake supports ASan, UBSan, TSan but not
   standalone LeakSanitizer. Phase 1 addresses this.

5. **No LTO CMake preset** -- The PRD requires a "Release + LTO" build configuration.
   CMakePresets.json only has debug, release, release-static. Phase 4 addresses this.

6. **No Tracy / Perfetto integration** -- The PRD recommends Tracy and Perfetto but
   no vcpkg dependency or integration exists. Phase 5 addresses this.

7. **No Google Benchmark dependency** -- The PRD mandates `markamp_bench` but Google
   Benchmark is not in vcpkg.json. Phase 6 addresses this.

8. **EventBus publish_fast still acquires mutex** -- Known from prior review. The
   `fast_lookup_mutex_` in `publish_fast()` defeats the "lock-free" claim. Phase 20
   addresses this with a flat subscriber array keyed by type_index hash.

9. **No signal/crash handler infrastructure** -- The PRD mentions crash dump generation
   and backtrace capture but no implementation path. Phase 30 addresses this.

10. **No CI benchmark baseline mechanism** -- The PRD says "compare against baseline JSON"
    but provides no schema or tooling. Phase 7 addresses this.

11. **No watchdog thread specification** -- PRD section 5.1 describes UI stall detection
    but gives no design for the watchdog thread. Phase 31 addresses this.

12. **SnapshotStore uses mutex, not atomic shared_ptr** -- DocumentSnapshot.h uses
    `std::lock_guard` where `std::atomic<std::shared_ptr<>>` (C++20) would be lock-free
    for the reader. Phase 21 addresses this.

13. **Profiler is a singleton** -- Violates the project's constructor injection convention.
    Phase 3 refactors to an injectable instance.

14. **No per-extension resource tracking** -- PRD section 9.2 requires it but no data
    model or collection mechanism exists. Phase 33 addresses this.

15. **No large file mode triggers** -- PRD section 9.1 specifies automatic activation
    but gives no threshold or implementation. Phase 35 addresses this.

16. **No structured logging (JSON)** -- Logger.h wraps spdlog text logging. PRD section
    4.1 requires structured JSON. Phase 29 addresses this.

17. **Packaging scripts are stubs** -- `packaging/` contains Info.plist, .desktop, .nsi,
    AppxManifest.xml but no CPack integration. Phase 38 addresses this.

18. **No fuzz testing harnesses** -- PRD section 6.5 mandates libFuzzer targets. Phase
    36 addresses this.

### Architectural Risks

1. **21 services eagerly constructed at startup** -- MarkAmpApp::OnInit() creates 21
   extension API services synchronously before showing the window. This is the single
   biggest startup time hazard. Phase 12 converts to lazy construction.

2. **ThemeRegistry::initialize() blocks startup** -- Scans filesystem for theme files.
   Phase 13 defers to background.

3. **Profiler::timings_ uses std::unordered_map<std::string, ...>** -- Hash map with
   heap-allocated keys on every record(). Phase 16 replaces with fixed-ID approach.

4. **EventBus handlers_ uses std::unordered_map** -- Hash map with type_index keys,
   shared_ptr values. Each publish() does a map lookup + atomic load. Phase 20 replaces
   with flat array.

5. **No build-time dead code detection** -- The PRD mentions dead code elimination but
   the project has no mechanism to detect unused translation units. Phase 15 adds
   link-map analysis.

---

## Phase Dependency Graph (Summarized)

```
Phase 01 --> Phase 02 --> Phase 03
Phase 03 --> Phase 04, Phase 05, Phase 06, Phase 07
Phase 05 --> Phase 08
Phase 06 --> Phase 09
Phase 03 --> Phase 10, Phase 11, Phase 12, Phase 13
Phase 10 --> Phase 14
Phase 11 --> Phase 15
Phase 12 --> Phase 16
Phase 08 --> Phase 17
Phase 09 --> Phase 18
Phase 14 --> Phase 19
Phase 16 --> Phase 20
Phase 20 --> Phase 21, Phase 22, Phase 23
Phase 17 --> Phase 24
Phase 18 --> Phase 25
Phase 21 --> Phase 26
Phase 24 --> Phase 27
Phase 22 --> Phase 28
Phase 29: standalone (requires Phase 03)
Phase 30: standalone (requires Phase 03)
Phase 31: standalone (requires Phase 05)
Phase 32: standalone (requires Phase 09)
Phase 33: standalone (requires Phase 05, Phase 12)
Phase 34: standalone (requires Phase 07)
Phase 35: standalone (requires Phase 03)
Phase 36: standalone (requires Phase 06)
Phase 37: standalone (requires Phase 09, Phase 36)
Phase 38: standalone (requires Phase 04)
Phase 39: standalone (requires Phase 07, Phase 09)
Phase 40: integration (requires Phase 34, Phase 37, Phase 39)
```

---

## Progression Summary

| Range     | Theme                                    |
|-----------|------------------------------------------|
| 01-02     | Build infrastructure & static analysis   |
| 03-04     | Measurement instrumentation & presets    |
| 05-07     | Profiling & benchmark tooling            |
| 08-09     | Profile-guided baseline capture          |
| 10-11     | Memory allocator & compile-time opts     |
| 12-13     | Startup deferral & lazy initialization   |
| 14-15     | Arena expansion & dead code elimination  |
| 16-17     | Profiler & hot path data structures      |
| 18-19     | Rendering performance & frame budgets    |
| 20-22     | EventBus evolution & lock-free paths     |
| 23-25     | Concurrency hardening & snapshot store   |
| 26-28     | Branch elimination & compile-time tables |
| 29-30     | Structured logging & crash reporting     |
| 31-32     | Watchdog & health monitoring             |
| 33-34     | Extension tracking & CI enforcement      |
| 35-36     | Large file mode & fuzz testing           |
| 37        | Chaos testing framework                  |
| 38        | Packaging & distribution                 |
| 39-40     | Load testing & final validation          |

---

## Phase Definitions

Each phase includes:
- **Title**: Short descriptive name
- **Prerequisite phases**: Hard dependencies
- **Objective**: What this phase accomplishes
- **Scope**: Specific deliverables
- **Key files created/modified**: Estimated file inventory
- **Acceptance criteria**: Concrete pass/fail conditions
- **Estimated file count**: New + modified files
- **Estimated complexity**: Low / Medium / High

---

### Phase 01: Sanitizer Preset Expansion & LeakSanitizer

**Prerequisites**: None

**Objective**: Ensure all four sanitizers (ASan, UBSan, TSan, LSan) are available as
CMake options and can be combined in CI matrix builds.

**Scope**:
- Add `MARKAMP_ENABLE_LSAN` option to CMakeLists.txt
- Update `cmake/Sanitizers.cmake` to support `-fsanitize=leak` standalone
- Add CMakePresets for sanitizer configurations:
  - `debug-asan` (ASan + UBSan)
  - `debug-tsan` (TSan)
  - `debug-lsan` (LSan standalone)
- Validate mutual exclusivity constraints (TSan vs ASan/UBSan)
- Add sanitizer presets to CMakePresets.json

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/cmake/Sanitizers.cmake`
- Modified: `/Users/ryanrentfro/code/markamp/CMakePresets.json`

**Acceptance criteria**:
- `cmake --preset debug-asan` configures without error
- `cmake --preset debug-tsan` configures without error
- `cmake --preset debug-lsan` configures without error
- Attempting ASan+TSan together produces FATAL_ERROR
- All existing tests pass under debug-asan preset

**Estimated file count**: 3 modified
**Estimated complexity**: Low

---

### Phase 02: Static Analysis Configuration (clang-tidy + cppcheck hardening)

**Prerequisites**: Phase 01

**Objective**: Create a project-wide `.clang-tidy` configuration file and a CI-gate
clang-tidy CMake integration that fails on new warnings.

**Scope**:
- Create `.clang-tidy` configuration at project root with checks:
  - `bugprone-*`, `clang-analyzer-*`, `cppcoreguidelines-*`, `modernize-*`,
    `performance-*`, `readability-*`
  - Disable checks incompatible with C++23 or wxWidgets macros
- Create `cmake/ClangTidy.cmake` module:
  - `MARKAMP_ENABLE_CLANG_TIDY` option
  - Sets `CMAKE_CXX_CLANG_TIDY` when enabled
- Add `debug-clang-tidy` preset to CMakePresets.json
- Update `cmake/CppCheck.cmake` to use `--std=c++23` if cppcheck supports it
- Create `cppcheck-suppressions.txt` if missing

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/.clang-tidy`
- Created: `/Users/ryanrentfro/code/markamp/cmake/ClangTidy.cmake`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/cmake/CppCheck.cmake`
- Modified: `/Users/ryanrentfro/code/markamp/CMakePresets.json`

**Acceptance criteria**:
- `cmake --preset debug-clang-tidy` configures and builds
- clang-tidy runs on compilation and reports findings
- No new clang-tidy errors in core/EventBus.h (baseline clean)
- `.clang-tidy` file is version-controlled

**Estimated file count**: 2 created, 3 modified
**Estimated complexity**: Medium

---

### Phase 03: Startup Timing Instrumentation & Profiler Refactor

**Prerequisites**: Phase 02

**Objective**: Insert precise timing probes at every startup phase and refactor the
Profiler from a singleton to an injectable instance (per CLAUDE.md convention).

**Scope**:
- Refactor `Profiler` class:
  - Remove `static instance()` singleton
  - Make constructible, movable
  - Accept pointer in PluginContext for extension access
- Create `StartupTimer` utility class:
  - Records timestamps for: main() entry, wxApp::OnInit() start, config loaded,
    theme initialized, services created, plugins activated, first frame rendered
  - Produces JSON report of startup phases
- Instrument `MarkAmpApp::OnInit()` with StartupTimer checkpoints
- Add `AppState` field or event for first-frame-rendered timestamp
- Create test: `test_startup_timer.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Profiler.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Profiler.cpp`
- Created: `/Users/ryanrentfro/code/markamp/src/core/StartupTimer.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/StartupTimer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_startup_timer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Profiler no longer uses `static instance()`
- StartupTimer produces valid JSON with all checkpoint names
- MarkAmpApp::OnInit() logs timing for each phase
- `MARKAMP_PROFILE_SCOPE` macro updated for non-singleton usage
- test_startup_timer passes

**Estimated file count**: 4 created, 5 modified
**Estimated complexity**: Medium

---

### Phase 04: LTO Build Presets & Release Optimization Flags

**Prerequisites**: Phase 03

**Objective**: Add Release+LTO and Profile build configurations as specified in PRD
section 9.

**Scope**:
- Add `release-lto` preset to CMakePresets.json:
  - `CMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE`
  - Platform-specific LTO flags (ThinLTO for Clang, -flto for GCC, /GL for MSVC)
- Add `profile` preset:
  - Release optimizations with debug info (`-O2 -g`)
  - Frame pointer preserved (`-fno-omit-frame-pointer`)
  - Tracy-compatible defines (placeholder for Phase 05)
- Create `cmake/LTO.cmake` module:
  - Auto-detect ThinLTO support
  - Fallback to regular LTO
  - Set linker flags (lld preferred)
- Update root CMakeLists.txt to include LTO.cmake

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/cmake/LTO.cmake`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/CMakePresets.json`

**Acceptance criteria**:
- `cmake --preset release-lto` configures and builds to completion
- `cmake --preset profile` configures and builds to completion
- LTO build produces a smaller binary than regular Release (verified by file size)
- Profile build includes debug symbols (verified by `file` or `dwarfdump`)

**Estimated file count**: 1 created, 2 modified
**Estimated complexity**: Medium

---

### Phase 05: Tracy Profiler Integration

**Prerequisites**: Phase 03

**Objective**: Integrate Tracy profiler for real-time frame analysis, gated behind a
compile-time flag.

**Scope**:
- Add `tracy` to vcpkg.json dependencies
- Create `cmake/Tracy.cmake` module:
  - `MARKAMP_ENABLE_TRACY` option (OFF by default)
  - Links tracy::TracyClient when enabled
  - Defines `MARKAMP_TRACY_ENABLED`
- Create `src/core/TracyIntegration.h`:
  - When MARKAMP_TRACY_ENABLED: `MARKAMP_TRACY_ZONE(name)` maps to `ZoneScoped`
  - When disabled: no-op macros
  - `MARKAMP_TRACY_FRAME_MARK` maps to `FrameMark`
  - `MARKAMP_TRACY_PLOT(name, value)` for value tracking
- Insert Tracy zone markers in:
  - `MarkAmpApp::OnInit()`
  - `MarkAmpApp::OnIdle()`
  - `EventBus::publish()`
  - `EventBus::publish_fast()`
- Update `profile` preset to enable Tracy
- Create test: `test_tracy_integration.cpp` (compile-only validation)

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/vcpkg.json`
- Created: `/Users/ryanrentfro/code/markamp/cmake/Tracy.cmake`
- Created: `/Users/ryanrentfro/code/markamp/src/core/TracyIntegration.h`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/cmake/Dependencies.cmake`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_tracy_integration.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/CMakePresets.json`

**Acceptance criteria**:
- Debug build compiles without Tracy (no-op macros)
- Profile build links Tracy and produces zone data
- `MARKAMP_TRACY_ZONE` compiles to zero-cost when disabled
- test_tracy_integration compiles and passes under both modes

**Estimated file count**: 3 created, 7 modified
**Estimated complexity**: Medium

---

### Phase 06: Google Benchmark Integration & Initial Benchmarks

**Prerequisites**: Phase 03

**Objective**: Add Google Benchmark as a dependency, create the `markamp_bench`
executable, and write the first benchmark suite for critical hot paths.

**Scope**:
- Add `benchmark` to vcpkg.json
- Create `benchmarks/` directory structure
- Create `benchmarks/CMakeLists.txt`:
  - `markamp_bench` executable
  - Links Google Benchmark + markamp_core
- Create initial benchmarks:
  - `bench_eventbus.cpp`: publish vs publish_fast throughput
  - `bench_profiler.cpp`: record() throughput, scope() overhead
  - `bench_frame_arena.cpp`: arena alloc/reset cycle vs heap
  - `bench_spsc_queue.cpp`: push/pop throughput
  - `bench_piece_table.cpp`: insert/delete operations
- Update root CMakeLists.txt with `MARKAMP_BUILD_BENCHMARKS` option

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/vcpkg.json`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_eventbus.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_profiler.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_frame_arena.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_spsc_queue.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_piece_table.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/cmake/Dependencies.cmake`

**Acceptance criteria**:
- `markamp_bench` builds and runs
- Each benchmark produces valid Google Benchmark JSON output
- EventBus publish benchmark shows >100K ops/sec
- FrameArena benchmark shows >5x throughput vs raw new/delete
- All benchmarks complete in <30 seconds total

**Estimated file count**: 6 created, 3 modified
**Estimated complexity**: Medium

---

### Phase 07: CI Benchmark Baseline & Regression Detection

**Prerequisites**: Phase 03, Phase 06

**Objective**: Create the benchmark regression detection infrastructure that CI can
use to compare against stored baselines.

**Scope**:
- Create `benchmarks/baseline/` directory for JSON baselines
- Create `scripts/benchmark_compare.py`:
  - Loads current benchmark JSON output
  - Compares against baseline JSON
  - Flags regressions >5% (configurable threshold)
  - Exits non-zero on regression
  - Produces human-readable diff report
- Create `scripts/benchmark_update_baseline.sh`:
  - Runs benchmarks and saves output as new baseline
- Create baseline schema definition (`benchmarks/baseline/schema.json`)
- Create initial baseline by running benchmarks
- Document CI integration in a comment block (actual CI config is out of scope)

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/baseline/schema.json`
- Created: `/Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py`
- Created: `/Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh`

**Acceptance criteria**:
- `benchmark_compare.py` correctly detects a simulated 10% regression
- `benchmark_compare.py` passes when results are within 5% of baseline
- Script outputs clear human-readable diff
- Exit code is 0 for pass, 1 for regression
- baseline schema validates actual benchmark output

**Estimated file count**: 3 created
**Estimated complexity**: Low

---

### Phase 08: Startup Profile Capture & Analysis

**Prerequisites**: Phase 05

**Objective**: Use Tracy instrumentation from Phase 05 to capture a detailed startup
profile, identify the top 5 time sinks, and document findings.

**Scope**:
- Add fine-grained Tracy zones to all MarkAmpApp::OnInit() phases:
  - Logger init, Config load, RecentWorkspaces, AppStateManager,
    CommandHistory, Platform creation, ThemeRegistry scan, ThemeEngine init,
    each of the 21 extension services, PluginManager activation, Mermaid/Math
    renderer init, MainFrame creation, first Show()
- Create `src/core/StartupPhase.h` enum listing all startup phases
- Create startup benchmark: `benchmarks/bench_startup.cpp`
  - Constructs core services in isolation (no wxWidgets GUI)
  - Measures each phase independently
- Run and capture baseline startup timings
- Document findings in `docs/v6_docs/startup_profile.md`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/StartupPhase.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_startup.cpp`
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/startup_profile.md`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`

**Acceptance criteria**:
- Every startup sub-phase has a named Tracy zone
- bench_startup produces per-phase timings
- startup_profile.md documents top 5 time sinks with measured durations
- No functional regression (all tests pass)

**Estimated file count**: 3 created, 2 modified
**Estimated complexity**: Medium

---

### Phase 09: Runtime Profile Baseline & Hot Path Identification

**Prerequisites**: Phase 06

**Objective**: Run benchmarks on core runtime operations, identify the top 10 hot paths,
and document findings to guide subsequent optimization phases.

**Scope**:
- Create runtime benchmarks:
  - `bench_html_renderer.cpp`: render small/medium/large markdown
  - `bench_syntax_highlighter.cpp`: incremental vs full tokenization
  - `bench_markdown_parser.cpp`: parse various document sizes
  - `bench_config.cpp`: get_string/get_int/get_bool throughput
  - `bench_theme_engine.cpp`: color lookup, brush/pen cache hit rate
- Run all benchmarks and record baseline
- Create `docs/v6_docs/runtime_profile.md` documenting:
  - Top 10 hot paths ranked by total CPU time
  - Cache miss analysis where measurable
  - Memory allocation frequency per operation

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_html_renderer.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_syntax_highlighter.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_markdown_parser.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_config.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_theme_engine.cpp`
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/runtime_profile.md`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`

**Acceptance criteria**:
- All 5 new benchmarks build and run successfully
- runtime_profile.md lists top 10 hot paths with measured timings
- Baseline JSON saved in benchmarks/baseline/
- Documentation identifies specific optimization targets

**Estimated file count**: 6 created, 1 modified
**Estimated complexity**: Medium

---

### Phase 10: Custom Allocator Integration (mimalloc)

**Prerequisites**: Phase 03

**Objective**: Replace the default C++ allocator with mimalloc for reduced fragmentation
and faster small allocations, gated behind a build option.

**Scope**:
- Add `mimalloc` to vcpkg.json
- Create `cmake/Allocator.cmake`:
  - `MARKAMP_USE_MIMALLOC` option (ON by default for Release)
  - Links mimalloc and sets `MARKAMP_CUSTOM_ALLOCATOR`
- Create `src/core/AllocatorConfig.h`:
  - Include mimalloc override header when enabled
  - Provide `allocator_name()`, `allocator_stats()` queries
- Verify mimalloc replaces global new/delete (write a test)
- Create benchmark: `bench_allocator.cpp`
  - Compare default vs mimalloc for small/medium/large allocations
  - Measure allocation-heavy workflows (EventBus publish storm, PieceTable edits)
- Create test: `test_allocator.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/vcpkg.json`
- Created: `/Users/ryanrentfro/code/markamp/cmake/Allocator.cmake`
- Created: `/Users/ryanrentfro/code/markamp/src/core/AllocatorConfig.h`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/cmake/Dependencies.cmake`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_allocator.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_allocator.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Build succeeds with and without mimalloc
- test_allocator confirms mimalloc is active when enabled
- bench_allocator shows measurable improvement (>10% for small allocs)
- No test regressions under mimalloc

**Estimated file count**: 3 created, 6 modified
**Estimated complexity**: Medium

---

### Phase 11: Compile-Time Optimization -- constexpr Tables & constinit Audit

**Prerequisites**: Phase 03

**Objective**: Eliminate runtime table construction by converting static lookup tables
to constexpr/constinit and auditing all translation units for non-trivial static init.

**Scope**:
- Audit all source files for static initialization patterns:
  - `static std::vector`, `static std::map`, `static std::string`
  - `static` objects with non-trivial constructors
- Convert identified tables to:
  - `constexpr std::array` for ordered lookups
  - `constinit` for file-scope state
  - Function-local statics where appropriate
- Priority targets (from code review):
  - Config.cpp hardcoded defaults (already moved to JSON -- verify)
  - BuiltInThemes.cpp theme definitions
  - SyntaxHighlighter keyword tables
  - Events.h string constants
- Create `scripts/audit_static_init.sh`:
  - Uses `nm` to find global constructors in object files
  - Reports functions with `__cxx_global_var_init`
- Create test: `test_constexpr_tables.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Created: `/Users/ryanrentfro/code/markamp/scripts/audit_static_init.sh`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_constexpr_tables.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- `audit_static_init.sh` reports zero non-trivial global constructors in core/
- All converted tables are constexpr or constinit
- test_constexpr_tables validates compile-time evaluation
- No functional regression

**Estimated file count**: 2 created, 4 modified
**Estimated complexity**: Medium

---

### Phase 12: Lazy Service Construction & Startup Deferral

**Prerequisites**: Phase 03

**Objective**: Convert the 21 eagerly-constructed extension services in MarkAmpApp to
lazy-initialized, demand-constructed instances using std::optional or std::unique_ptr
with deferred construction.

**Scope**:
- Create `src/core/LazyService.h`:
  - Template wrapper: `LazyService<T>` with `get_or_create()`
  - Thread-safe first-access construction using std::call_once
  - Optional construction arguments captured at declaration
- Refactor MarkAmpApp to use LazyService for:
  - All 21 extension API services (context_key, output_channel, etc.)
  - MermaidRenderer, MathRenderer
  - RecentWorkspaces
- Keep eagerly initialized: EventBus, Config, AppStateManager, ThemeEngine, PluginManager
  (these are needed before first frame)
- Update PluginContext to support lazy resolution
- Create test: `test_lazy_service.cpp`
- Create benchmark: `bench_lazy_startup.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/LazyService.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_lazy_service.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_lazy_startup.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`

**Acceptance criteria**:
- Services are constructed on first access, not at startup
- bench_lazy_startup shows measurable startup time reduction
- PluginContext still provides valid pointers when accessed
- All existing tests pass (services still work when eventually constructed)
- No null pointer dereferences in test suite

**Estimated file count**: 2 created, 6 modified
**Estimated complexity**: High

---

### Phase 13: Deferred Theme & Extension Loading

**Prerequisites**: Phase 03

**Objective**: Move ThemeRegistry filesystem scanning and extension scanning off the
startup critical path, using AsyncPipeline for background loading.

**Scope**:
- Refactor ThemeRegistry::initialize():
  - Split into `initialize_builtin()` (synchronous, fast) and
    `scan_user_themes()` (async, deferred)
  - Built-in themes loaded at startup (constexpr data from Phase 11)
  - User theme scanning via AsyncPipeline
  - ThemeEngine starts with built-in themes, updates when scan completes
- Refactor PluginManager::activate_all():
  - Split into `activate_builtin()` (synchronous) and
    `scan_and_activate_extensions()` (deferred)
  - Built-in plugins activated immediately
  - Extension scanning deferred to post-first-frame
- Add `StartupDeferralEvent` to Events.h for post-first-frame trigger
- Instrument with StartupTimer checkpoints
- Create test: `test_deferred_loading.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ThemeRegistry.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ThemeRegistry.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_deferred_loading.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Built-in themes available before first frame
- User theme scan completes in background without blocking UI
- Extension scanning starts after first frame rendered
- bench_startup shows reduced time-to-first-frame
- test_deferred_loading validates async completion

**Estimated file count**: 1 created, 7 modified
**Estimated complexity**: High

---

### Phase 14: Arena Allocator Expansion to Hot Paths

**Prerequisites**: Phase 10

**Objective**: Expand FrameArena usage from per-frame scratch space to cover the three
highest-allocation hot paths identified in Phase 09.

**Scope**:
- Expand FrameArena to EventBus:
  - Use arena-backed pmr::vector for handler snapshots during publish()
  - Avoid heap allocation per publish call
- Expand FrameArena to HtmlRenderer:
  - Use arena for temporary string concatenation during render pass
  - Use arena-backed containers for render batch
- Expand FrameArena to SyntaxHighlighter:
  - Use arena for Token vector during incremental tokenization
- Create `FrameArenaPool` class:
  - Maintains per-thread arena instances
  - Thread-local accessor for safe concurrent use
- Create benchmark: `bench_arena_hotpaths.cpp`
- Create test: `test_arena_expansion.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/FrameArena.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_arena_hotpaths.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_arena_expansion.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- EventBus publish() makes zero heap allocations for handler dispatch
- HtmlRenderer render pass uses arena for temporaries
- bench_arena_hotpaths shows >30% allocation reduction
- All existing tests pass
- No arena overflow (monotonic_buffer_resource falls back correctly)

**Estimated file count**: 2 created, 7 modified
**Estimated complexity**: High

---

### Phase 15: Dead Code Elimination & Translation Unit Splitting

**Prerequisites**: Phase 11

**Objective**: Identify and remove dead code, split oversized translation units, and
verify that unused plugin code is not linked.

**Scope**:
- Create `scripts/dead_code_report.sh`:
  - Analyzes link map from Release build
  - Identifies unreferenced symbols
  - Cross-references with source files
- Audit PluginContext.h includes:
  - Replace direct includes with forward declarations where possible
  - Reduce header dependency chain
- Split large translation units (>1000 lines):
  - Identify candidates via `wc -l`
  - Split into focused .cpp files
- Remove any dead code identified by the analysis
- Update source_group() in src/CMakeLists.txt to stay synchronized
- Verify `-fdata-sections -ffunction-sections` + `--gc-sections` linker flags

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/scripts/dead_code_report.sh`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/cmake/LTO.cmake`
- Various source files split or trimmed

**Acceptance criteria**:
- dead_code_report.sh produces report with zero false-negative known dead code
- PluginContext.h compiles with forward declarations only (fewer includes)
- No translation unit exceeds 1000 lines
- Binary size does not increase (ideally decreases)
- All tests pass

**Estimated file count**: 1 created, 4+ modified
**Estimated complexity**: Medium

---

### Phase 16: Profiler Data Structure Optimization

**Prerequisites**: Phase 12

**Objective**: Replace the Profiler's heap-allocating `unordered_map<string, ...>` with
a fixed-ID system that avoids heap allocation on the recording hot path.

**Scope**:
- Create `src/core/ProfilerIds.h`:
  - `enum class ProfileId : uint16_t` with named constants for all profiled zones
  - `constexpr` array of string names indexed by ProfileId
- Refactor Profiler internals:
  - Replace `unordered_map<string, TimingData>` with `array<TimingData, kMaxProfileIds>`
  - `record()` takes `ProfileId` instead of `string_view` (zero-alloc)
  - Keep string_view overload for dynamic/extension profiling (separate slow path)
- Update `MARKAMP_PROFILE_SCOPE` macro to use ProfileId
- Replace ring buffer for timing samples (capped circular buffer, no vector growth)
- Create benchmark: `bench_profiler_v2.cpp`
- Create test: `test_profiler_v2.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/ProfilerIds.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Profiler.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Profiler.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_profiler_v2.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_profiler_v2.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Profiler::record(ProfileId) makes zero heap allocations
- Ring buffer has configurable cap (default 1024 samples per zone)
- bench_profiler_v2 shows >5x throughput improvement over string-keyed version
- test_profiler_v2 validates all operations
- All existing MARKAMP_PROFILE_SCOPE sites still work

**Estimated file count**: 3 created, 4 modified
**Estimated complexity**: Medium

---

### Phase 17: Hot Path Branch Elimination & Strategy Resolution

**Prerequisites**: Phase 08

**Objective**: Eliminate runtime branching in inner loops by pre-resolving configuration
branches into function pointers / strategy objects at config-load time.

**Scope**:
- Audit hot paths for config-check branching patterns:
  - `if (config.feature_enabled)` in render loops
  - `if (config.show_minimap)` in layout calculations
  - `if (config.word_wrap)` in line measurement
- Create `src/core/ResolvedConfig.h`:
  - Pre-computed function pointers for configurable behavior
  - Rebuilt on Config change event
  - Example: `std::function<void(Line&)> measure_line` resolved once
- Apply `[[likely]]` / `[[unlikely]]` to remaining necessary branches
- Apply `MARKAMP_HOT` / `MARKAMP_COLD` attributes to identified functions
- Create benchmark: `bench_branch_elimination.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/ResolvedConfig.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/ResolvedConfig.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Config.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_branch_elimination.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance criteria**:
- Zero config-check branches remain in identified hot loops
- ResolvedConfig updates automatically on ConfigChangedEvent
- bench_branch_elimination shows measurable improvement
- `[[likely]]`/`[[unlikely]]` applied to all error-path branches in hot code
- All tests pass

**Estimated file count**: 3 created, 5 modified
**Estimated complexity**: Medium

---

### Phase 18: Rendering Pipeline Optimization -- Damage Rects & ScrollBlit

**Prerequisites**: Phase 09

**Objective**: Wire the existing DirtyRegionAccumulator and ScrollBlit infrastructure
into the actual rendering pipeline to eliminate full-viewport repaints.

**Scope**:
- Integrate DirtyRegionAccumulator into EditorPanel/PreviewPanel:
  - On edit: invalidate only affected line range
  - On scroll: use ScrollBlit to shift content, paint only revealed strip
  - On theme change: invalidate all (full repaint)
- Integrate IncrementalLineWrap.h:
  - Re-wrap only changed lines, not entire document
- Wire ViewportCache into the paint path:
  - Only render lines in visible range + prefetch margin
  - Skip lines outside viewport entirely
- Ensure DoubleBufferedPaint.h is used for all paint operations
- Create benchmark: `bench_rendering_pipeline.cpp`
- Create test: `test_incremental_rendering.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_rendering_pipeline.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_incremental_rendering.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Single-character edit does NOT repaint entire viewport
- Scroll operations use blit + strip paint (verified by dirty region count)
- bench_rendering_pipeline shows >50% frame time reduction for small edits
- test_incremental_rendering validates damage rect correctness
- No visual artifacts (manual verification)

**Estimated file count**: 2 created, 5 modified
**Estimated complexity**: High

---

### Phase 19: Unbounded Container Caps & Memory Budget Enforcement

**Prerequisites**: Phase 14

**Objective**: Cap all unbounded-growth containers identified in the PRD and implement
runtime memory budget monitoring.

**Scope**:
- Cap the following containers:
  - EventBus queued_events_ (max 4096, drop oldest)
  - EventBus fast_queue_ (already capped at 1024, verify)
  - OutputChannelService logs (max 10000 lines per channel)
  - NotificationService queue (max 100 pending)
  - ExtensionTelemetry buffers (max 1000 entries per extension)
  - Profiler timing samples (already addressed in Phase 16, verify)
- Create `src/core/BoundedContainer.h`:
  - `BoundedQueue<T, MaxSize>`: ring buffer semantics, overwrites oldest
  - `BoundedVector<T, MaxSize>`: vector with hard cap, rejects on full
- Create `src/core/MemoryBudget.h`:
  - Track total memory usage approximation
  - Compare against budget (150MB idle, 2x file size for large files)
  - Publish MemoryPressureEvent when approaching limit
- Create test: `test_bounded_containers.cpp`
- Create test: `test_memory_budget.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/BoundedContainer.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/MemoryBudget.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/MemoryBudget.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_bounded_containers.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_memory_budget.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- No container grows without bound
- BoundedQueue correctly overwrites oldest when full
- MemoryBudget publishes event when exceeding 80% of budget
- All tests pass including bounded container edge cases
- No data loss for user-facing data (caps only apply to diagnostic/log buffers)

**Estimated file count**: 5 created, 6 modified
**Estimated complexity**: Medium

---

### Phase 20: EventBus Evolution -- Flat Subscriber Storage

**Prerequisites**: Phase 16

**Objective**: Replace the EventBus `unordered_map<type_index, shared_ptr<vector>>`
with a flat, cache-friendly subscriber array, eliminating the `fast_lookup_mutex_`.

**Scope**:
- Create `src/core/EventBusV2.h`:
  - Compile-time event type registration using type traits
  - Flat `std::array<SubscriberSlot, kMaxEventTypes>` indexed by event type ID
  - Each slot: atomic pointer to immutable subscriber list (COW preserved)
  - `publish_fast()` becomes truly lock-free (no mutex, no map lookup)
  - `subscribe()` still takes lock for COW list replacement
- Create `src/core/EventTypeId.h`:
  - Assigns sequential IDs to event types at compile time
  - Uses constexpr counter or template specialization
- Migrate EventBus.h to use new internals (same public API)
- Create benchmark: `bench_eventbus_v2.cpp`
- Create test: `test_eventbus_v2.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/EventTypeId.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_eventbus_v2.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_eventbus_v2.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- `publish_fast()` acquires zero locks
- No `unordered_map` lookup in hot publish path
- bench_eventbus_v2 shows >2x throughput for publish_fast
- All existing EventBus tests pass without API changes
- test_eventbus_v2 validates concurrent publish/subscribe safety

**Estimated file count**: 2 created, 6 modified
**Estimated complexity**: High

---

### Phase 21: Lock-Free SnapshotStore & Atomic Shared Pointer

**Prerequisites**: Phase 20

**Objective**: Replace the mutex-based SnapshotStore with C++20
`std::atomic<std::shared_ptr<>>` for truly lock-free reader access.

**Scope**:
- Refactor `DocumentSnapshot.h`:
  - Replace `mutable std::mutex` with `std::atomic<std::shared_ptr<DocumentSnapshot>>`
  - `current()` becomes lock-free atomic load
  - `publish()` uses atomic store (still serialized by writer thread)
- Audit all shared_ptr atomic operations in codebase:
  - EventBus COW already uses `std::atomic_load/store` (C++11 free functions)
  - Migrate to `std::atomic<std::shared_ptr<>>` (C++20 class template) for clarity
- Create test: `test_lockfree_snapshot.cpp`
- Create benchmark: `bench_snapshot_store.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/DocumentSnapshot.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_lockfree_snapshot.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_snapshot_store.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- SnapshotStore::current() contains no mutex lock
- EventBus uses C++20 atomic<shared_ptr> instead of free functions
- bench_snapshot_store shows improvement under contention
- test_lockfree_snapshot validates concurrent read/write correctness
- All existing snapshot-related tests pass

**Estimated file count**: 2 created, 4 modified
**Estimated complexity**: Medium

---

### Phase 22: Thread Safety Hardening -- Exception Boundaries

**Prerequisites**: Phase 20

**Objective**: Ensure all thread boundaries have typed exception handlers and that no
exception can escape a worker thread unhandled.

**Scope**:
- Audit all `std::thread` and `AsyncPipeline` workers for exception handling:
  - AsyncHighlighter
  - AsyncFileLoader
  - AsyncPipeline template
  - Any std::thread in extension services
- Create `src/core/ThreadBoundary.h`:
  - `thread_safe_invoke(callable)` wrapper that catches typed exceptions
  - Logs exception details with thread ID
  - Never uses `catch(...)`
  - Returns std::expected for error propagation
- Replace any bare `catch(...)` found in codebase with typed handlers
- Verify no `catch(...)` exists via grep (create CI check script)
- Create `scripts/check_catch_all.sh`
- Create test: `test_thread_boundary.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/ThreadBoundary.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/AsyncPipeline.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp`
- Created: `/Users/ryanrentfro/code/markamp/scripts/check_catch_all.sh`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_thread_boundary.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Zero `catch(...)` in entire codebase (verified by script)
- All worker threads have typed exception handlers at boundary
- ThreadBoundary wrapper logs and propagates exceptions correctly
- test_thread_boundary validates error handling behavior
- No unhandled exceptions cause std::terminate

**Estimated file count**: 3 created, 4 modified
**Estimated complexity**: Medium

---

### Phase 23: Data-Oriented Refactor -- Contiguous Subscriber Storage

**Prerequisites**: Phase 20

**Objective**: Convert event subscriber storage from vector-of-structs to
struct-of-arrays for better cache locality during publish iteration.

**Scope**:
- Refactor EventBus subscriber storage:
  - Current: `vector<HandlerEntry>` where HandlerEntry = {id, function}
  - New: Separate arrays for IDs and handlers
  - Handler array is contiguous for cache-line-friendly iteration
- Apply SoA pattern to PluginManager dependency graph:
  - Current: vector of Plugin objects with scattered fields
  - New: Contiguous arrays for plugin IDs, activation states, dependencies
- Apply SoA to StyleRunStore:
  - Current: vector of StyleRun structs
  - New: Separate arrays for start, length, style_id
- Create benchmark: `bench_data_oriented.cpp`
- Create test: `test_soa_storage.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/StyleRunStore.h`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_data_oriented.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_soa_storage.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- EventBus handler iteration is cache-line friendly (verified by benchmark)
- bench_data_oriented shows measurable improvement for iteration-heavy operations
- test_soa_storage validates correct storage/retrieval
- All existing EventBus and PluginManager tests pass
- No functional regression

**Estimated file count**: 2 created, 6 modified
**Estimated complexity**: Medium

---

### Phase 24: Plugin Isolation Hardening -- Timeout & Memory Caps

**Prerequisites**: Phase 17

**Objective**: Implement plugin call timeout detection and per-plugin soft memory caps
as specified in PRD section 5.2.

**Scope**:
- Create `src/core/PluginSafeCall.h`:
  - `safe_call(plugin_id, callable, timeout)` wrapper
  - Uses std::async with timeout for extension calls
  - Logs and reports slow plugin calls
  - Returns std::expected with timeout error
- Create `src/core/PluginMemoryTracker.h`:
  - Per-plugin allocation tracking (approximate, using arena tagging)
  - Configurable soft cap per plugin (default 50MB)
  - Warning logged when approaching cap
  - Hard kill when exceeding 2x soft cap
- Integrate into PluginManager::activate() and event dispatch
- Create PluginIsolationEvent for diagnostics reporting
- Create test: `test_plugin_isolation.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/PluginSafeCall.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/PluginMemoryTracker.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/PluginMemoryTracker.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_plugin_isolation.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Plugin call exceeding timeout (5s default) is terminated and logged
- Plugin exceeding memory soft cap generates warning
- Plugin exceeding 2x cap is forcefully deactivated
- test_plugin_isolation validates timeout and memory cap behavior
- Existing plugins are not affected (caps are generous defaults)

**Estimated file count**: 4 created, 5 modified
**Estimated complexity**: High

---

### Phase 25: UB Elimination Audit & Compiler Warning Hardening

**Prerequisites**: Phase 18

**Objective**: Systematic audit and elimination of undefined behavior, plus enabling
the full warning set specified in PRD section 5.3.

**Scope**:
- Add missing warnings to CompilerWarnings.cmake:
  - `-Wshadow` (currently missing)
  - `-Wold-style-cast`
  - `-Woverloaded-virtual`
  - `-Wnon-virtual-dtor`
- Audit for UB patterns:
  - `reinterpret_cast` usage (ObjectPool uses it -- verify safety)
  - Unchecked downcasts (static_cast of Event& in handler dispatch)
  - Raw pointer ownership ambiguity (document all raw pointer contracts)
  - Signed integer overflow (check for arithmetic on user-supplied sizes)
- Fix all identified UB:
  - Add `dynamic_cast` with check for downcasts in debug mode
  - Add arithmetic overflow guards
  - Document pointer ownership with comments
- Run full test suite under ASan+UBSan and fix all findings
- Create test: `test_ub_edge_cases.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/cmake/CompilerWarnings.cmake`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/FrameArena.h` (ObjectPool fix)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` (downcast safety)
- Various source files with UB fixes
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_ub_edge_cases.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Full test suite passes under ASan+UBSan with zero findings
- Zero `reinterpret_cast` on non-trivially-compatible types
- All downcasts are either compile-time safe or runtime-checked
- No `-Wshadow` or `-Wold-style-cast` warnings
- test_ub_edge_cases exercises boundary conditions

**Estimated file count**: 1 created, 5+ modified
**Estimated complexity**: Medium

---

### Phase 26: Compile-Time Lookup Tables -- constexpr Maps

**Prerequisites**: Phase 21

**Objective**: Replace remaining runtime-built lookup tables with constexpr arrays
and binary search, eliminating constructor-time allocation.

**Scope**:
- Create `src/core/ConstexprMap.h`:
  - `constexpr_lookup<Key, Value, N>` sorted array with binary search
  - Compile-time sorted construction
  - O(log N) lookup, zero runtime allocation
- Convert the following tables:
  - MIME type lookups in HtmlRenderer
  - Markdown syntax keywords in SyntaxHighlighter
  - Config key defaults in Config.cpp
  - Event name lookups
  - CSS property mappings in theme system
- Create test: `test_constexpr_map.cpp`
- Create benchmark: `bench_constexpr_lookup.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/ConstexprMap.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_constexpr_map.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_constexpr_lookup.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- All converted tables are constexpr (compile-time evaluated)
- bench_constexpr_lookup shows comparable or better performance vs unordered_map
- Zero runtime allocation for table construction
- test_constexpr_map validates lookup correctness
- All existing tests pass

**Estimated file count**: 3 created, 5 modified
**Estimated complexity**: Medium

---

### Phase 27: Frame Budget Enforcement & Graceful Degradation Wiring

**Prerequisites**: Phase 24

**Objective**: Wire the existing FrameBudgetToken and DegradationLevel infrastructure
into the actual UI rendering loop so subsystems bail early when budget is exhausted.

**Scope**:
- Integrate FrameBudgetToken into MainFrame/EditorPanel paint cycle:
  - Create token at frame start
  - Pass to all rendering subsystems
  - Each subsystem checks `token.is_exhausted()` before expensive work
- Implement degradation levels:
  - Full: all rendering features
  - Reduced: skip minimap, syntax highlighting for off-screen lines
  - Minimal: plain text rendering only, no overlays
- Wire AdaptiveThrottle into the idle handler:
  - Typing mode: 4ms budget
  - Idle mode: 16ms budget
- Integrate FrameScheduler priority queue:
  - Input tasks always run first
  - Paint tasks second
  - Background work only when budget remains
- Create test: `test_frame_budget_enforcement.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/FrameBudgetToken.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_frame_budget_enforcement.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Frame time never exceeds 20ms (16ms budget + 4ms grace) during typing
- Degradation levels are visually observable under artificial load
- Input events are never dropped due to rendering overrun
- test_frame_budget_enforcement validates budget checking logic
- FrameScheduler processes tasks in priority order

**Estimated file count**: 1 created, 5 modified
**Estimated complexity**: High

---

### Phase 28: Incremental Re-tokenization & Document Change Minimization

**Prerequisites**: Phase 22

**Objective**: Ensure single-line edits only re-tokenize the affected line(s), not the
entire document.

**Scope**:
- Audit SyntaxHighlighter for full-document re-tokenization paths
- Implement incremental tokenization:
  - Accept edit range (start_line, end_line, delta_lines)
  - Re-tokenize only affected lines
  - Merge results into existing token array
- Audit MarkdownParser for full-reparse paths:
  - Implement incremental parse for single-block edits
  - Fall back to full parse for structural changes
- Wire LineIndex change notifications to SyntaxHighlighter
- Create benchmark: `bench_incremental_tokenization.cpp`
- Create test: `test_incremental_tokenization.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/MarkdownParser.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_incremental_tokenization.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_incremental_tokenization.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Single-character edit re-tokenizes at most 3 lines (edited + neighbors)
- bench_incremental_tokenization shows >10x speedup for small edits
- Full-document tokenization still works as fallback
- test_incremental_tokenization validates correctness for edge cases
- No visual highlighting regressions

**Estimated file count**: 2 created, 5 modified
**Estimated complexity**: High

---

### Phase 29: Structured Logging Framework

**Prerequisites**: Phase 03

**Objective**: Replace ad-hoc spdlog text logging with structured JSON logging,
supporting file and console sinks with configurable levels.

**Scope**:
- Refactor `src/core/Logger.h` and `Logger.cpp`:
  - Add `MARKAMP_LOG_FATAL` level
  - Support structured JSON output format via spdlog custom formatter
  - Add context fields: timestamp, thread_id, component, level
  - Configurable output: console (text), file (JSON), both
- Create `src/core/StructuredLogger.h`:
  - `LogEntry` struct with typed fields
  - `log_structured(component, level, message, key-value pairs...)`
  - Compile-time format string validation
- Update Logger initialization to read log config from Config
- Maintain backward compatibility with existing MARKAMP_LOG_* macros
- Create test: `test_structured_logging.cpp`

**Key files**:
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Logger.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Logger.cpp`
- Created: `/Users/ryanrentfro/code/markamp/src/core/StructuredLogger.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_structured_logging.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- JSON log output is valid JSON (parseable by jq)
- Each log entry includes timestamp, thread_id, level, message
- Existing MARKAMP_LOG_* macros still work
- MARKAMP_LOG_FATAL triggers and is recorded
- test_structured_logging validates format and filtering
- Log level is configurable at runtime

**Estimated file count**: 2 created, 3 modified
**Estimated complexity**: Medium

---

### Phase 30: Crash Reporting Infrastructure

**Prerequisites**: Phase 03

**Objective**: Add crash dump generation and symbolicated stack trace capture for
post-mortem debugging.

**Scope**:
- Create `src/core/CrashReporter.h` / `.cpp`:
  - Signal handler registration (SIGSEGV, SIGABRT, SIGFPE, SIGBUS)
  - Backtrace capture using platform APIs:
    - macOS: `backtrace()` + `atos` symbolication
    - Linux: `backtrace()` + `addr2line`
    - Windows: `MiniDumpWriteDump` + `StackWalk64`
  - Write crash dump to file in crash_reports/ directory
  - Include: stack trace, signal info, version, timestamp
- Create `src/platform/CrashHandler_Mac.cpp` (Objective-C++ if needed)
- Create `src/platform/CrashHandler_Linux.cpp`
- Create `src/platform/CrashHandler_Win.cpp`
- Register handlers in MarkAmpApp::OnInit()
- Create test: `test_crash_reporter.cpp` (validates dump file creation via SIGABRT)

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/CrashReporter.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/CrashReporter.cpp`
- Created: `/Users/ryanrentfro/code/markamp/src/platform/CrashHandler_Mac.cpp`
- Created: `/Users/ryanrentfro/code/markamp/src/platform/CrashHandler_Linux.cpp`
- Created: `/Users/ryanrentfro/code/markamp/src/platform/CrashHandler_Win.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_crash_reporter.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Crash generates dump file in designated directory
- Dump file includes readable stack trace with function names
- Signal handlers do not interfere with normal operation
- test_crash_reporter validates dump file creation
- Works on macOS (primary development platform)

**Estimated file count**: 6 created, 3 modified
**Estimated complexity**: High

---

### Phase 31: Watchdog System -- UI Stall & Deadlock Detection

**Prerequisites**: Phase 05

**Objective**: Implement a watchdog thread that detects UI thread stalls (>100ms) and
potential deadlocks.

**Scope**:
- Create `src/core/Watchdog.h` / `.cpp`:
  - Dedicated watchdog thread (lowest priority)
  - UI thread posts heartbeat every 50ms (via idle handler)
  - Watchdog detects missing heartbeat after 100ms threshold
  - On stall detected:
    - Log stall duration and capture stack trace of UI thread
    - Publish UIStallEvent with duration and trace
    - Track cumulative stall time for diagnostics
  - Configurable thresholds
- Create deadlock detector:
  - Track mutex acquisition order per thread
  - Detect potential lock-ordering violations
  - Log warnings (does not kill -- advisory only)
- Add UIStallEvent to Events.h
- Wire into MarkAmpApp idle handler
- Create test: `test_watchdog.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/Watchdog.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/Watchdog.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_watchdog.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Artificial 200ms sleep on UI thread triggers stall detection
- UIStallEvent contains stall duration and thread info
- Watchdog thread has negligible overhead (<0.1% CPU)
- test_watchdog validates stall detection timing
- No false positives during normal operation

**Estimated file count**: 3 created, 4 modified
**Estimated complexity**: High

---

### Phase 32: Health Monitoring Panel

**Prerequisites**: Phase 09

**Objective**: Create an internal diagnostics panel (hidden by default) showing
real-time performance metrics.

**Scope**:
- Create `src/ui/HealthPanel.h` / `.cpp`:
  - Frame latency histogram (uses existing FrameHistogram)
  - Memory usage graph (current, peak, budget)
  - Active plugins list with activation time
  - EventBus queue depth (pending events)
  - Extension CPU usage (if available from Phase 33)
  - Snapshot generation age
  - UI stall count (from watchdog)
- Accessible via Command Palette: "Developer: Open Health Panel"
- Auto-refresh at 1Hz (not every frame)
- Uses existing ThemeEngine for styling
- Add command registration in MarkAmpApp
- Create test: `test_health_panel.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/ui/HealthPanel.h`
- Created: `/Users/ryanrentfro/code/markamp/src/ui/HealthPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_health_panel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Panel opens via Command Palette
- Shows live frame time histogram
- Shows current memory usage
- Shows active plugin count
- Refresh rate is 1Hz (not impacting frame budget)
- test_health_panel validates data binding

**Estimated file count**: 3 created, 4 modified
**Estimated complexity**: Medium

---

### Phase 33: Per-Extension Resource Tracking

**Prerequisites**: Phase 05, Phase 12

**Objective**: Track memory, CPU time, and event handler latency per extension, exposing
data in diagnostics.

**Scope**:
- Create `src/core/ExtensionResourceTracker.h` / `.cpp`:
  - Per-extension metrics: memory_bytes, cpu_time_us, event_latency_us
  - Instrument all plugin event handler calls with timing
  - Memory tracking via tagged arena or allocation hooks
  - Rolling window statistics (last 60 seconds)
- Integrate into PluginManager event dispatch:
  - Wrap handler calls with timing probes
  - Record per-extension event handler duration
- Expose via HealthPanel (Phase 32) and structured logging
- Create ExtensionMetricsEvent for periodic reporting
- Create test: `test_extension_resource_tracking.cpp`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/ExtensionResourceTracker.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/ExtensionResourceTracker.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_extension_resource_tracking.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Per-extension CPU time is tracked to microsecond precision
- Per-extension event latency shows p50/p95/p99
- Slow extensions (>10ms event handlers) are flagged
- test_extension_resource_tracking validates metric accuracy
- Tracking overhead <1% of total CPU time

**Estimated file count**: 3 created, 4 modified
**Estimated complexity**: Medium

---

### Phase 34: CI Performance Gate & Automation

**Prerequisites**: Phase 07

**Objective**: Create CI configuration that runs sanitizers, benchmarks, and static
analysis on every commit, failing on regressions.

**Scope**:
- Create `.github/workflows/ci-perf.yml` (or equivalent CI config):
  - Matrix: debug-asan, debug-tsan, debug-lsan
  - Run all tests under each sanitizer
  - Run clang-tidy (fail on new warnings)
  - Run benchmarks and compare to baseline
  - Run `scripts/check_catch_all.sh`
  - Run `scripts/audit_static_init.sh`
- Create `.github/workflows/ci-benchmark.yml`:
  - Runs only on main branch merges
  - Updates benchmark baseline
  - Stores as artifact
- Create CI documentation: `docs/v6_docs/ci_performance.md`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/.github/workflows/ci-perf.yml`
- Created: `/Users/ryanrentfro/code/markamp/.github/workflows/ci-benchmark.yml`
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/ci_performance.md`

**Acceptance criteria**:
- CI config is syntactically valid
- All three sanitizer presets are tested
- Benchmark regression >5% would fail the build
- clang-tidy violations fail the build
- Documentation explains CI pipeline

**Estimated file count**: 3 created
**Estimated complexity**: Medium

---

### Phase 35: Large File Mode

**Prerequisites**: Phase 03

**Objective**: Implement automatic large file mode that activates when files exceed a
configurable threshold, reducing rendering and analysis scope.

**Scope**:
- Create `src/core/LargeFileMode.h` / `.cpp`:
  - Configurable threshold (default: 1MB, configurable via Config)
  - When activated:
    - Disable minimap
    - Enable lazy rendering (only visible + prefetch lines)
    - Limit token window to viewport range
    - Reduce diagnostics frequency (1Hz instead of per-keystroke)
    - Disable link preview popovers
    - Disable syntax highlighting for lines >10000 chars
  - Emit LargeFileModeEvent on activation/deactivation
- Integrate into file open path (EditorPanel/MarkdownDocument)
- Wire to Config for threshold adjustment
- Create test: `test_large_file_mode.cpp`
- Create benchmark: `bench_large_file.cpp` (10MB document open + scroll)

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/LargeFileMode.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/LargeFileMode.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/tests/unit/test_large_file_mode.cpp`
- Created: `/Users/ryanrentfro/code/markamp/benchmarks/bench_large_file.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/benchmarks/CMakeLists.txt`

**Acceptance criteria**:
- Files >1MB automatically trigger large file mode
- Minimap disabled in large file mode
- Token window limited to viewport
- bench_large_file shows 10MB document opens in <500ms
- test_large_file_mode validates threshold and feature toggling
- Scrolling remains smooth (16ms frame budget) in large files

**Estimated file count**: 4 created, 5 modified
**Estimated complexity**: Medium

---

### Phase 36: Fuzz Testing Harnesses

**Prerequisites**: Phase 06

**Objective**: Create libFuzzer harnesses for all parser and loader entry points as
specified in PRD section 6.5.

**Scope**:
- Create `fuzz/` directory structure
- Create `fuzz/CMakeLists.txt`:
  - Build fuzz targets with `-fsanitize=fuzzer,address`
  - Each target is a standalone executable
- Create fuzz harnesses:
  - `fuzz_markdown_parser.cpp`: fuzz Md4cWrapper input
  - `fuzz_yaml_config.cpp`: fuzz Config YAML loading
  - `fuzz_extension_manifest.cpp`: fuzz ExtensionManifest JSON
  - `fuzz_theme_loader.cpp`: fuzz ThemeLoader markdown input
  - `fuzz_html_sanitizer.cpp`: fuzz HtmlSanitizer input
  - `fuzz_mermaid_renderer.cpp`: fuzz MermaidRenderer input
- Create seed corpus directories with sample valid inputs
- Create `scripts/run_fuzz.sh` for local fuzzing
- Document fuzzing setup in `docs/v6_docs/fuzz_testing.md`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/fuzz/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/fuzz/fuzz_markdown_parser.cpp`
- Created: `/Users/ryanrentfro/code/markamp/fuzz/fuzz_yaml_config.cpp`
- Created: `/Users/ryanrentfro/code/markamp/fuzz/fuzz_extension_manifest.cpp`
- Created: `/Users/ryanrentfro/code/markamp/fuzz/fuzz_theme_loader.cpp`
- Created: `/Users/ryanrentfro/code/markamp/fuzz/fuzz_html_sanitizer.cpp`
- Created: `/Users/ryanrentfro/code/markamp/fuzz/fuzz_mermaid_renderer.cpp`
- Created: `/Users/ryanrentfro/code/markamp/scripts/run_fuzz.sh`
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/fuzz_testing.md`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance criteria**:
- All 6 fuzz targets compile with `-fsanitize=fuzzer`
- Each target runs for 60 seconds without crashes on seed corpus
- Seed corpus contains at least 10 valid samples per target
- run_fuzz.sh automates harness execution
- fuzz_testing.md documents setup and usage

**Estimated file count**: 9 created, 1 modified
**Estimated complexity**: Medium

---

### Phase 37: Chaos Testing Framework

**Prerequisites**: Phase 09, Phase 36

**Objective**: Build a chaos testing framework that injects random failures to verify
graceful degradation under adversarial conditions.

**Scope**:
- Create `src/core/ChaosEngine.h` / `.cpp`:
  - `ChaosMode` enum: Off, Low, Medium, High
  - Failure injection points:
    - Random plugin activation failure
    - Random filesystem permission error
    - Random allocation failure (malloc returns nullptr)
    - Random event handler exception
    - Random thread scheduling delay
  - Configurable failure probability per injection point
  - Activated via Config or command-line flag
- Create `tests/chaos/` directory:
  - `chaos_test_runner.cpp`: orchestrates chaos test scenarios
  - `chaos_scenario_plugin_failure.cpp`
  - `chaos_scenario_fs_error.cpp`
  - `chaos_scenario_oom.cpp`
- Create `scripts/run_chaos_tests.sh`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/src/core/ChaosEngine.h`
- Created: `/Users/ryanrentfro/code/markamp/src/core/ChaosEngine.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/chaos/chaos_test_runner.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/chaos/chaos_scenario_plugin_failure.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/chaos/chaos_scenario_fs_error.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/chaos/chaos_scenario_oom.cpp`
- Created: `/Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh`
- Modified: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- Chaos engine can be activated via config flag
- Plugin failure injection does not crash host
- Filesystem error injection produces graceful error messages
- OOM injection does not cause undefined behavior
- All chaos scenarios complete without segfault or abort
- Application recovers when chaos mode is disabled

**Estimated file count**: 7 created, 2 modified
**Estimated complexity**: High

---

### Phase 38: Packaging & Distribution (CPack Integration)

**Prerequisites**: Phase 04

**Objective**: Create CPack-based packaging for macOS .pkg, Windows MSI, Linux .deb/.rpm,
and portable tarball.

**Scope**:
- Create `cmake/Packaging.cmake`:
  - CPack configuration for all target platforms
  - macOS: .pkg generator with productbuild
  - Windows: WIX or NSIS generator
  - Linux: DEB and RPM generators
  - Portable: TGZ generator
- Configure install targets:
  - Binary, resources, themes, config_defaults.json
  - Desktop integration files (.desktop, Info.plist)
  - Uninstaller support
- Create `packaging/CMakeLists.txt` for install rules
- Update existing packaging files:
  - `packaging/macos/Info.plist` -- verify version substitution
  - `packaging/linux/markamp.desktop` -- verify paths
  - `packaging/windows/installer.nsi` -- verify sections
- Create `scripts/build_packages.sh` for CI packaging
- Document in `docs/v6_docs/packaging.md`

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/cmake/Packaging.cmake`
- Created: `/Users/ryanrentfro/code/markamp/packaging/CMakeLists.txt`
- Created: `/Users/ryanrentfro/code/markamp/scripts/build_packages.sh`
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/packaging.md`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/packaging/macos/Info.plist`
- Modified: `/Users/ryanrentfro/code/markamp/packaging/linux/markamp.desktop`

**Acceptance criteria**:
- `cpack -G DragNDrop` produces macOS .dmg on macOS
- `cpack -G TGZ` produces portable tarball on all platforms
- Install targets place files in correct locations
- Uninstall target removes all installed files
- packaging.md documents all supported formats

**Estimated file count**: 4 created, 3 modified
**Estimated complexity**: Medium

---

### Phase 39: Load Testing Framework

**Prerequisites**: Phase 07, Phase 09

**Objective**: Build automated load tests that simulate high-stress conditions as
specified in PRD section 11.1.

**Scope**:
- Create `tests/load/` directory
- Create load test scenarios:
  - `load_500_files.cpp`: simulate workspace with 500+ files
  - `load_100_extensions.cpp`: simulate 100 installed extensions
  - `load_large_document.cpp`: 100K-line markdown document
  - `load_continuous_editing.cpp`: sustained rapid keystroke simulation
  - `load_concurrent_operations.cpp`: simultaneous search + edit + preview
- Create `LoadTestHarness` class:
  - Generates synthetic workspaces
  - Creates mock extension manifests
  - Generates large markdown documents
  - Records metrics: frame time, memory, event queue depth
- Create `scripts/run_load_tests.sh`
- Results output as Google Benchmark-compatible JSON

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/tests/load/load_500_files.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/load/load_100_extensions.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/load/load_large_document.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/load/load_continuous_editing.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/load/load_concurrent_operations.cpp`
- Created: `/Users/ryanrentfro/code/markamp/tests/load/LoadTestHarness.h`
- Created: `/Users/ryanrentfro/code/markamp/tests/load/LoadTestHarness.cpp`
- Created: `/Users/ryanrentfro/code/markamp/scripts/run_load_tests.sh`
- Modified: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

**Acceptance criteria**:
- 500-file workspace loads without OOM or crash
- 100-extension scenario activates all extensions without timeout
- 100K-line document opens in <2 seconds
- Continuous editing maintains <16ms frame time for 60 seconds
- All load tests produce machine-readable metrics

**Estimated file count**: 8 created, 1 modified
**Estimated complexity**: High

---

### Phase 40: Final Validation & Definition of Done

**Prerequisites**: Phase 34, Phase 37, Phase 39

**Objective**: Run the complete validation suite to verify all PRD requirements are met,
document results, and produce the final v6 release report.

**Scope**:
- Run complete validation:
  - All sanitizers clean (ASan, UBSan, TSan, LSan)
  - All benchmarks within 5% of baseline
  - Cold start <150ms (measured)
  - Warm start <50ms (measured)
  - Frame time <16ms under editing load
  - Memory idle <150MB
  - All fuzz targets pass 10-minute runs
  - All load tests pass
  - All chaos tests pass
  - No `catch(...)` in codebase
  - clang-tidy clean
- Create `docs/v6_docs/validation_report.md`:
  - Per-requirement pass/fail matrix
  - Benchmark results table
  - Startup timing breakdown
  - Memory budget compliance
  - Known limitations and future work
- Create `docs/v6_docs/performance_tuning_guide.md`:
  - How to use Tracy for profiling
  - How to run fuzz tests
  - How to interpret health panel
  - How to adjust performance budgets
- Update CHANGELOG.md with v6 release notes

**Key files**:
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/validation_report.md`
- Created: `/Users/ryanrentfro/code/markamp/docs/v6_docs/performance_tuning_guide.md`
- Modified: `/Users/ryanrentfro/code/markamp/CHANGELOG.md`

**Acceptance criteria**:
- Every PRD requirement has a documented pass/fail result
- Cold start measured at <150ms on reference hardware
- Zero sanitizer findings
- Zero benchmark regressions >5%
- validation_report.md is complete and accurate
- All 40 phases marked complete

**Estimated file count**: 2 created, 1 modified
**Estimated complexity**: Medium

---

## Summary Statistics

| Metric                        | Value    |
|-------------------------------|----------|
| Total phases                  | 40       |
| New files created (est.)      | ~115     |
| Existing files modified (est.)| ~130     |
| New test files                | ~30      |
| New benchmark files           | ~15      |
| New fuzz harnesses            | 6        |
| New cmake modules             | 6        |
| New scripts                   | 8        |
| High complexity phases        | 10       |
| Medium complexity phases      | 24       |
| Low complexity phases         | 6        |

## Phase Size Distribution

- Smallest phase: Phase 07 (3 files, scripts only)
- Largest phase: Phase 38 (7 files, cross-platform packaging)
- Average phase: ~5-8 files touched
- All phases scoped for single AI agent session execution
