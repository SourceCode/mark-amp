# Phase 15: Bounded Containers and Resource Caps

## Metadata

| Field | Value |
|---|---|
| Phase ID | 15 |
| Prerequisites | Phase 01 (Result types), Phase 05 (EventBus bounded queue already done) |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 2 modified, 1 test |
| PRD Sections | PI-16 (all containers bounded), PII-6 (memory pressure monitor), PII-7 (fail-safe memory reserve), PII-9 (fragmentation monitoring), PII-10 (hard caps with graceful degradation) |

---

## Objective

Add explicit capacity limits to all dynamically-growing containers: diagnostics buffers, output channels, telemetry events, and open file buffers. Provide `BoundedVector<T>` and `BoundedDeque<T>` container wrappers with configurable max size and eviction policy. Establish centralized resource limits and enforce hard caps with graceful degradation.

---

## Background

The PRD mandates that all containers must be bounded (PI-16) and hard caps must degrade gracefully instead of crashing (PII-10). Currently several containers in MarkAmp can grow without limit — a runaway diagnostic stream or a plugin producing endless output could exhaust memory. EventBus queue bounding was done in Phase 05; this phase covers all remaining containers.

---

## Scope

### Tasks

1. **Create `src/core/BoundedContainer.h` / `src/core/BoundedContainer.cpp`**:
   ```cpp
   namespace markamp::core {

   enum class EvictionPolicy {
       DropOldest,    // Remove oldest when full
       DropNewest,    // Reject new items when full
       DropRandom     // Remove random item when full (for cache-like structures)
   };

   template<typename T>
   class BoundedVector {
   public:
       explicit BoundedVector(size_t max_size, EvictionPolicy policy = EvictionPolicy::DropOldest);

       auto push_back(T value) -> bool;  // Returns false if dropped (DropNewest)
       auto size() const -> size_t;
       auto capacity() const -> size_t;
       auto at(size_t index) const -> const T&;
       auto begin() const -> auto;
       auto end() const -> auto;
       void clear();

       // Stats
       auto dropped_count() const -> size_t;

   private:
       std::deque<T> data_;  // deque for efficient front removal
       size_t max_size_;
       EvictionPolicy policy_;
       size_t dropped_count_{0};
   };

   template<typename T>
   class BoundedDeque {
       // Similar interface with front/back access
   };

   } // namespace markamp::core
   ```
   - On overflow with `DropOldest`: remove front element, add new at back, log at DEBUG level
   - On overflow with `DropNewest`: reject new element, return false, log at DEBUG level
   - Track `dropped_count_` for metrics

2. **Create `src/core/ResourceLimits.h`**:
   ```cpp
   namespace markamp::core::resource_limits {

   // Container limits
   constexpr size_t kMaxDiagnosticsEntries = 10000;
   constexpr size_t kMaxOutputChannelLines = 50000;
   constexpr size_t kMaxTelemetryEvents = 5000;
   constexpr size_t kMaxOpenFileBuffers = 100;
   constexpr size_t kMaxExtensions = 200;
   constexpr size_t kMaxRecentFiles = 50;
   constexpr size_t kMaxUndoHistory = 1000;
   constexpr size_t kMaxSearchResults = 10000;
   constexpr size_t kMaxBookmarks = 10000;

   // Memory limits
   constexpr size_t kMaxIdleMemoryMB = 150;
   constexpr size_t kMemoryWarningThresholdMB = 500;
   constexpr size_t kMemoryReserveBytes = 1 * 1024 * 1024;  // 1 MB emergency reserve

   } // namespace markamp::core::resource_limits
   ```

3. **Modify `src/core/DiagnosticsService.cpp`**:
   - Replace unbounded diagnostics collection with `BoundedVector<DiagnosticEntry>(kMaxDiagnosticsEntries)`
   - Log warning when diagnostics overflow

4. **Modify `src/core/OutputChannelService.cpp`**:
   - Replace unbounded output lines with `BoundedVector<std::string>(kMaxOutputChannelLines)`
   - Log warning when output channel overflow

5. **Create `tests/unit/test_bounded_container.cpp`**:
   - TEST_CASE: "BoundedVector accepts items up to capacity"
   - TEST_CASE: "BoundedVector DropOldest removes front on overflow"
   - TEST_CASE: "BoundedVector DropNewest rejects on overflow"
   - TEST_CASE: "BoundedVector tracks dropped count"
   - TEST_CASE: "BoundedVector clear resets state"
   - TEST_CASE: "BoundedVector iteration works correctly"
   - TEST_CASE: "BoundedVector at() access works"
   - TEST_CASE: "BoundedDeque front/back access works"
   - TEST_CASE: "Hard cap produces warning when hit"
   - TEST_CASE: "ResourceLimits constants are sane values"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/BoundedContainer.h` |
| Create | `src/core/BoundedContainer.cpp` |
| Create | `src/core/ResourceLimits.h` |
| Modify | `src/core/DiagnosticsService.cpp` |
| Modify | `src/core/OutputChannelService.cpp` |
| Create | `tests/unit/test_bounded_container.cpp` |

---

## Implementation Notes

- **Template implementation**: Most of `BoundedVector<T>` is in the header file (template class). The `.cpp` file contains non-template helper functions.
- **Internal storage**: Use `std::deque<T>` rather than `std::vector<T>` for the internal storage. `deque` supports efficient `pop_front()` for `DropOldest` policy without copying all elements.
- **Thread safety**: `BoundedVector` itself is NOT thread-safe. The caller is responsible for synchronization. This keeps the container lightweight. Services that need thread safety (e.g., DiagnosticsService) already have their own mutexes.
- **Dropped count**: This metric feeds into Phase 34 (telemetry spans) and Phase 38 (fault domain health).
- **Memory reserve**: The 1 MB emergency reserve (PII-7) is allocated at startup and freed if `std::bad_alloc` is caught, giving the system enough memory to log the error and degrade gracefully. Implement this in the main app startup.
- **Hard cap enforcement**: When `kMaxOpenFileBuffers` is reached, show a user-visible notification: "Maximum number of open files reached. Please close some files."
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] BoundedVector drops oldest when at capacity (DropOldest policy)
- [ ] BoundedVector rejects new items when at capacity (DropNewest policy)
- [ ] Eviction logged at DEBUG level
- [ ] Dropped count tracked accurately
- [ ] No `std::bad_alloc` from runaway container growth
- [ ] Hard caps produce user-visible warning when hit
- [ ] ResourceLimits constants centralized in one header
- [ ] DiagnosticsService uses bounded container
- [ ] OutputChannelService uses bounded container
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test BoundedVector at capacity boundary (N-1, N, N+1 items)
- Test each eviction policy
- Test that dropped_count accurately reflects evictions
- Test clear() resets both container and stats
- Stress test: push 1 million items into a BoundedVector(100), verify size never exceeds 100
- Run under ASan to verify no memory issues with eviction
