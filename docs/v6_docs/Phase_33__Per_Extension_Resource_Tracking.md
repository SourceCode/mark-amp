# Phase 33: Per-Extension Resource Tracking

## Metadata

| Field | Value |
|---|---|
| Phase ID | 33 |
| Prerequisites | Phase 05, Phase 12 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 4 modified |
| PRD Sections | 9.2 Extension Resource Tracking |

---

## Objective

Track memory, CPU time, and event handler latency per extension. Expose metrics in diagnostics and structured logging.

---

## Background

The PRD requires: "Track per extension: Memory, CPU, Event handler latency. Expose in diagnostics." This enables identification of poorly performing extensions that degrade the overall application experience. When an extension's event handler takes >10ms consistently, it should be flagged.

---

## Scope

### Tasks

1. **Create `src/core/ExtensionResourceTracker.h` / `ExtensionResourceTracker.cpp`**:
   - Per-extension metrics structure:
     ```cpp
     struct ExtensionMetrics {
         std::string extension_id;
         uint64_t total_cpu_time_us;      // cumulative CPU time
         uint64_t memory_bytes;           // current estimated memory
         uint64_t event_count;            // total events handled
         uint64_t slow_event_count;       // events >10ms
         LatencyStats event_latency;      // p50, p95, p99
     };
     ```
   - `LatencyStats` using a compact percentile tracker:
     - Rolling window of last 1000 samples
     - Efficiently compute p50, p95, p99 using sorted insertion or t-digest
   - `record_event(extension_id, duration_us)`: called after each event handler
   - `record_memory(extension_id, bytes)`: called periodically
   - `get_metrics(extension_id) -> ExtensionMetrics`: query current metrics
   - `get_all_metrics() -> vector<ExtensionMetrics>`: for Health Panel
   - `get_slow_extensions(threshold_us) -> vector<string>`: identify problem extensions

2. **Integrate into PluginManager event dispatch**:
   - Wrap all plugin event handler calls with timing probes:
     ```cpp
     auto start = steady_clock::now();
     handler(event);
     auto elapsed = duration_cast<microseconds>(steady_clock::now() - start);
     resource_tracker_->record_event(plugin_id, elapsed.count());
     ```
   - For memory: use tagged arena or allocation delta measurement around handler calls
   - Minimal overhead: timing uses `steady_clock` (typically ~20ns per call)

3. **Publish periodic metrics event**:
   - `ExtensionMetricsEvent`: published every 60 seconds
   - Contains aggregated metrics for all extensions
   - Consumed by Health Panel (Phase 32) and structured logger (Phase 29)

4. **Flag slow extensions**:
   - If p95 event latency > 10ms, log warning with extension ID
   - If p99 event latency > 50ms, publish `ExtensionSlowEvent`
   - Configurable thresholds via Config

5. **Create `tests/unit/test_extension_resource_tracking.cpp`**:
   - Test event recording and latency calculation
   - Test percentile accuracy (p50, p95, p99)
   - Test slow extension detection
   - Test rolling window behavior (old samples are evicted)
   - Test concurrent recording from multiple threads
   - Test memory tracking accuracy

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ExtensionResourceTracker.h` |
| Create | `src/core/ExtensionResourceTracker.cpp` |
| Modify | `src/core/PluginManager.cpp` |
| Modify | `src/core/Events.h` |
| Modify | `src/CMakeLists.txt` |
| Create | `tests/unit/test_extension_resource_tracking.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- For percentile tracking with a rolling window, use a sorted array of the last N samples. Insertion is O(N) but N=1000 is small enough. Alternatively, use a t-digest for approximate percentiles with O(1) insertion.
- CPU time tracking: `std::chrono::steady_clock` measures wall time, not CPU time. For actual CPU time, use `clock_gettime(CLOCK_THREAD_CPUTIME_ID)` on Linux/macOS or `GetThreadTimes()` on Windows. Wall time is a reasonable approximation for single-threaded handlers.
- Memory tracking per extension is inherently approximate without a per-extension allocator. Options:
  1. Measure RSS delta before/after handler (noisy but simple)
  2. Use tagged arenas (requires extension to allocate through arena)
  3. Skip precise memory tracking, report only allocation count
- The tracking overhead should be <1% of total CPU time. Timing two `steady_clock::now()` calls per handler invocation is ~40ns, which is negligible for handlers that take microseconds or milliseconds.

---

## Acceptance Criteria

- [ ] Per-extension CPU time tracked to microsecond precision
- [ ] Per-extension event latency shows p50/p95/p99 values
- [ ] Slow extensions (>10ms p95) are flagged in logs
- [ ] `test_extension_resource_tracking` validates metric accuracy
- [ ] Tracking overhead <1% of total CPU time
- [ ] `ExtensionMetricsEvent` published every 60 seconds
- [ ] Metrics accessible via `get_all_metrics()` for Health Panel
- [ ] Concurrent recording is thread-safe

---

## Testing Strategy

- Run test_extension_resource_tracking with known latency distributions
- Verify percentile calculations against expected values
- Run bench_eventbus (Phase 06) with tracking enabled, verify <1% overhead
- Run under TSan for thread safety validation
