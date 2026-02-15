# Phase 11: Numeric Range Enforcement

## Metadata

| Field | Value |
|---|---|
| Phase ID | 11 |
| Prerequisites | Phase 01 (Result types) |
| Estimated Complexity | Low |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-13 (numeric range enforcement), PI-20 (integer overflow guards), PI-21 (underflow prevention) |

---

## Objective

Clamp all user-configurable numeric values to valid ranges and provide checked arithmetic utilities (`checked_add`, `checked_multiply`, `safe_subtract`) that prevent integer overflow and unsigned underflow.

---

## Background

The PRD mandates numeric range enforcement (PI-13), integer overflow guards (PI-20), and underflow prevention (PI-21). v1.7.10 added some arithmetic underflow guards, but coverage is incomplete. User-configurable values like font_size, tab_size, and word_wrap_column need to be clamped to sane ranges before use. Arithmetic overflow in buffer indexing and offset math can cause memory corruption.

---

## Scope

### Tasks

1. **Create `src/core/NumericGuards.h` / `NumericGuards.cpp`**:
   ```cpp
   namespace markamp::core {

   // Clamp a config value to a valid range, logging if clamped
   template<typename T>
   [[nodiscard]] auto clamp_config(T value, T min, T max,
                                    std::string_view config_key = "") -> T;

   // Checked arithmetic — returns error on overflow
   [[nodiscard]] auto checked_add(int64_t a, int64_t b) -> Result<int64_t>;
   [[nodiscard]] auto checked_add(size_t a, size_t b) -> Result<size_t>;
   [[nodiscard]] auto checked_multiply(int64_t a, int64_t b) -> Result<int64_t>;
   [[nodiscard]] auto checked_multiply(size_t a, size_t b) -> Result<size_t>;

   // Safe unsigned subtraction — returns 0 instead of wrap-around
   [[nodiscard]] auto safe_subtract(size_t a, size_t b) -> size_t;
   [[nodiscard]] auto safe_subtract(unsigned int a, unsigned int b) -> unsigned int;

   // Narrowing conversion guards
   [[nodiscard]] auto safe_narrow(int64_t value) -> Result<int>;
   [[nodiscard]] auto safe_narrow(size_t value) -> Result<int>;

   } // namespace markamp::core
   ```
   - `clamp_config()`: clamps value to [min, max], logs `MARKAMP_LOG_WARN` with config_key if clamped
   - `checked_add()`: returns error with `ErrorCode::InvalidArgument` on overflow
   - `checked_multiply()`: returns error on overflow
   - `safe_subtract()`: returns 0 when b > a (no wrap-around)
   - `safe_narrow()`: returns error if value doesn't fit in target type

   **Config clamping ranges:**
   | Setting | Min | Max |
   |---|---|---|
   | font_size | 6 | 128 |
   | tab_size | 1 | 16 |
   | edge_column | 1 | 500 |
   | auto_save_interval_seconds | 5 | 3600 |
   | cursor_width | 1 | 10 |
   | word_wrap_column | 20 | 1000 |
   | letter_spacing | -5.0 | 20.0 |
   | line_height | 0.5 | 5.0 |
   | zoom_level | 25 | 500 |

2. **Modify `src/core/Config.cpp`**:
   - Apply `clamp_config()` in `get_int()` and `get_double()` for known config keys
   - Create a static map of config key -> (min, max) pairs
   - Unknown config keys pass through unclamped

3. **Create `tests/unit/test_numeric_guards.cpp`**:
   - TEST_CASE: "clamp_config preserves in-range value"
   - TEST_CASE: "clamp_config clamps below minimum"
   - TEST_CASE: "clamp_config clamps above maximum"
   - TEST_CASE: "clamp_config logs warning on clamp"
   - TEST_CASE: "checked_add returns correct sum"
   - TEST_CASE: "checked_add returns error on INT64_MAX overflow"
   - TEST_CASE: "checked_add returns error on SIZE_MAX overflow"
   - TEST_CASE: "checked_multiply returns correct product"
   - TEST_CASE: "checked_multiply returns error on overflow"
   - TEST_CASE: "safe_subtract returns difference when a > b"
   - TEST_CASE: "safe_subtract returns 0 when b > a"
   - TEST_CASE: "safe_narrow converts valid value"
   - TEST_CASE: "safe_narrow rejects out-of-range value"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/NumericGuards.h` |
| Create | `src/core/NumericGuards.cpp` |
| Modify | `src/core/Config.cpp` |
| Create | `tests/unit/test_numeric_guards.cpp` |

---

## Implementation Notes

- **Overflow detection for signed integers**: Use `__builtin_add_overflow()` / `__builtin_mul_overflow()` on GCC/Clang (available in all supported compilers). These are the most efficient and correct way to detect overflow.
- **Overflow detection fallback**: For MSVC, use `_addcarry_u64` or manual check: `if (a > 0 && b > INT64_MAX - a)`.
- **Unsigned subtraction**: Simply `return (a >= b) ? (a - b) : 0;`. This eliminates the entire class of unsigned underflow bugs.
- **Template clamp_config**: Works for int, double, float. The logging uses `MARKAMP_LOG_WARN` with the config_key to make diagnostics actionable.
- **Config integration**: The clamping in Config.cpp should happen at the point of retrieval (`get_int`, `get_double`), not at storage (`set`). This ensures values are always safe when used, even if they were stored before clamping was added.
- **safe_narrow**: Use `std::numeric_limits<int>::min()` and `std::numeric_limits<int>::max()` for range checking.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] `font_size=0` clamped to 6 with log warning
- [ ] `font_size=9999` clamped to 128 with log warning
- [ ] `checked_add(INT64_MAX, 1)` returns overflow error
- [ ] `checked_multiply(INT64_MAX, 2)` returns overflow error
- [ ] `safe_subtract(0u, 1u)` returns 0 (not wrap-around to SIZE_MAX)
- [ ] `safe_narrow(INT64_MAX)` returns error (doesn't fit in int)
- [ ] Existing config values unchanged if within valid range
- [ ] All clamping ranges documented in centralized table
- [ ] All 12+ test cases pass

---

## Testing Strategy

- Test each arithmetic function with normal values, boundary values, and overflow-triggering values
- Test clamp_config with values below min, above max, and within range
- Verify logging output on clamped values
- Test safe_subtract with a=0, b=1 to verify no wrap-around
- Test safe_narrow with edge cases (INT_MIN, INT_MAX, 0, negative values)
- Run under UBSan to verify no undefined behavior in arithmetic operations
