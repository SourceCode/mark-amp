# Phase 16: Profiler Data Structure Optimization

## Metadata

| Field | Value |
|---|---|
| Phase ID | 16 |
| Prerequisites | Phase 12 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 4 modified |
| PRD Sections | 4.1.2 Cache Locality First, 4.1.3 Avoid Unbounded Growth |

---

## Objective

Replace the Profiler's heap-allocating `unordered_map<string, ...>` with a fixed-ID system that avoids heap allocation on the recording hot path. Switch from growing vector to capped ring buffer for timing samples.

---

## Background

The current Profiler uses `std::unordered_map<std::string, TimingData>` for storing timing records. Every `record()` call performs a hash map lookup with a heap-allocated string key. On high-frequency paths (EventBus publish, render passes), this creates significant allocation overhead. A fixed-ID system using an enum index into a flat array eliminates all hash computation and string allocation.

---

## Scope

### Tasks

1. **Create `src/core/ProfilerIds.h`**:
   - `enum class ProfileId : uint16_t` with named constants for all profiled zones:
     - `EventBus_Publish`, `EventBus_PublishFast`, `EventBus_Subscribe`
     - `HtmlRenderer_Render`, `HtmlRenderer_RenderBlock`, `HtmlRenderer_Concatenate`
     - `SyntaxHighlighter_Tokenize`, `SyntaxHighlighter_IncrementalTokenize`
     - `MarkdownParser_Parse`, `MarkdownParser_IncrementalParse`
     - `Config_Get`, `Config_Set`
     - `ThemeEngine_ColorLookup`, `ThemeEngine_BrushCache`
     - `PluginManager_Dispatch`, `PluginManager_Activate`
     - `Startup_Total`, `Startup_ConfigLoad`, `Startup_ThemeInit`, etc.
     - `kMaxProfileIds` sentinel value (use for array sizing)
   - `constexpr std::array<std::string_view, kMaxProfileIds>` mapping IDs to names
   - `[[nodiscard]] constexpr auto profile_id_name(ProfileId id) -> std::string_view`

2. **Refactor Profiler internals** (`src/core/Profiler.h`, `Profiler.cpp`):
   - Replace `std::unordered_map<std::string, TimingData>` with `std::array<TimingData, kMaxProfileIds>`
   - New `record(ProfileId id, Duration duration)` method — zero allocation, direct array index
   - Keep `record(std::string_view name, Duration duration)` as slow path for dynamic/extension profiling (uses separate map)
   - Replace `std::vector<Sample>` with fixed-size circular buffer (ring buffer):
     - Default capacity: 1024 samples per zone
     - Overwrites oldest on wrap
     - No dynamic growth, no allocation
   - Update `MARKAMP_PROFILE_SCOPE` macro to accept `ProfileId`
   - Provide backward-compatible macro that converts string to runtime lookup

3. **Create `benchmarks/bench_profiler_v2.cpp`**:
   - `BM_Profiler_Record_ById`: ProfileId-based record throughput
   - `BM_Profiler_Record_ByName`: string_view-based record throughput (slow path)
   - `BM_Profiler_Scope_ById`: scoped timing with ProfileId
   - `BM_Profiler_RingBuffer`: ring buffer write throughput
   - Compare against pre-optimization baseline

4. **Create `tests/unit/test_profiler_v2.cpp`**:
   - Test record by ProfileId stores correctly
   - Test ring buffer wrapping behavior
   - Test ring buffer at capacity
   - Test ProfileId name lookup
   - Test slow path (string-based) still works
   - Test report generation includes all active zones

5. **Update all existing `MARKAMP_PROFILE_SCOPE` call sites**:
   - Replace string literals with `ProfileId` constants where possible
   - Extension/plugin profiling continues to use string-based slow path

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ProfilerIds.h` |
| Modify | `src/core/Profiler.h` |
| Modify | `src/core/Profiler.cpp` |
| Create | `benchmarks/bench_profiler_v2.cpp` |
| Create | `tests/unit/test_profiler_v2.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- The flat array approach trades memory (pre-allocated slots for all possible zones) for speed (O(1) lookup, no allocation). With `uint16_t` IDs and 256 max zones, the overhead is ~256 * sizeof(TimingData) which is negligible.
- Ring buffer implementation: `struct RingBuffer<T, N> { std::array<T, N> data; uint32_t write_pos = 0; }`. Write: `data[write_pos % N] = value; ++write_pos;`.
- The `MARKAMP_PROFILE_SCOPE(ProfileId::EventBus_Publish)` macro should expand to create a scoped timer that calls `record(id, elapsed)` in its destructor.
- For backward compatibility, `MARKAMP_PROFILE_SCOPE("custom_name")` should still compile, using the string-based slow path.
- Consider using `if constexpr` to dispatch between ID-based and string-based paths at compile time.

---

## Acceptance Criteria

- [ ] `Profiler::record(ProfileId)` makes zero heap allocations
- [ ] Ring buffer has configurable cap (default 1024 samples per zone)
- [ ] Ring buffer correctly wraps and overwrites oldest entries
- [ ] `bench_profiler_v2` shows >5x throughput improvement for ID-based recording
- [ ] `test_profiler_v2` validates all operations including ring buffer edge cases
- [ ] All existing `MARKAMP_PROFILE_SCOPE` sites still compile and work
- [ ] Report generation includes data from both ID-based and string-based zones
- [ ] `ProfileId` enum has a `kMaxProfileIds` sentinel for array sizing

---

## Testing Strategy

- Run test_profiler_v2 for unit-level validation
- Run bench_profiler_v2 and compare against Phase 06 profiler baseline
- Run full test suite to ensure no regressions from refactor
- Verify profile report output is correct and complete
