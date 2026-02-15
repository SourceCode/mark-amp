# Phase 39: Load Testing Framework

## Metadata

| Field | Value |
|---|---|
| Phase ID | 39 |
| Prerequisites | Phase 07, Phase 09 |
| Estimated Complexity | High |
| Estimated File Count | 8 created, 1 modified |
| PRD Sections | 11.1 Load Testing |

---

## Objective

Build automated load tests that simulate high-stress conditions: 500+ file workspaces, 100+ extensions, large documents, and continuous editing bursts. Verify the application maintains performance under sustained load.

---

## Background

The PRD requires: "Simulate: 500+ file workspace, 100+ extensions installed, Large markdown rendering, Continuous editing bursts." Load testing verifies that the system scales beyond typical usage patterns and identifies breaking points.

---

## Scope

### Tasks

1. **Create `tests/load/LoadTestHarness.h` / `LoadTestHarness.cpp`**:
   - Utilities for generating synthetic test data:
     - `generate_workspace(num_files, avg_file_size) -> TempDirectory`: creates a temporary workspace with N markdown files
     - `generate_extensions(num_extensions) -> vector<ExtensionManifest>`: creates mock extension manifests
     - `generate_large_document(num_lines) -> string`: creates a large markdown document
     - `generate_edit_sequence(num_edits) -> vector<EditOperation>`: creates a sequence of edit operations
   - Metrics recording:
     - Frame time samples
     - Memory usage samples
     - Event queue depth samples
     - Per-operation latency
   - JSON output compatible with Google Benchmark for CI integration

2. **Create load test scenarios**:

   **`tests/load/load_500_files.cpp`**:
   - Generate workspace with 500 markdown files (1KB-100KB each)
   - Open workspace
   - Measure: workspace scan time, file index build time, search across all files
   - Success: no OOM, no crash, scan <5 seconds

   **`tests/load/load_100_extensions.cpp`**:
   - Generate 100 mock extension manifests
   - Activate all extensions
   - Measure: activation time, memory overhead, event dispatch latency with all active
   - Success: all activate without timeout, memory <300MB total

   **`tests/load/load_large_document.cpp`**:
   - Generate 100K-line markdown document (~5MB)
   - Open document
   - Measure: open time, initial render time, scroll latency, search latency
   - Success: opens in <2 seconds, scroll at 60fps, search in <500ms

   **`tests/load/load_continuous_editing.cpp`**:
   - Open a medium document (1000 lines)
   - Simulate sustained typing at 100 characters/second for 60 seconds
   - Measure: frame time p50/p95/p99, memory growth, event queue depth
   - Success: p95 frame time <16ms for full 60 seconds

   **`tests/load/load_concurrent_operations.cpp`**:
   - Simultaneously run: text search (background), editing (foreground), preview generation (background)
   - Measure: edit latency, search throughput, preview update frequency
   - Success: edit latency <8ms, no data races, no deadlocks

3. **Create `scripts/run_load_tests.sh`**:
   - Builds load test executables
   - Runs all scenarios sequentially
   - Reports pass/fail per scenario with metrics
   - Accepts `--scenario` flag to run individual tests
   - Output compatible with CI artifacts

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `tests/load/LoadTestHarness.h` |
| Create | `tests/load/LoadTestHarness.cpp` |
| Create | `tests/load/load_500_files.cpp` |
| Create | `tests/load/load_100_extensions.cpp` |
| Create | `tests/load/load_large_document.cpp` |
| Create | `tests/load/load_continuous_editing.cpp` |
| Create | `tests/load/load_concurrent_operations.cpp` |
| Create | `scripts/run_load_tests.sh` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- Load tests are expensive to run (seconds to minutes). They should NOT be part of the normal `ctest` run. Gate behind `MARKAMP_BUILD_LOAD_TESTS` option.
- For the 500-file workspace, use `std::filesystem::temp_directory_path()` and clean up after the test.
- Mock extensions should have valid manifest structure but no-op handlers. They should still go through the full activation path.
- Continuous editing simulation: use a `std::chrono`-based loop that submits edit operations at a fixed rate (100 chars/sec = 1 edit every 10ms).
- Concurrent operations test: use `std::thread` for background operations. Run search and preview generation on background threads while editing on the "main" thread. Check for data races with TSan.
- All load tests should produce machine-readable output (JSON) for trend analysis.
- The LoadTestHarness should handle cleanup (temp files, extensions) even if the test crashes (use RAII).

---

## Acceptance Criteria

- [ ] 500-file workspace loads without OOM or crash in <5 seconds
- [ ] 100-extension scenario activates all extensions without timeout
- [ ] 100K-line document opens in <2 seconds
- [ ] Continuous editing maintains p95 frame time <16ms for 60 seconds
- [ ] Concurrent operations complete without data races or deadlocks
- [ ] All load tests produce machine-readable JSON metrics
- [ ] `run_load_tests.sh` reports pass/fail per scenario
- [ ] LoadTestHarness cleans up temporary files after each test

---

## Testing Strategy

- Run each load test individually to verify correctness
- Run all load tests sequentially via run_load_tests.sh
- Run concurrent operations test under TSan for race detection
- Verify JSON output is parseable and contains expected metrics
- Compare results across runs for stability
