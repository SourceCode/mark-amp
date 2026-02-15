# Phase 18: Generation-Based Invalidation

## Metadata

| Field | Value |
|---|---|
| Phase ID | 18 |
| Prerequisites | Phase 06 (thread ownership) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-28 (generation-based invalidation), PI-19 (no cross-thread shared raw references) |

---

## Objective

Extend the existing `GenerationCounter` pattern to be universally available and create a `ValidatedRef<T>` wrapper that returns an error on stale access instead of dereferencing potentially freed memory. This eliminates use-after-free bugs from stale cross-thread references.

---

## Background

The PRD mandates generation-based invalidation everywhere (PI-28) and no cross-thread shared raw references (PI-19). The codebase already has `GenerationCounter.h`. This phase enhances it with a `ValidatedRef<T>` that combines a reference with a generation counter, enabling safe cross-thread data access patterns where readers hold references to data that may be invalidated by writers.

---

## Scope

### Tasks

1. **Modify `src/core/GenerationCounter.h`**:
   - Ensure `GenerationCounter` uses `std::atomic<uint64_t>` for thread safety
   - Add convenience methods:
     ```cpp
     class GenerationCounter {
     public:
         GenerationCounter() : generation_{0} {}

         [[nodiscard]] auto current() const -> uint64_t {
             return generation_.load(std::memory_order_acquire);
         }

         auto increment() -> uint64_t {
             return generation_.fetch_add(1, std::memory_order_release) + 1;
         }

         [[nodiscard]] auto is_valid(uint64_t captured) const -> bool {
             return captured == generation_.load(std::memory_order_acquire);
         }

     private:
         std::atomic<uint64_t> generation_;
     };
     ```

2. **Create `src/core/ValidatedRef.h` / `ValidatedRef.cpp`**:
   ```cpp
   namespace markamp::core {

   // A reference that becomes invalid when the source is modified
   template<typename T>
   class ValidatedRef {
   public:
       ValidatedRef(const T& data, const GenerationCounter& counter)
           : data_(&data)
           , counter_(&counter)
           , captured_generation_(counter.current())
       {}

       // Access the referenced data (returns error if invalidated)
       [[nodiscard]] auto get() const -> Result<std::reference_wrapper<const T>> {
           if (!is_valid()) {
               return std::unexpected(make_error(
                   ErrorCode::InvalidArgument,
                   SubsystemId::Threading,
                   "ValidatedRef: stale reference (generation mismatch)"));
           }
           return std::ref(*data_);
       }

       // Check if the reference is still valid
       [[nodiscard]] auto is_valid() const -> bool {
           return counter_->is_valid(captured_generation_);
       }

       // Get the generation this ref was created at
       [[nodiscard]] auto generation() const -> uint64_t {
           return captured_generation_;
       }

   private:
       const T* data_;
       const GenerationCounter* counter_;
       uint64_t captured_generation_;
   };

   // Immutable snapshot: copies data and is always valid
   template<typename T>
   class ImmutableSnapshot {
   public:
       explicit ImmutableSnapshot(T data)
           : data_(std::make_shared<const T>(std::move(data)))
       {}

       [[nodiscard]] auto get() const -> const T& { return *data_; }
       [[nodiscard]] auto operator->() const -> const T* { return data_.get(); }

   private:
       std::shared_ptr<const T> data_;
   };

   } // namespace markamp::core
   ```
   - `ValidatedRef<T>`: lightweight, non-owning reference with generation check
   - `ImmutableSnapshot<T>`: owning copy that is always valid (for cross-thread data sharing)
   - Both are thread-safe for reads

3. **Create `tests/unit/test_validated_ref.cpp`**:
   - TEST_CASE: "ValidatedRef get succeeds when valid"
   - TEST_CASE: "ValidatedRef get returns error after invalidation"
   - TEST_CASE: "ValidatedRef is_valid reflects current state"
   - TEST_CASE: "GenerationCounter starts at 0"
   - TEST_CASE: "GenerationCounter increment returns new value"
   - TEST_CASE: "GenerationCounter is_valid with current generation"
   - TEST_CASE: "GenerationCounter is_valid returns false after increment"
   - TEST_CASE: "ImmutableSnapshot is always valid"
   - TEST_CASE: "ImmutableSnapshot can be shared across threads"
   - TEST_CASE: "ValidatedRef works with complex types"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/GenerationCounter.h` |
| Create | `src/core/ValidatedRef.h` |
| Create | `src/core/ValidatedRef.cpp` |
| Create | `tests/unit/test_validated_ref.cpp` |

---

## Implementation Notes

- **Memory ordering**: Use `std::memory_order_acquire` for reads and `std::memory_order_release` for writes. This is sufficient for the generation counter pattern and avoids the overhead of `seq_cst`.
- **ValidatedRef is non-owning**: It stores a raw pointer to the data and the counter. It must NOT outlive the data or counter. Use cases: within a single function scope where the caller holds the owning reference.
- **ImmutableSnapshot is owning**: It makes a copy via `shared_ptr<const T>`. Use cases: passing data from one thread to another where the sender may modify the original.
- **Pattern usage**: EventBus already uses immutable snapshots for queued events. Other subsystems (search results, config snapshots, document state) should adopt these patterns.
- **Template implementation**: Goes mostly in header files. The `.cpp` file is for non-template documentation and any helper functions.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Accessing a `ValidatedRef` after source invalidation returns error (not crash or UB)
- [ ] `GenerationCounter` is atomic and lock-free
- [ ] `ValidatedRef::is_valid()` correctly reflects invalidation state
- [ ] `ImmutableSnapshot` is always valid regardless of source modifications
- [ ] `ImmutableSnapshot` can be safely shared across threads
- [ ] Works with the immutable snapshots pattern used in EventBus
- [ ] Zero overhead for `is_valid()` check (single atomic load)
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test ValidatedRef with single-threaded invalidation
- Test ValidatedRef with multi-threaded access (one writer, one reader)
- Test ImmutableSnapshot independence from source
- Test GenerationCounter atomic operations
- Stress test: rapid increment + is_valid checks from multiple threads
- Run under TSan to verify no data races
