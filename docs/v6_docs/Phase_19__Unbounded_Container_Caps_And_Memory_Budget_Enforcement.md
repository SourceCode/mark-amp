# Phase 19: Unbounded Container Caps & Memory Budget Enforcement

## Metadata

| Field | Value |
|---|---|
| Phase ID | 19 |
| Prerequisites | Phase 14 |
| Estimated Complexity | Medium |
| Estimated File Count | 5 created, 6 modified |
| PRD Sections | 4.1.3 Avoid Unbounded Growth, 7.2 Memory Budget |

---

## Objective

Cap all unbounded-growth containers identified in the PRD and implement runtime memory budget monitoring with pressure events.

---

## Background

The PRD identifies that caps must extend to: EventBus queue, Notification queue, OutputChannel logs, Extension telemetry buffers. The memory budget is: idle <150MB, large file <2x file size overhead, per-plugin configurable cap. Currently, some containers (Profiler history, Mermaid block sources) are already capped, but others may grow without bound.

---

## Scope

### Tasks

1. **Create `src/core/BoundedContainer.h`**:
   - `BoundedQueue<T, MaxSize>`:
     - Ring buffer semantics — overwrites oldest when full
     - `push(T&&)`, `front()`, `pop()`, `size()`, `capacity()`, `is_full()`
     - Contiguous storage via `std::array<T, MaxSize>`
     - Thread-safe variant with atomic read/write indices
   - `BoundedVector<T>`:
     - Vector with runtime-configurable hard cap
     - `push_back()` returns false when at capacity
     - `try_push_back()` alternative that drops silently
     - `resize_cap(new_cap)` to adjust at runtime

2. **Apply caps to containers**:
   - **EventBus queued_events_**: max 4096 events, drop oldest on overflow
   - **EventBus fast_queue_**: already capped at 1024, verify and document
   - **OutputChannelService logs**: max 10,000 lines per channel, drop oldest
   - **NotificationService queue**: max 100 pending notifications, drop oldest
   - **Extension telemetry buffers**: max 1,000 entries per extension, drop oldest
   - **Profiler timing samples**: already addressed in Phase 16 (ring buffer), verify

3. **Create `src/core/MemoryBudget.h` / `MemoryBudget.cpp`**:
   - Track approximate total memory usage:
     - Query allocator stats (mimalloc from Phase 10)
     - Query known large containers (PieceTable, EventBus, caches)
   - Define budgets:
     - `idle_budget_bytes`: 150MB (configurable via Config)
     - `large_file_multiplier`: 2x file size
     - `extension_budget_bytes`: 50MB per extension
   - Compare actual vs budget periodically (every 1 second)
   - Publish `MemoryPressureEvent` when exceeding 80% of budget
   - Publish `MemoryExceededEvent` when exceeding 100% of budget
   - Include current usage and budget in event payload

4. **Add events to `Events.h`**:
   - `MemoryPressureEvent`: { current_bytes, budget_bytes, percentage }
   - `MemoryExceededEvent`: { current_bytes, budget_bytes, details }

5. **Create `tests/unit/test_bounded_containers.cpp`**:
   - Test BoundedQueue push/pop at capacity
   - Test BoundedQueue overwrite oldest behavior
   - Test BoundedVector rejection at capacity
   - Test edge cases: empty queue pop, single-element capacity

6. **Create `tests/unit/test_memory_budget.cpp`**:
   - Test MemoryBudget reports correct usage approximation
   - Test MemoryPressureEvent fires at 80% threshold
   - Test MemoryExceededEvent fires at 100%
   - Test budget calculation for large files

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/BoundedContainer.h` |
| Create | `src/core/MemoryBudget.h` |
| Create | `src/core/MemoryBudget.cpp` |
| Modify | `src/core/EventBus.h` |
| Modify | `src/core/EventBus.cpp` |
| Modify | `src/core/OutputChannelService.cpp` |
| Modify | `src/core/Events.h` |
| Create | `tests/unit/test_bounded_containers.cpp` |
| Create | `tests/unit/test_memory_budget.cpp` |
| Modify | `src/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- BoundedQueue uses a circular buffer pattern: `write_pos % capacity` for the write index. This avoids any shifting or copying on push.
- Caps should only apply to diagnostic/log buffers — never cap user data containers. User document content, undo history, and bookmark data must not be silently dropped.
- Memory budget tracking is approximate. Don't try to count every allocation — use allocator-level statistics (mimalloc provides `mi_heap_get_used()`) supplemented by known large container sizes.
- The periodic check (1Hz) should not impact frame budget. Use an idle handler or timer, not every-frame check.
- MemoryPressureEvent can be consumed by subsystems to release caches, trim prefetch buffers, etc.

---

## Acceptance Criteria

- [ ] No diagnostic/log container grows without bound
- [ ] BoundedQueue correctly overwrites oldest when full
- [ ] BoundedVector correctly rejects push when at capacity
- [ ] MemoryBudget publishes `MemoryPressureEvent` when exceeding 80% of budget
- [ ] MemoryBudget publishes `MemoryExceededEvent` when exceeding 100%
- [ ] All tests pass including bounded container edge cases
- [ ] No data loss for user-facing data (caps only on diagnostic buffers)
- [ ] EventBus queue cap prevents unbounded growth under event storm

---

## Testing Strategy

- Run test_bounded_containers for thorough edge case validation
- Run test_memory_budget for threshold detection validation
- Run full test suite for regression checking
- Stress test: publish 10,000 events rapidly, verify EventBus queue stays within cap
