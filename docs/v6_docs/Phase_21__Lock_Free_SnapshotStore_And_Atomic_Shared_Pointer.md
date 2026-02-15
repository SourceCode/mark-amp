# Phase 21: Lock-Free SnapshotStore & Atomic Shared Pointer

## Metadata

| Field | Value |
|---|---|
| Phase ID | 21 |
| Prerequisites | Phase 20 |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 4 modified |
| PRD Sections | 4.2 Lock-Free and Concurrency |

---

## Objective

Replace the mutex-based SnapshotStore with C++20 `std::atomic<std::shared_ptr<>>` for truly lock-free reader access. Migrate all atomic shared_ptr usage in the codebase to the C++20 class template.

---

## Background

`DocumentSnapshot.h` uses `mutable std::mutex` + `std::lock_guard` to protect snapshot access. This blocks readers when a writer is updating the snapshot. C++20 provides `std::atomic<std::shared_ptr<T>>` which offers lock-free reads on most platforms. The PRD mandates: "Use atomic shared_ptr swaps for snapshots" and "Lock-free read-mostly paths."

---

## Scope

### Tasks

1. **Refactor `DocumentSnapshot.h`**:
   - Replace `mutable std::mutex snapshot_mutex_` with `std::atomic<std::shared_ptr<DocumentSnapshot>>`
   - `current()` becomes: `return snapshot_.load(std::memory_order_acquire);` — lock-free
   - `publish(new_snapshot)` becomes: `snapshot_.store(new_snapshot, std::memory_order_release);`
   - Writer serialization maintained by external mechanism (only writer thread publishes)
   - Remove the mutex and lock_guard entirely

2. **Audit all `std::atomic_load`/`std::atomic_store` free function usage**:
   - EventBus COW lists already use C++11 free functions (`std::atomic_load`, `std::atomic_store`)
   - Migrate these to C++20 `std::atomic<std::shared_ptr<T>>` class template
   - This is a cleaner API and may be more efficient on some platforms

3. **Verify platform support**:
   - `std::atomic<std::shared_ptr<T>>` requires `__cpp_lib_atomic_shared_ptr`
   - If not available (older stdlib), provide a polyfill using free functions
   - Check with `#if __cpp_lib_atomic_shared_ptr >= 202011L`

4. **Create `tests/unit/test_lockfree_snapshot.cpp`**:
   - Test concurrent read/write: multiple reader threads, one writer thread
   - Test that readers always see a complete, consistent snapshot (never torn)
   - Test that old snapshots are correctly reference-counted (no premature deletion)
   - Stress test with rapid snapshot updates and concurrent reads

5. **Create `benchmarks/bench_snapshot_store.cpp`**:
   - `BM_Snapshot_Read_Contended`: read throughput with concurrent writer
   - `BM_Snapshot_Read_Uncontended`: read throughput without contention
   - `BM_Snapshot_Write`: write (publish) throughput
   - Compare against mutex-based baseline

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/DocumentSnapshot.h` |
| Modify | `src/core/EventBus.h` |
| Create | `tests/unit/test_lockfree_snapshot.cpp` |
| Create | `benchmarks/bench_snapshot_store.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `std::atomic<std::shared_ptr<T>>` may not be truly lock-free on all platforms (check `atomic<shared_ptr<T>>::is_always_lock_free`). Even when using internal spinlocks, it's typically faster than a full mutex because the critical section is tiny (pointer swap).
- Memory ordering: `memory_order_acquire` for loads ensures the reader sees all data written before the store. `memory_order_release` for stores ensures the writer's data is visible to subsequent loads.
- The reference counting in `shared_ptr` ensures old snapshots remain valid as long as any reader holds a reference. This is the key safety property.
- If `__cpp_lib_atomic_shared_ptr` is not available, fall back to `std::atomic_load(shared_ptr<T>*)` and `std::atomic_store(shared_ptr<T>*, shared_ptr<T>)` free functions (C++11).
- Run the concurrent tests under TSan to verify no data races.

---

## Acceptance Criteria

- [ ] `SnapshotStore::current()` contains no mutex lock
- [ ] EventBus uses C++20 `std::atomic<std::shared_ptr<>>` instead of free functions
- [ ] `bench_snapshot_store` shows improvement under contention
- [ ] `test_lockfree_snapshot` validates concurrent read/write correctness
- [ ] All existing snapshot-related tests pass
- [ ] No data races under TSan
- [ ] Platform compatibility: compiles with polyfill when C++20 feature unavailable

---

## Testing Strategy

- Run test_lockfree_snapshot under normal conditions and TSan
- Run bench_snapshot_store and compare against mutex baseline
- Run full test suite for regression checking
- Stress test with 8 reader threads and 1 writer publishing rapidly
