# Phase 10: Safe Regex Execution

## Metadata

| Field | Value |
|---|---|
| Phase ID | 10 |
| Prerequisites | Phase 01 (Result types) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PI-12 (safe regex execution) |

---

## Objective

Wrap all regex operations with timeout guards and exception handling to prevent catastrophic backtracking from hanging the application. Provide `safe_regex_match()`, `safe_regex_search()`, and `safe_regex_replace()` that return `Result<T>` with configurable timeout.

---

## Background

The PRD mandates safe regex execution with timeout and exception guards (PI-12). `std::regex` is known for catastrophic backtracking on certain patterns (e.g., `(a+)+b` on long strings of 'a'). Currently MarkAmp uses regex in `IncrementalSearcher` and `SearchQueryParser` without any protection against pathological patterns. A user-supplied search regex could hang the entire application.

---

## Scope

### Tasks

1. **Create `src/core/SafeRegex.h` / `SafeRegex.cpp`**:
   ```cpp
   namespace markamp::core {

   // Default timeout for regex operations
   constexpr auto kDefaultRegexTimeout = std::chrono::milliseconds{100};

   // Safe regex match with timeout
   [[nodiscard]] auto safe_regex_match(
       std::string_view input,
       const std::regex& pattern,
       std::chrono::milliseconds timeout = kDefaultRegexTimeout
   ) -> Result<bool>;

   // Safe regex search with timeout
   [[nodiscard]] auto safe_regex_search(
       std::string_view input,
       const std::regex& pattern,
       std::chrono::milliseconds timeout = kDefaultRegexTimeout
   ) -> Result<std::optional<std::smatch>>;

   // Safe regex replace with timeout
   [[nodiscard]] auto safe_regex_replace(
       std::string_view input,
       const std::regex& pattern,
       std::string_view replacement,
       std::chrono::milliseconds timeout = kDefaultRegexTimeout
   ) -> Result<std::string>;

   // Safe regex compilation (catches std::regex_error)
   [[nodiscard]] auto safe_regex_compile(
       std::string_view pattern,
       std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript
   ) -> Result<std::regex>;

   } // namespace markamp::core
   ```
   - Timeout implementation: run regex in a timed scope, check elapsed time periodically
   - Since `std::regex` does not support interruption, use a background thread with `std::future` and `wait_for()` timeout
   - On timeout: return error with `ErrorCode::Timeout`
   - On `std::regex_error`: catch and return error with `ErrorCode::InvalidArgument`
   - `safe_regex_compile()`: catches `std::regex_error` during pattern compilation

2. **Modify `src/core/IncrementalSearcher.cpp`**:
   - Replace `std::regex_search()` calls with `safe_regex_search()`
   - Handle timeout error: show "Search timeout" in UI, do not hang

3. **Modify `src/core/SearchQueryParser.cpp`**:
   - Replace `std::regex_match()` calls with `safe_regex_match()`
   - Handle timeout error: reject pattern with user-visible message

4. **Create `tests/unit/test_safe_regex.cpp`**:
   - TEST_CASE: "safe_regex_match returns true for matching pattern"
   - TEST_CASE: "safe_regex_match returns false for non-matching pattern"
   - TEST_CASE: "safe_regex_search finds match"
   - TEST_CASE: "safe_regex_search returns empty for no match"
   - TEST_CASE: "safe_regex_replace performs replacement"
   - TEST_CASE: "safe_regex_compile catches invalid pattern"
   - TEST_CASE: "catastrophic backtracking pattern returns timeout error"
   - TEST_CASE: "normal patterns complete within timeout"
   - TEST_CASE: "timeout error includes ErrorCode::Timeout"
   - TEST_CASE: "regex_error wrapped in Result error"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/SafeRegex.h` |
| Create | `src/core/SafeRegex.cpp` |
| Modify | `src/core/IncrementalSearcher.cpp` |
| Modify | `src/core/SearchQueryParser.cpp` |
| Create | `tests/unit/test_safe_regex.cpp` |

---

## Implementation Notes

- **Timeout via std::async**: Launch regex operation in `std::async(std::launch::async, ...)` and use `future.wait_for(timeout)`. If timeout expires, the future is abandoned. This is not ideal (the thread continues running) but is the only portable way to enforce timeout on `std::regex`.
- **Alternative approach**: For production optimization, consider using `re2` (Google's regex library) which guarantees linear-time execution. This can be a future enhancement. For now, the `std::async` timeout is sufficient.
- **Thread overhead**: Creating a thread per regex call is expensive. For the search-as-you-type path, consider caching the `std::async` result and cancelling previous searches. This optimization is within scope if needed.
- **Catastrophic backtracking test**: Use pattern `(a+)+b` on input `"aaaaaaaaaaaaaaaaaaaaa"` (no trailing 'b'). This should trigger timeout with `std::regex`.
- **Error messages**: Include pattern summary (truncated to 50 chars) in timeout error for diagnostics.
- **`safe_regex_compile()`**: This is important for user-supplied patterns. Compilation itself can throw `std::regex_error` for invalid patterns.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Catastrophic backtracking pattern (e.g., `(a+)+b` on `"aaaa..."`) returns timeout error
- [ ] `std::regex_error` caught and wrapped in `Result` error
- [ ] Default timeout of 100ms per operation
- [ ] `safe_regex_compile()` catches invalid patterns at compile time
- [ ] Existing search functionality unchanged for normal patterns
- [ ] IncrementalSearcher uses safe regex (no more raw `std::regex_search`)
- [ ] SearchQueryParser uses safe regex (no more raw `std::regex_match`)
- [ ] Timeout error includes `ErrorCode::Timeout`
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test with normal patterns to verify correctness is preserved
- Test with known catastrophic backtracking patterns to verify timeout
- Test with invalid patterns to verify compile-time error handling
- Test timeout boundary: pattern that takes ~50ms should succeed, ~200ms should timeout at 100ms
- Run under TSan to verify thread safety of async approach
- Run under ASan to verify no memory issues with abandoned futures
