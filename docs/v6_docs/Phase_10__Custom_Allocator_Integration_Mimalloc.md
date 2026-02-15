# Phase 10: Custom Allocator Integration (mimalloc)

## Metadata

| Field | Value |
|---|---|
| Phase ID | 10 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 6 modified |
| PRD Sections | 6.3 Heap Profiling, 4.1.1 Arena Allocators |

---

## Objective

Replace the default C++ allocator with mimalloc for reduced fragmentation and faster small allocations. This provides a foundation-level improvement for all allocation-heavy code paths.

---

## Background

The PRD mentions mimalloc and jemalloc for heap profiling but never specifies replacing the default allocator. For a desktop app targeting <150MB idle memory and <150ms cold start, a custom global allocator is a high-impact, low-effort win. mimalloc (Microsoft's allocator) provides 2-3x faster small allocations, better fragmentation behavior, and built-in statistics.

---

## Scope

### Tasks

1. **Add `mimalloc` to `vcpkg.json`**:
   - Add mimalloc as a dependency
   - Ensure override mode is available

2. **Create `cmake/Allocator.cmake`**:
   - `MARKAMP_USE_MIMALLOC` option (ON by default for Release, OFF for Debug+Sanitizers)
   - When enabled: link mimalloc in override mode (replaces global new/delete)
   - Define `MARKAMP_CUSTOM_ALLOCATOR=mimalloc`
   - Auto-disable when sanitizers are active (ASan has its own allocator)
   - Support future addition of jemalloc as alternative

3. **Create `src/core/AllocatorConfig.h`**:
   - `#include <mimalloc.h>` when `MARKAMP_USE_MIMALLOC` defined
   - Include mimalloc-override header for global replacement
   - Query functions:
     - `[[nodiscard]] auto allocator_name() -> std::string_view`
     - `[[nodiscard]] auto allocator_stats() -> AllocatorStats` (heap size, peak, segments)
   - When mimalloc not enabled: return "system" and empty stats

4. **Create `benchmarks/bench_allocator.cpp`**:
   - `BM_Alloc_SmallObject`: 64-byte allocation/deallocation cycle
   - `BM_Alloc_MediumObject`: 4KB allocation/deallocation cycle
   - `BM_Alloc_LargeObject`: 1MB allocation/deallocation cycle
   - `BM_Alloc_MixedWorkload`: realistic pattern (many small, few large)
   - `BM_Alloc_EventBusStorm`: simulate EventBus publish storm allocations
   - `BM_Alloc_PieceTableEdits`: simulate PieceTable edit allocation pattern

5. **Create `tests/unit/test_allocator.cpp`**:
   - Verify mimalloc is active when enabled (`mi_is_in_main()` or similar check)
   - Verify allocation/deallocation works correctly
   - Verify stats reporting returns non-zero values
   - Verify no interference with existing functionality

6. **Update build files**:
   - Root `CMakeLists.txt`: include `cmake/Allocator.cmake`
   - `cmake/Dependencies.cmake`: find mimalloc package
   - `benchmarks/CMakeLists.txt`: add new benchmark
   - `tests/CMakeLists.txt`: add new test

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `vcpkg.json` |
| Create | `cmake/Allocator.cmake` |
| Create | `src/core/AllocatorConfig.h` |
| Modify | `CMakeLists.txt` |
| Modify | `cmake/Dependencies.cmake` |
| Create | `benchmarks/bench_allocator.cpp` |
| Create | `tests/unit/test_allocator.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- mimalloc in override mode replaces `malloc`/`free`/`new`/`delete` globally. This is the simplest integration path.
- **Critical**: mimalloc must be disabled when sanitizers are active. ASan replaces the allocator and conflicts with mimalloc. The cmake module must check for sanitizer flags and auto-disable.
- mimalloc provides `mi_stats_print()` and `mi_heap_get_default()` for statistics. Use these for the `allocator_stats()` function.
- On macOS, mimalloc's override mode works by interposing dylib symbols. On Linux, it uses `LD_PRELOAD` or compile-time linking. On Windows, it patches the CRT.
- The benchmark should compare performance with and without mimalloc if possible (or document the comparison).

---

## Acceptance Criteria

- [ ] Build succeeds with mimalloc enabled (Release)
- [ ] Build succeeds with mimalloc disabled (Debug, Debug+Sanitizers)
- [ ] `test_allocator` confirms mimalloc is active when enabled
- [ ] `bench_allocator` shows measurable improvement (>10% for small allocations)
- [ ] No test regressions under mimalloc
- [ ] Sanitizer builds automatically disable mimalloc
- [ ] `allocator_name()` returns "mimalloc" when active, "system" otherwise
- [ ] `allocator_stats()` returns non-zero heap size when mimalloc active

---

## Testing Strategy

- Build with Release preset (mimalloc ON) and run all tests
- Build with debug-asan preset (mimalloc OFF) and run all tests
- Run bench_allocator and compare against system allocator numbers
- Run test_allocator to verify activation detection
