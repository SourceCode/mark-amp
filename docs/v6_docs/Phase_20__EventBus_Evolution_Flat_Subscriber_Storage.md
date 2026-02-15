# Phase 20: EventBus Evolution -- Flat Subscriber Storage

## Metadata

| Field | Value |
|---|---|
| Phase ID | 20 |
| Prerequisites | Phase 16 |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 6 modified |
| PRD Sections | 11.1 EventBus Evolution |

---

## Objective

Replace the EventBus `unordered_map<type_index, shared_ptr<vector>>` with a flat, cache-friendly subscriber array. Make `publish_fast()` truly lock-free by eliminating the `fast_lookup_mutex_`.

---

## Background

The current EventBus uses `std::unordered_map<std::type_index, std::shared_ptr<std::vector<HandlerEntry>>>` for subscriber storage. Every `publish()` performs a hash map lookup. `publish_fast()` claims to be "lock-free" but still acquires `fast_lookup_mutex_` for the fast lookup table. The PRD mandates: "Flat contiguous subscriber lists, cache-friendly storage, avoid std::function where possible, use templated callback wrappers."

---

## Scope

### Tasks

1. **Create `src/core/EventTypeId.h`**:
   - Compile-time event type ID assignment
   - Each event type gets a unique `uint16_t` ID
   - Use template specialization or CRTP counter pattern:
     ```cpp
     template<typename T>
     struct EventTypeIdHolder {
         static inline const uint16_t value = next_id();
     };
     ```
   - `constexpr uint16_t kMaxEventTypes = 256;`
   - `event_type_id<T>()` returns the ID for event type T

2. **Refactor EventBus internals** (`src/core/EventBus.h`, `EventBus.cpp`):
   - Replace `unordered_map<type_index, ...>` with:
     ```cpp
     std::array<std::atomic<std::shared_ptr<SubscriberList>>, kMaxEventTypes> subscribers_;
     ```
   - `SubscriberList` is an immutable vector of handler entries (COW pattern preserved)
   - `publish_fast()` becomes:
     ```
     auto id = event_type_id<EventT>();
     auto list = subscribers_[id].load(std::memory_order_acquire);
     // iterate list — no mutex, no map lookup
     ```
   - `subscribe()` still takes a lock for COW list replacement:
     - Load current list
     - Create new list with added handler
     - Atomic store new list
   - `unsubscribe()` follows same COW pattern
   - **Remove `fast_lookup_mutex_`** — no longer needed

3. **Maintain public API compatibility**:
   - `subscribe<EventT>(handler)` — same signature
   - `unsubscribe(subscription_id)` — same signature
   - `publish(event)` — same signature
   - `publish_fast(event)` — same signature, now truly lock-free
   - Type-erased `publish(type_index, void*)` for runtime-typed events

4. **Update Events.h**:
   - Ensure all event types work with the new ID system
   - Verify `MARKAMP_DECLARE_EVENT` macro is compatible

5. **Create `benchmarks/bench_eventbus_v2.cpp`**:
   - `BM_EventBusV2_PublishFast`: lock-free publish throughput
   - `BM_EventBusV2_Publish`: standard publish throughput
   - `BM_EventBusV2_Subscribe`: subscribe throughput
   - `BM_EventBusV2_ConcurrentPublish`: multi-threaded publish
   - Compare against Phase 06 baseline

6. **Create `tests/unit/test_eventbus_v2.cpp`**:
   - Test subscribe and receive events
   - Test unsubscribe stops delivery
   - Test concurrent publish/subscribe safety (multi-threaded)
   - Test publish_fast is truly lock-free (timing-based verification)
   - Test COW list replacement doesn't affect in-flight iteration
   - Test event type ID uniqueness

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/EventTypeId.h` |
| Modify | `src/core/EventBus.h` |
| Modify | `src/core/EventBus.cpp` |
| Modify | `src/core/Events.h` |
| Create | `benchmarks/bench_eventbus_v2.cpp` |
| Create | `tests/unit/test_eventbus_v2.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- The flat array approach requires each event type to have a unique ID. Use atomic counter incremented on first template instantiation.
- `std::atomic<std::shared_ptr<T>>` is C++20. If not available, use `std::atomic_load`/`std::atomic_store` free functions (C++11).
- COW (Copy-On-Write) for subscriber lists: reads are lock-free (atomic load), writes create a new list and atomically swap. This is safe because writers are serialized (by a write mutex) and readers always see a consistent list.
- The `kMaxEventTypes = 256` limit is generous. If exceeded, fall back to hash map for overflow event types.
- `publish_fast()` hot path should be: load atomic pointer, check non-null, iterate handlers. Three operations, no locks, no hash computation.
- Memory ordering: `memory_order_acquire` for loads, `memory_order_release` for stores. This ensures handlers see the complete event data.

---

## Acceptance Criteria

- [ ] `publish_fast()` acquires zero locks (no mutex in the path)
- [ ] No `unordered_map` lookup in hot publish path
- [ ] `bench_eventbus_v2` shows >2x throughput for `publish_fast` vs Phase 06 baseline
- [ ] All existing EventBus tests pass without API changes
- [ ] `test_eventbus_v2` validates concurrent publish/subscribe safety
- [ ] Event type ID is unique per type and stable across program lifetime
- [ ] COW list replacement is safe during concurrent iteration
- [ ] `fast_lookup_mutex_` is removed from EventBus

---

## Testing Strategy

- Run test_eventbus_v2 with TSan (debug-tsan preset) for concurrency validation
- Run bench_eventbus_v2 and compare against Phase 06 baseline
- Run all existing EventBus-dependent tests for regression checking
- Stress test: 8 threads publishing concurrently while 2 threads subscribe/unsubscribe
