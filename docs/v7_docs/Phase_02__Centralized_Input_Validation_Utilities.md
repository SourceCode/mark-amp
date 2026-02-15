# Phase 02: Centralized Input Validation Utilities

## Metadata

| Field | Value |
|---|---|
| Phase ID | 02 |
| Prerequisites | Phase 01 |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 0 modified, 1 test |
| PRD Sections | PI-7 (centralized validators), PI-8 (all external data untrusted), PI-14 (enum validation) |

---

## Objective

Build the shared validation library that all input-facing code will use. Replaces scattered validation logic with consistent, tested validators for paths, YAML keys, strings, numeric values, and enum strings. Every validator returns `Result<T>` from Phase 01.

---

## Background

The PRD mandates centralized input validation utilities (PI-7) and treating all external data as untrusted (PI-8). Currently validation logic is scattered across individual services with inconsistent error handling. This phase provides a single namespace of validators that Phases 07-14 build upon.

---

## Scope

### Tasks

1. **Create `src/core/Validation.h` / `Validation.cpp`**:
   - Namespace `markamp::core::validation`
   - All validators return `Result<T>` (from Phase 01)
   - Validators:
     ```cpp
     auto validate_string_length(std::string_view s, size_t max_len,
                                 std::string_view field_name = "") -> Result<std::string_view>;

     auto validate_yaml_key(std::string_view key) -> Result<std::string>;

     auto validate_numeric_range(int value, int min, int max,
                                 std::string_view field_name = "") -> Result<int>;

     auto validate_numeric_range(double value, double min, double max,
                                 std::string_view field_name = "") -> Result<double>;

     auto validate_enum_string(std::string_view value,
                               std::span<const std::string_view> valid_values,
                               std::string_view field_name = "") -> Result<std::string_view>;

     auto validate_not_empty(std::string_view s,
                             std::string_view field_name = "") -> Result<std::string_view>;

     auto validate_utf8(std::string_view s) -> Result<std::string_view>;
     ```
   - YAML key validation rules: max 256 chars, alphanumeric + `_` + `-` + `.`, no control characters
   - Enum validation: case-insensitive comparison option

2. **Create `src/core/PathValidator.h` / `PathValidator.cpp`**:
   - Namespace `markamp::core::validation`
   - Path validators:
     ```cpp
     auto validate_path(std::string_view path,
                        std::string_view workspace_root) -> Result<std::filesystem::path>;

     auto canonicalize_path(const std::filesystem::path& path,
                            const std::filesystem::path& root) -> Result<std::filesystem::path>;

     auto is_within_boundary(const std::filesystem::path& path,
                             const std::filesystem::path& boundary) -> bool;

     auto detect_symlink_escape(const std::filesystem::path& path,
                                const std::filesystem::path& boundary) -> Result<std::filesystem::path>;
     ```
   - All filesystem operations use `std::error_code` overloads (no exceptions)
   - Rejects: `..` traversal outside workspace, symlink escapes, null bytes in paths
   - Platform-aware: handles case-insensitive HFS+ (macOS), case-sensitive ext4 (Linux), NTFS (Windows)

3. **Create `tests/unit/test_validation.cpp`**:
   - TEST_CASE: "validate_string_length accepts valid strings"
   - TEST_CASE: "validate_string_length rejects oversized strings"
   - TEST_CASE: "validate_yaml_key accepts valid keys"
   - TEST_CASE: "validate_yaml_key rejects control characters"
   - TEST_CASE: "validate_yaml_key rejects oversized keys"
   - TEST_CASE: "validate_numeric_range accepts in-range values"
   - TEST_CASE: "validate_numeric_range rejects out-of-range values"
   - TEST_CASE: "validate_enum_string accepts valid enum values"
   - TEST_CASE: "validate_enum_string rejects unknown values"
   - TEST_CASE: "validate_path rejects traversal attacks"
   - TEST_CASE: "validate_path accepts valid workspace paths"
   - TEST_CASE: "canonicalize_path resolves relative paths"
   - TEST_CASE: "detect_symlink_escape catches escapes"
   - TEST_CASE: "validate_not_empty rejects empty strings"
   - TEST_CASE: "validate_utf8 rejects invalid UTF-8 sequences"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/Validation.h` |
| Create | `src/core/Validation.cpp` |
| Create | `src/core/PathValidator.h` |
| Create | `src/core/PathValidator.cpp` |
| Create | `tests/unit/test_validation.cpp` |

---

## Implementation Notes

- Path validation is security-critical. Use `std::filesystem::weakly_canonical()` with `std::error_code` overload to avoid exceptions on broken symlinks.
- For symlink escape detection: resolve the canonical path and verify it starts with the boundary path.
- YAML key character validation: allow `[a-zA-Z0-9_\-\.]` only. This covers standard YAML frontmatter keys used in MarkAmp themes and configs.
- For enum validation with case-insensitive mode, convert both sides to lowercase before comparison.
- `validate_utf8` should check for valid UTF-8 encoding without external dependencies (use a simple state machine).
- All error messages should include the `field_name` parameter when provided, making diagnostics actionable.
- Update `src/CMakeLists.txt` to add both .cpp files.
- Update `tests/CMakeLists.txt` to add test target.

---

## Acceptance Criteria

- [ ] Path traversal attacks (`../../etc/passwd`) return error with `ErrorCode::PathTraversal`
- [ ] Symlink escapes detected on all platforms
- [ ] YAML keys validated for length (max 256) and character set
- [ ] Enum string validation rejects unknown values with clear error message
- [ ] Numeric range validation rejects out-of-range values
- [ ] Empty string validation catches empty and whitespace-only inputs
- [ ] UTF-8 validation catches invalid byte sequences
- [ ] All error results include actionable field names
- [ ] All 15+ test cases pass
- [ ] No filesystem exceptions thrown (all use error_code overloads)

---

## Testing Strategy

- Unit tests for each validator with valid and invalid inputs
- Path validation tested with platform-specific edge cases (symlinks, case sensitivity)
- Fuzzy inputs tested: null bytes, extremely long strings, binary data
- Verify error messages include field names for diagnostics
- Run under ASan to verify no buffer overflows in path handling
