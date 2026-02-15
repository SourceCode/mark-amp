# Phase 14: Arena Allocator Expansion to Hot Paths

## Metadata

| Field | Value |
|---|---|
| Phase ID | 14 |
| Prerequisites | Phase 10 |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 7 modified |
| PRD Sections | 4.1.1 Use Arena Allocators |

---

## Objective

Expand FrameArena usage from per-frame scratch space to cover the three highest-allocation hot paths: EventBus dispatch, HtmlRenderer render passes, and SyntaxHighlighter tokenization.

---

## Background

The PRD mandates: "Expand FrameArena for per-frame allocations, render batch operations, temporary parsing buffers. Use std::pmr::polymorphic_allocator, std::pmr::vector, std::pmr::string. Avoid heap churn in EventBus, Rendering passes, Theme application."

FrameArena already exists in the codebase. This phase extends its use to the three highest-allocation hot paths to eliminate heap churn during frame processing.

---

## Scope

### Tasks

1. **Create `FrameArenaPool` class** (extend `src/core/FrameArena.h`):
   - Maintains per-thread arena instances via `thread_local`
   - `get_arena()` returns the current thread's arena
   - `reset_all()` resets all active arenas (called once per frame on main thread)
   - Configurable arena size (default: 256KB per thread)
   - Uses `std::pmr::monotonic_buffer_resource` as the underlying allocator
   - Falls back to upstream allocator (mimalloc from Phase 10) when arena exhausted

2. **Expand FrameArena to EventBus**:
   - During `publish()`: use arena-backed `std::pmr::vector` for handler snapshot copy
   - Currently: handler list is copied to a `std::vector` (heap allocation per publish)
   - New: handler list copied to `std::pmr::vector` using frame arena (zero heap alloc)
   - Arena is reset at frame boundary, so snapshot lifetime is correct

3. **Expand FrameArena to HtmlRenderer**:
   - Use arena-backed `std::pmr::string` for temporary HTML string concatenation
   - Use arena-backed containers for render batch (list of render operations)
   - The render pass produces output, then arena resets — temporaries are automatically freed

4. **Expand FrameArena to SyntaxHighlighter**:
   - Use arena-backed `std::pmr::vector<Token>` for incremental tokenization results
   - Tokens are produced during tokenization, consumed by rendering, then arena resets
   - Avoids per-tokenize heap allocation for token storage

5. **Create `benchmarks/bench_arena_hotpaths.cpp`**:
   - `BM_EventBus_Publish_Arena`: publish with arena vs without
   - `BM_HtmlRenderer_Arena`: render pass with arena temporaries
   - `BM_SyntaxHighlighter_Arena`: tokenize with arena token storage
   - Compare allocation counts and throughput

6. **Create `tests/unit/test_arena_expansion.cpp`**:
   - Test FrameArenaPool thread-local behavior
   - Test arena-backed EventBus publish correctness
   - Test arena reset does not corrupt in-flight data
   - Test fallback when arena is exhausted

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/FrameArena.h` |
| Modify | `src/core/EventBus.h` |
| Modify | `src/core/EventBus.cpp` |
| Modify | `src/rendering/HtmlRenderer.cpp` |
| Modify | `src/core/SyntaxHighlighter.cpp` |
| Create | `benchmarks/bench_arena_hotpaths.cpp` |
| Create | `tests/unit/test_arena_expansion.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `std::pmr::monotonic_buffer_resource` allocates from a contiguous buffer and does not free individual allocations — it only supports bulk reset. This is ideal for frame-scoped temporaries.
- The arena must be reset once per frame, after all rendering is complete. If data from the arena is needed beyond the frame, it must be copied to the heap first.
- Thread-local arenas avoid the need for synchronization. Each thread (main UI, background highlighter) gets its own arena.
- When the arena is exhausted, `monotonic_buffer_resource` automatically allocates from its upstream resource (default `new_delete_resource` or mimalloc). This prevents crashes from oversized frames.
- Be careful with arena-backed strings: if the string escapes the frame scope (stored in a persistent data structure), it will be dangling after arena reset.

---

## Acceptance Criteria

- [ ] EventBus `publish()` makes zero heap allocations for handler dispatch (verified by benchmark)
- [ ] HtmlRenderer render pass uses arena for all temporary strings and containers
- [ ] SyntaxHighlighter uses arena-backed token vector
- [ ] `bench_arena_hotpaths` shows >30% allocation reduction in hot paths
- [ ] All existing tests pass (no correctness regressions)
- [ ] Arena exhaustion falls back correctly (no crash, just slower)
- [ ] `test_arena_expansion` validates arena behavior and thread safety

---

## Testing Strategy

- Run bench_arena_hotpaths and compare to pre-arena baseline
- Run all unit tests including new test_arena_expansion
- Run under ASan to detect any use-after-free from arena reset
- Stress test with large documents to verify arena exhaustion fallback
