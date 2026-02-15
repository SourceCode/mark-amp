# Phase 16: Safe Allocation and Pointer Discipline

## Metadata

| Field | Value |
|---|---|
| Phase ID | 16 |
| Prerequisites | Phase 01 (Result types) |
| Estimated Complexity | Low |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-17 (safe allocation wrapper), PI-18 (no raw owning pointers), PI-22 (TextSpan validation) |

---

## Objective

Add allocation failure guards at subsystem boundaries with `safe_make_unique<T>()` and `safe_make_shared<T>()` that catch `std::bad_alloc` and return `Result<T>`. Enhance `TextSpan` with generation counter validation to prevent use-after-free.

---

## Background

The PRD mandates safe allocation wrappers (PI-17), no raw owning pointers (PI-18), and TextSpan lifetime validation (PI-22). While modern allocators rarely fail on desktop systems, OOM can occur under memory pressure (large files, many extensions). The allocation wrappers provide a consistent pattern for handling this. TextSpan generation validation prevents accessing spans that reference freed buffer storage.

---

## Scope

### Tasks

1. **Create `src/core/SafeAlloc.h` / `SafeAlloc.cpp`**:
   ```cpp
   namespace markamp::core {

   // Safe allocation wrappers that catch bad_alloc
   template<typename T, typename... Args>
   [[nodiscard]] auto safe_make_unique(Args&&... args) -> Result<std::unique_ptr<T>>;

   template<typename T, typename... Args>
   [[nodiscard]] auto safe_make_shared(Args&&... args) -> Result<std::shared_ptr<T>>;

   // Safe vector reserve that catches bad_alloc
   template<typename T>
   [[nodiscard]] auto safe_reserve(std::vector<T>& vec, size_t count) -> Result<void>;

   // Safe string resize that catches bad_alloc
   [[nodiscard]] auto safe_string_resize(std::string& str, size_t count) -> Result<void>;

   } // namespace markamp::core
   ```
   - Catch `std::bad_alloc` and return `Error` with `ErrorCode::AllocationFailed`
   - Error includes requested size (via `sizeof(T)` or count parameter)
   - Log `MARKAMP_LOG_ERROR` on allocation failure

2. **Modify `src/core/TextSpan.h`**:
   - Add generation counter to TextSpan:
     ```cpp
     struct TextSpan {
         size_t start;
         size_t length;
         uint64_t generation;  // Must match buffer's current generation

         [[nodiscard]] auto is_valid(uint64_t current_generation) const -> bool {
             return generation == current_generation;
         }
     };
     ```
   - Add validation method that checks generation before dereferencing
   - If generation mismatch: return error instead of accessing potentially freed memory

3. **Create `tests/unit/test_safe_alloc.cpp`**:
   - TEST_CASE: "safe_make_unique creates valid unique_ptr"
   - TEST_CASE: "safe_make_shared creates valid shared_ptr"
   - TEST_CASE: "safe_make_unique with constructor arguments"
   - TEST_CASE: "safe_reserve succeeds with reasonable size"
   - TEST_CASE: "safe_string_resize succeeds with reasonable size"
   - TEST_CASE: "TextSpan is_valid with matching generation"
   - TEST_CASE: "TextSpan is_valid with mismatched generation"
   - TEST_CASE: "TextSpan zero-length span is valid"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/SafeAlloc.h` |
| Create | `src/core/SafeAlloc.cpp` |
| Modify | `src/core/TextSpan.h` |
| Create | `tests/unit/test_safe_alloc.cpp` |

---

## Implementation Notes

- **Template implementation**: `safe_make_unique` and `safe_make_shared` are template functions. Implementation goes in the header file. The `.cpp` file contains non-template helpers (logging, error construction).
- **Testing bad_alloc**: Testing actual allocation failure is difficult in unit tests. Test the wrapper logic by verifying it returns `Result<T>` correctly. Bad_alloc testing is better done in Phase 40 (chaos testing) with memory pressure simulation.
- **TextSpan generation**: The existing `GenerationCounter.h` in the codebase provides the generation counter mechanism. TextSpan's `generation` field should match the generation of the buffer it references. When the buffer is modified, its generation increments, and all outstanding TextSpans become invalid.
- **Pointer audit**: While this phase provides the safe allocation wrappers, a full audit of raw owning pointers is a code review task. Document the convention: "All new code must use `unique_ptr`/`shared_ptr` for ownership. Raw pointers are only for non-owning references."
- **Performance**: `safe_make_unique/shared` have zero overhead in the success path (no extra branches in the hot path). The try/catch only activates on `bad_alloc`.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] `safe_make_unique<T>()` returns `Result<std::unique_ptr<T>>` on success
- [ ] `safe_make_unique<T>()` catches `bad_alloc` and returns error
- [ ] `safe_make_shared<T>()` returns `Result<std::shared_ptr<T>>` on success
- [ ] `safe_reserve()` catches `bad_alloc` on vector reserve
- [ ] Error includes `ErrorCode::AllocationFailed` with size information
- [ ] TextSpan validates generation counter before access
- [ ] TextSpan with mismatched generation returns `is_valid() == false`
- [ ] No new raw owning pointers introduced in this phase
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test safe allocation wrappers with various types (POD, class with constructor, move-only)
- Test TextSpan generation validation with matching and mismatched generations
- Test safe_reserve with zero size, small size, and large size
- Verify error messages include requested allocation size
- Run under ASan to verify no memory issues
- Bad_alloc simulation tested in Phase 40 (chaos testing)
