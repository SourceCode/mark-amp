# Phase 23: Data-Oriented Refactor -- Contiguous Subscriber Storage

## Metadata

| Field | Value |
|---|---|
| Phase ID | 23 |
| Prerequisites | Phase 20 |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 6 modified |
| PRD Sections | 11.2 Data-Oriented Refactors, 4.1.2 Cache Locality First |

---

## Objective

Convert event subscriber storage, PluginManager dependency graph, and StyleRunStore from Array-of-Structs (AoS) to Struct-of-Arrays (SoA) for better cache locality during iteration.

---

## Background

The PRD mandates: "Audit EditorPanel state, PluginManager storage, Event storage. Convert scattered objects to contiguous pools." And: "Prefer std::vector over std::list, contiguous memory layouts, SoA over AoS where applicable." SoA layout stores each field in its own contiguous array, so iterating over a single field hits fewer cache lines.

---

## Scope

### Tasks

1. **Refactor EventBus subscriber iteration** (builds on Phase 20):
   - Current: `vector<HandlerEntry>` where `HandlerEntry = {id, priority, function}`
   - New SoA layout:
     - `vector<SubscriptionId> ids_`
     - `vector<uint8_t> priorities_`
     - `vector<HandlerFunction> handlers_`
   - Iteration during publish: only `handlers_` is accessed (cache friendly)
   - Unsubscribe: uses `ids_` to find index, then removes from all arrays

2. **Apply SoA to PluginManager dependency graph**:
   - Current: `vector<PluginInfo>` where PluginInfo is a struct with many fields
   - New SoA for hot-path data:
     - `vector<PluginId> plugin_ids_`
     - `vector<bool> active_states_`
     - `vector<ActivationEvent> activation_events_`
   - Cold data (description, path, version) stays in AoS for rare access
   - Activation check loop only touches `active_states_` array

3. **Apply SoA to StyleRunStore**:
   - Current: `vector<StyleRun>` where `StyleRun = {start, length, style_id}`
   - New SoA:
     - `vector<uint32_t> starts_`
     - `vector<uint32_t> lengths_`
     - `vector<uint16_t> style_ids_`
   - Rendering iterates `starts_` and `lengths_` for position calculation
   - Style lookup only touches `style_ids_` when applying styles

4. **Create `benchmarks/bench_data_oriented.cpp`**:
   - `BM_EventBus_IterateHandlers_AoS`: baseline with struct-of-arrays
   - `BM_EventBus_IterateHandlers_SoA`: new SoA layout
   - `BM_PluginManager_CheckActive_AoS`: baseline
   - `BM_PluginManager_CheckActive_SoA`: SoA active_states iteration
   - `BM_StyleRun_Iterate_AoS`: baseline
   - `BM_StyleRun_Iterate_SoA`: SoA iteration
   - Parameterize by element count (100, 1000, 10000)

5. **Create `tests/unit/test_soa_storage.cpp`**:
   - Test SoA add/remove operations keep arrays synchronized
   - Test iteration produces same results as AoS
   - Test empty container edge cases
   - Test concurrent read safety

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/EventBus.h` |
| Modify | `src/core/PluginManager.h` |
| Modify | `src/core/PluginManager.cpp` |
| Modify | `src/core/StyleRunStore.h` |
| Create | `benchmarks/bench_data_oriented.cpp` |
| Create | `tests/unit/test_soa_storage.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- SoA is most beneficial when iteration touches only a subset of fields. If all fields are accessed together, SoA and AoS perform similarly.
- Keeping arrays synchronized during add/remove requires careful bookkeeping. Consider a helper class `SoATable<Fields...>` that manages the synchronization.
- For EventBus handlers, the hot path only iterates the handler function array. The ID array is only used for unsubscribe lookups (cold path).
- StyleRunStore's SoA layout benefits rendering because position calculation only needs starts/lengths, while style application only needs style_ids.
- Use `std::vector::reserve()` to pre-allocate SoA arrays to avoid reallocations.

---

## Acceptance Criteria

- [ ] EventBus handler iteration accesses only contiguous handler array during publish
- [ ] `bench_data_oriented` shows measurable improvement for iteration-heavy operations
- [ ] `test_soa_storage` validates correct storage/retrieval for all SoA conversions
- [ ] All existing EventBus and PluginManager tests pass
- [ ] StyleRunStore iteration is cache-friendly (verified by benchmark)
- [ ] No functional regression from layout change

---

## Testing Strategy

- Run bench_data_oriented and compare AoS vs SoA results
- Run test_soa_storage for correctness validation
- Run full test suite for regression checking
- Run under ASan to detect any memory access issues
