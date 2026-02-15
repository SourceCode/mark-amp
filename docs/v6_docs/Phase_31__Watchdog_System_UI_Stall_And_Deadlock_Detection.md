# Phase 31: Watchdog System -- UI Stall & Deadlock Detection

## Metadata

| Field | Value |
|---|---|
| Phase ID | 31 |
| Prerequisites | Phase 05 |
| Estimated Complexity | High |
| Estimated File Count | 3 created, 4 modified |
| PRD Sections | 5.1 Watchdog System |

---

## Objective

Implement a watchdog thread that detects UI thread stalls (>100ms) and potential deadlocks. Log incidents and publish events for diagnostics.

---

## Background

The PRD requires: "UI thread stall detection (>100ms), Deadlock detection, Extension activation timeout. If stall detected: Log incident, Recover gracefully if possible." A watchdog thread monitors the UI thread's heartbeat and detects when it stops responding, which indicates a stall (long computation on UI thread) or deadlock (mutual lock dependency).

---

## Scope

### Tasks

1. **Create `src/core/Watchdog.h` / `Watchdog.cpp`**:
   - Dedicated watchdog thread:
     - Lowest priority (`std::thread` with platform priority hint)
     - Sleeps for `check_interval_ms` (default: 50ms)
     - On wake: check heartbeat timestamp
   - UI thread heartbeat:
     - UI thread calls `Watchdog::heartbeat()` in idle handler (every frame)
     - Records `std::chrono::steady_clock::now()` into atomic variable
   - Stall detection:
     - If `now() - last_heartbeat > stall_threshold_ms` (default: 100ms):
       - Log stall with duration
       - Capture UI thread stack trace (platform-specific, best-effort)
       - Publish `UIStallEvent` with duration and optional trace
       - Increment cumulative stall counter
   - Configuration:
     - `watchdog.enabled`: bool (default: true)
     - `watchdog.stall_threshold_ms`: uint32_t (default: 100)
     - `watchdog.check_interval_ms`: uint32_t (default: 50)
   - Startup/shutdown:
     - `start()`: launch watchdog thread
     - `stop()`: signal thread to exit and join

2. **Create deadlock detection (advisory)**:
   - Track mutex acquisition order per thread:
     - `MutexTracker::lock(mutex_id, thread_id)`: record acquisition
     - `MutexTracker::unlock(mutex_id, thread_id)`: record release
   - Detect potential lock-ordering violations:
     - Thread A: locks M1 then M2
     - Thread B: locks M2 then M1
     - This is a potential deadlock even if it hasn't occurred yet
   - Log warnings for ordering violations (advisory only, does not kill)
   - Optionally gate behind `MARKAMP_DEBUG_DEADLOCK` compile flag

3. **Add events to `Events.h`**:
   - `UIStallEvent`: { duration_ms, thread_id, stack_trace (optional) }

4. **Wire into `MarkAmpApp`**:
   - Start watchdog after EventBus is initialized
   - Call `watchdog.heartbeat()` in the idle handler
   - Stop watchdog in `OnExit()`

5. **Create `tests/unit/test_watchdog.cpp`**:
   - Test stall detection: simulate 200ms sleep on "UI thread," verify detection
   - Test heartbeat prevents false positives under normal operation
   - Test `UIStallEvent` contains correct duration
   - Test watchdog thread has negligible overhead
   - Test start/stop lifecycle

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/Watchdog.h` |
| Create | `src/core/Watchdog.cpp` |
| Modify | `src/core/Events.h` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/CMakeLists.txt` |
| Create | `tests/unit/test_watchdog.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- The heartbeat must be atomic: `std::atomic<std::chrono::steady_clock::time_point>` or `std::atomic<uint64_t>` with timestamp encoding.
- Stack trace capture of another thread is platform-specific and not guaranteed:
  - macOS: `thread_get_state()` + `backtrace_from_fp()`
  - Linux: `pthread_getattr_np()` or `/proc/<pid>/task/<tid>/stack`
  - Windows: `SuspendThread()` + `StackWalk64()` + `ResumeThread()`
  - This is best-effort — if it fails, just report "stack unavailable"
- The watchdog thread should have minimal overhead. Sleeping for 50ms between checks means it wakes up ~20 times/second, each wake is a timestamp comparison (~10ns).
- Deadlock detection via lock ordering is a debug-mode feature. In production, it adds too much overhead (tracking every mutex lock/unlock).
- Avoid false positives: if the application is minimized or the system is under heavy load, stalls may not indicate bugs. Consider adding a "system load" heuristic.

---

## Acceptance Criteria

- [ ] Artificial 200ms sleep on UI thread triggers stall detection
- [ ] `UIStallEvent` contains stall duration and thread info
- [ ] Watchdog thread has negligible overhead (<0.1% CPU)
- [ ] `test_watchdog` validates stall detection timing
- [ ] No false positives during normal operation (1000 frames without false trigger)
- [ ] Watchdog starts and stops cleanly (no leaked threads)
- [ ] Deadlock detection logs ordering violations (debug mode only)
- [ ] Configuration is respected (threshold, interval)

---

## Testing Strategy

- Run test_watchdog to verify detection timing and event publishing
- Run application under normal use for extended period, verify no false positives
- Inject deliberate stall (sleep on UI thread), verify detection
- Run under TSan to verify thread safety of watchdog
- Measure CPU overhead of watchdog thread using profiler
