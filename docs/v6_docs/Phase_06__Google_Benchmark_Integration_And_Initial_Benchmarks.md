# Phase 06: Google Benchmark Integration & Initial Benchmarks

## Metadata

| Field | Value |
|---|---|
| Phase ID | 06 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 6 created, 3 modified |
| PRD Sections | 6.6 Deterministic Benchmark Suite |

---

## Objective

Add Google Benchmark as a dependency, create the `markamp_bench` executable, and write the first benchmark suite for critical hot paths. This establishes the performance measurement baseline that CI will enforce.

---

## Background

The PRD mandates a `markamp_bench` benchmark executable using Google Benchmark (open source) to measure: startup cold path, publish vs publish_fast, layout recompute, syntax highlight incremental, file open large document, and extension activation. This phase creates the infrastructure and first five benchmarks targeting core data structures.

---

## Scope

### Tasks

1. **Add `benchmark` to `vcpkg.json`**:
   - Add `benchmark` (Google Benchmark) as a dependency
   - Ensure it supports C++23

2. **Create `benchmarks/` directory structure**:
   - `benchmarks/CMakeLists.txt` — defines `markamp_bench` executable
   - Links Google Benchmark and `markamp_core` library
   - Gated behind `MARKAMP_BUILD_BENCHMARKS` option (OFF by default)

3. **Create initial benchmarks**:

   **`bench_eventbus.cpp`**:
   - `BM_EventBus_Publish`: publish throughput (handlers subscribed)
   - `BM_EventBus_PublishFast`: publish_fast throughput
   - `BM_EventBus_Subscribe`: subscribe/unsubscribe cycle
   - Parameterize by subscriber count (1, 10, 100)

   **`bench_profiler.cpp`**:
   - `BM_Profiler_Record`: record() throughput
   - `BM_Profiler_Scope`: MARKAMP_PROFILE_SCOPE overhead
   - `BM_Profiler_Report`: report generation time

   **`bench_frame_arena.cpp`**:
   - `BM_FrameArena_AllocReset`: arena alloc/reset cycle vs heap new/delete
   - `BM_FrameArena_ManySmallAllocs`: 1000 small allocations per frame
   - `BM_FrameArena_LargeAlloc`: single large allocation

   **`bench_spsc_queue.cpp`**:
   - `BM_SPSCQueue_PushPop`: single-producer single-consumer throughput
   - `BM_SPSCQueue_BatchPush`: batch push operations
   - Parameterize by message size (small struct, medium, large)

   **`bench_piece_table.cpp`**:
   - `BM_PieceTable_Insert`: insert operations at random positions
   - `BM_PieceTable_Delete`: delete operations
   - `BM_PieceTable_GetLine`: line retrieval after edits
   - Parameterize by document size (1KB, 100KB, 1MB)

4. **Update root `CMakeLists.txt`**:
   - Add `MARKAMP_BUILD_BENCHMARKS` option
   - `add_subdirectory(benchmarks)` when enabled

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `vcpkg.json` |
| Create | `benchmarks/CMakeLists.txt` |
| Create | `benchmarks/bench_eventbus.cpp` |
| Create | `benchmarks/bench_profiler.cpp` |
| Create | `benchmarks/bench_frame_arena.cpp` |
| Create | `benchmarks/bench_spsc_queue.cpp` |
| Create | `benchmarks/bench_piece_table.cpp` |
| Modify | `CMakeLists.txt` |
| Modify | `cmake/Dependencies.cmake` |

---

## Implementation Notes

- Google Benchmark provides `BENCHMARK()` macro and `benchmark::State` for iteration control.
- Use `--benchmark_format=json` flag to produce machine-readable output for CI comparison.
- Each benchmark file should include `<benchmark/benchmark.h>` and define `BENCHMARK_MAIN()` in the main executable.
- For `markamp_bench`, use a single `main()` that registers all benchmark files. Google Benchmark provides `BENCHMARK_MAIN()` for this.
- Benchmarks should test the core library in isolation — no wxWidgets GUI dependencies.
- Use `benchmark::DoNotOptimize()` to prevent compiler from eliminating measured work.

---

## Acceptance Criteria

- [ ] `markamp_bench` builds when `MARKAMP_BUILD_BENCHMARKS=ON`
- [ ] Each benchmark produces valid Google Benchmark JSON output (`--benchmark_format=json`)
- [ ] EventBus publish benchmark shows >100K ops/sec
- [ ] FrameArena benchmark shows >5x throughput vs raw new/delete
- [ ] All 5 benchmark files compile without warnings
- [ ] All benchmarks complete in <30 seconds total
- [ ] Benchmark results are reproducible (variance <10% between runs)

---

## Testing Strategy

- Build with `-DMARKAMP_BUILD_BENCHMARKS=ON`
- Run `./markamp_bench --benchmark_format=json --benchmark_out=baseline.json`
- Verify JSON output is parseable
- Run twice and compare results for stability
- Verify each benchmark reports reasonable numbers
