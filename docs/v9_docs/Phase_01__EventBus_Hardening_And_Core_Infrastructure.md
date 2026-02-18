# Phase 01: EventBus Hardening and Core Infrastructure

## Overview
The EventBus is the backbone of MarkAmp's architecture, yet it currently has known issues: `publish_fast()` still acquires a mutex despite documentation claiming lock-free operation, the `queued_events_` vector is unbounded, and `main()` has zero exception handling. This phase hardens the foundational infrastructure that every other subsystem depends on.

## Prerequisites
- None (this is the foundation phase)

## Tasks

### Task 1: Replace EventBus publish_fast() Mutex with Lock-Free Queue
**Files:** `src/core/EventBus.h`, `src/core/EventBus.cpp`, `src/core/SPSCQueue.h`
**Description:** The `publish_fast()` method currently acquires a mutex, contradicting its documented "fast-path" purpose. Replace the internal storage with a bounded SPSC or MPSC lock-free queue using the existing `SPSCQueue.h` primitive for the hot path. Keep `publish()` for the general case with mutex protection.
**Acceptance Criteria:**
- `publish_fast()` executes without acquiring any mutex
- Bounded queue prevents unbounded memory growth (cap at 4096 entries)
- Backpressure policy: drop oldest event when queue is full, log warning
- All existing 111+ tests continue to pass

### Task 2: Bound the queued_events_ Vector
**Files:** `src/core/EventBus.h`, `src/core/EventBus.cpp`
**Description:** The `queued_events_` vector can grow unboundedly if consumers are slow. Add a configurable maximum capacity (default 8192) with a circular overwrite policy. Emit a diagnostic event when capacity is reached.
**Acceptance Criteria:**
- `queued_events_` never exceeds configured maximum
- Oldest events are dropped when full
- A `MARKAMP_LOG_WARN` is emitted on overflow with event count

### Task 3: Add Exception Barrier to main() / wxEntry
**Files:** `src/app/MarkAmpApp.cpp`, `src/app/MarkAmpApp.h`
**Description:** `main()` currently has zero exception handling. Wrap the OnInit/OnExit lifecycle with a top-level try/catch that logs structured errors and attempts graceful shutdown. This prevents unhandled exceptions from silently terminating the application.
**Acceptance Criteria:**
- Unhandled exceptions in OnInit produce a logged error and clean exit
- Unhandled exceptions in OnIdle are caught, logged, and do not crash the app
- OnExit failures are logged but do not prevent shutdown

### Task 4: Add Thread Ownership Assertions to EventBus
**Files:** `src/core/EventBus.h`, `src/core/EventBus.cpp`, `src/core/ThreadBoundary.h`
**Description:** EventBus subscribers execute callbacks that often touch UI. Add debug-mode assertions verifying that `process_queued()` and `drain_fast_queue()` are called from the main thread only. Use `ThreadBoundary.h` utilities for the check.
**Acceptance Criteria:**
- Debug builds assert on main-thread-only methods called from worker threads
- Release builds have zero overhead (assertion compiles out)
- Test added: `test_event_bus.cpp` verifies assertion fires on wrong thread in debug

### Task 5: Add Subscription Leak Detection
**Files:** `src/core/EventBus.h`, `src/core/EventBus.cpp`
**Description:** When EventBus is destroyed, any remaining active subscriptions indicate a leak (UI components not properly cleaning up). Add a destructor check that logs all remaining subscription type names with subscriber count.
**Acceptance Criteria:**
- EventBus destructor logs warning if active subscriptions remain
- Each leaked subscription type name and count is logged
- Normal shutdown path (all RAII subscriptions destroyed first) produces no warnings

### Task 6: Consolidate Event Type Deduplication
**Files:** `src/core/Events.h`
**Description:** Events.h contains documented duplicate removal notes (R12 removing duplicates of R8/R9 events) but the file is 2430+ lines with no section index. Add a table-of-contents comment at the top listing every event by category with line numbers. Verify no semantic duplicates remain.
**Acceptance Criteria:**
- Table-of-contents comment at top of Events.h lists all event categories
- No two events serve the same semantic purpose
- Each event category section has a clear boundary comment

### Task 7: Add EventBus Performance Counters
**Files:** `src/core/EventBus.h`, `src/core/EventBus.cpp`
**Description:** Add lightweight counters tracking: events published per type, events dropped, max queue depth reached, and average drain time. Expose via a `stats()` method for the health panel.
**Acceptance Criteria:**
- `EventBus::stats()` returns struct with publish count, drop count, max depth, avg drain time
- Counters are atomic and do not impact hot-path performance
- Test verifies counters increment correctly

### Task 8: Strengthen Config Error Handling
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** Config currently hardcodes 40+ defaults and returns `std::expected` from load/save. Add validation that checks for unknown keys (potential typos), range validation for numeric settings, and type checking. Log warnings for invalid values rather than silently using defaults.
**Acceptance Criteria:**
- Unknown config keys produce a warning log
- Numeric settings outside defined ranges produce a warning and use clamped values
- Boolean settings with non-boolean values produce a warning and use default
- All existing config tests pass

### Task 9: Add Structured Startup Timing
**Files:** `src/core/StartupTimer.h`, `src/core/StartupTimer.cpp`, `src/app/MarkAmpApp.cpp`
**Description:** The `StartupTimer` exists but is not wired into the app startup. Instrument each phase of `MarkAmpApp::OnInit()` with timing markers so startup bottlenecks can be identified and tracked across versions.
**Acceptance Criteria:**
- Each numbered section in OnInit (1-13) has a start/end timing marker
- Total startup time logged at INFO level
- Per-phase times logged at DEBUG level
- Timing data accessible via `StartupTimer::phases()` for health panel

### Task 10: Fix OnIdle Event Draining Efficiency
**Files:** `src/app/MarkAmpApp.cpp`
**Description:** `OnIdle` currently drains both `process_queued()` and `drain_fast_queue()` on every idle tick. If there are no events to process, this is wasted CPU. Add a fast check (e.g., `has_pending()`) to skip processing when both queues are empty.
**Acceptance Criteria:**
- OnIdle returns immediately when no events are pending
- `has_pending()` method added to EventBus, checking without locking
- Behavior is identical when events are present

### Task 11: Add PluginManager Activation Error Reporting
**Files:** `src/core/PluginManager.cpp`, `src/core/PluginManager.h`
**Description:** `plugin_manager_->activate_all()` silently swallows activation failures. Add structured error reporting: which plugins failed, why, and whether they are optional or required.
**Acceptance Criteria:**
- Each failed plugin activation is logged with plugin ID and error reason
- `activate_all()` returns a struct with success_count and failure_details vector
- Built-in plugins that fail to activate trigger a warning notification

### Task 12: Add RAII Service Lifetime Guards
**Files:** `src/app/MarkAmpApp.h`, `src/app/MarkAmpApp.cpp`
**Description:** The manual reverse-order `.reset()` chain in `OnExit()` is fragile. Wrap the service collection in a struct whose destructor handles the correct teardown order, eliminating the manual reset chain.
**Acceptance Criteria:**
- All 21 extension services are destroyed in correct reverse order automatically
- Core services (EventBus, Config, etc.) are destroyed after extension services
- Shutdown produces the same log output as before
- No use-after-free on shutdown (verified with AddressSanitizer)

### Task 13: Consolidate Logger Initialization
**Files:** `src/core/Logger.h`, `src/core/Logger.cpp`, `src/core/StructuredLogger.h`
**Description:** Two logging systems exist: `Logger.h` (MARKAMP_LOG_* macros wrapping spdlog) and `StructuredLogger.h` (structured logging with fields). Consolidate so structured logger is the default and MARKAMP_LOG_* macros route through it.
**Acceptance Criteria:**
- MARKAMP_LOG_* macros emit structured log records
- Existing log format is preserved for console output
- Structured fields (timestamp, level, component) are available for programmatic access

### Task 14: Add Service Health Check Protocol
**Files:** `src/core/HealthPanel.h` (exists), new `src/core/IHealthCheckable.h`
**Description:** The HealthPanel header exists but there is no standard interface for services to report health. Define an `IHealthCheckable` interface with `health_status()` returning ok/degraded/failed plus diagnostic string. Implement on EventBus, Config, ThemeEngine, and PluginManager.
**Acceptance Criteria:**
- `IHealthCheckable` interface defined with `health_status()` and `diagnostic_info()`
- EventBus, Config, ThemeEngine, PluginManager implement the interface
- Health status reflects actual state (e.g., EventBus reports "degraded" if queue overflow detected)

### Task 15: Add Deterministic Service Initialization Order Validation
**Files:** `src/app/MarkAmpApp.cpp`
**Description:** Services have implicit initialization order dependencies (e.g., EventBus before everything, Config before ThemeEngine). Add compile-time or runtime assertions that verify the initialization order matches the dependency graph.
**Acceptance Criteria:**
- Attempting to create ThemeEngine before EventBus triggers a clear assertion
- Service initialization order is documented in a comment block at the top of OnInit
- No circular dependencies exist (verified by topological sort check)

### Task 16: Add EventBus Type-Safe Event Filtering
**Files:** `src/core/EventBus.h`
**Description:** Currently subscribers receive all events of a type. Add an optional filter predicate to `subscribe()` so subscribers can reject events before callback invocation, reducing unnecessary work.
**Acceptance Criteria:**
- `subscribe<T>(callback, filter_predicate)` overload added
- Filter predicate is evaluated before callback
- Existing `subscribe<T>(callback)` continues to work unchanged
- Test demonstrates filtering reduces callback invocations

### Task 17: Document and Test the Event Lifecycle Contract
**Files:** `tests/unit/test_event_bus.cpp`
**Description:** Add comprehensive tests documenting the EventBus lifecycle contract: subscription ordering, RAII cleanup, re-entrant publishing behavior, and cross-event-type ordering guarantees.
**Acceptance Criteria:**
- Test: subscriptions are called in registration order
- Test: RAII subscription cleanup prevents dangling callbacks
- Test: publishing from within a callback does not deadlock
- Test: queued events are processed FIFO
- Test: fast-path events are processed in insertion order

### Task 18: Add CMake Build Infrastructure Validation
**Files:** `CMakeLists.txt`, `tests/CMakeLists.txt`
**Description:** The tests/CMakeLists.txt is 2082 lines with 170+ test targets. Verify that every source file in `src/` is listed in `add_executable()`, that `source_group()` matches, and that no orphan files exist outside the build.
**Acceptance Criteria:**
- Script or test verifies all `.cpp` files under `src/` are in `add_executable()`
- All `.h` files under `src/` are in `source_group()` or `target_sources()`
- No orphan source files exist outside the build
- Report lists any discrepancies

### Task 19: Add Debug Build Watchdog Timer
**Files:** `src/core/Watchdog.h`, `src/app/MarkAmpApp.cpp`
**Description:** The `Watchdog.h` header exists but is not wired. In debug builds, start a watchdog thread that detects if the main thread is blocked for more than 2 seconds (configurable). Log a stack trace when triggered.
**Acceptance Criteria:**
- Watchdog detects main-thread stalls > 2 seconds in debug builds
- Stall produces a log with thread ID and duration
- Watchdog is disabled in release builds
- Clean shutdown stops the watchdog without false positives

### Task 20: Add Memory Budget Monitoring
**Files:** `src/core/MemoryBudget.h`, `src/app/MarkAmpApp.cpp`
**Description:** `MemoryBudget.h` exists but is not wired into the application. Add periodic (every 30 seconds) memory usage sampling in debug builds, logging when the application exceeds configurable thresholds (default: warn at 500MB, error at 1GB).
**Acceptance Criteria:**
- Memory usage sampled every 30 seconds in debug builds
- Warning logged when usage exceeds 500MB
- Error logged when usage exceeds 1GB
- Thresholds configurable via Config

## Testing Requirements
- All existing 111+ test suites must continue to pass
- New tests for EventBus hardening: lock-free queue, bounded queue, subscription leak detection
- New tests for Config validation: unknown keys, range checks, type checks
- Integration test: full startup/shutdown cycle completes without warnings
- AddressSanitizer clean on startup/shutdown cycle

## Phase Completion Criteria
- EventBus is hardened: bounded queues, lock-free fast path, subscription leak detection
- main() has exception barriers preventing silent crashes
- Config validates inputs and reports issues
- Startup timing is instrumented and logged
- Service initialization order is validated
- Debug-mode watchdog and memory monitoring are active
- All tests pass: `cmake --preset debug && cmake --build build/debug -j$(sysctl -n hw.ncpu) && cd build/debug && ctest --output-on-failure`
