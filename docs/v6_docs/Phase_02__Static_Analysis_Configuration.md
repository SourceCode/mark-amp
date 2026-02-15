# Phase 02: Static Analysis Configuration (clang-tidy + cppcheck)

## Metadata

| Field | Value |
|---|---|
| Phase ID | 02 |
| Prerequisites | Phase 01 |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 3 modified |
| PRD Sections | 6.4 Static Analysis |

---

## Objective

Create a project-wide `.clang-tidy` configuration file and a CI-gate clang-tidy CMake integration that fails on new warnings. Establish cppcheck integration with C++23 support.

---

## Background

The PRD mandates clang-tidy, cppcheck, and include-what-you-use as mandatory static analysis with a CI gate that allows zero new warnings. Currently, no `.clang-tidy` file exists in the project, and cppcheck integration may not be configured for C++23.

---

## Scope

### Tasks

1. **Create `.clang-tidy` at project root**:
   - Enable check groups: `bugprone-*`, `clang-analyzer-*`, `cppcoreguidelines-*`, `modernize-*`, `performance-*`, `readability-*`
   - Disable checks incompatible with C++23 or wxWidgets macros:
     - `modernize-use-trailing-return-type` (already used project-wide, would be noisy)
     - `cppcoreguidelines-avoid-magic-numbers` (too noisy for initial baseline)
     - `readability-magic-numbers` (same)
     - Any checks that conflict with wxWidgets `DECLARE_EVENT_TABLE` macros
   - Set `WarningsAsErrors` to empty initially (build baseline first)
   - Configure `HeaderFilterRegex` to match project headers only (`src/.*`)

2. **Create `cmake/ClangTidy.cmake` module**:
   - `MARKAMP_ENABLE_CLANG_TIDY` option (OFF by default)
   - When enabled, sets `CMAKE_CXX_CLANG_TIDY` to `clang-tidy;--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy`
   - Optionally pass `--warnings-as-errors=*` for CI mode via `MARKAMP_CLANG_TIDY_ERRORS` option

3. **Add `debug-clang-tidy` preset to `CMakePresets.json`**:
   - Inherits from `debug`
   - Sets `MARKAMP_ENABLE_CLANG_TIDY=ON`

4. **Update `cmake/CppCheck.cmake`** (if exists):
   - Ensure `--std=c++23` flag is passed
   - Create `cppcheck-suppressions.txt` if missing

5. **Update root `CMakeLists.txt`**:
   - Include `cmake/ClangTidy.cmake`

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `.clang-tidy` |
| Create | `cmake/ClangTidy.cmake` |
| Modify | `CMakeLists.txt` |
| Modify | `cmake/CppCheck.cmake` (if exists) |
| Modify | `CMakePresets.json` |

---

## Implementation Notes

- The `.clang-tidy` file uses YAML format with `Checks`, `WarningsAsErrors`, `HeaderFilterRegex`, and `CheckOptions` keys.
- Start with `WarningsAsErrors: ''` (empty) to establish a baseline. Future phases can tighten this.
- `HeaderFilterRegex` should exclude third-party headers and generated files.
- The clang-tidy integration runs during compilation, so it adds build time. Keep it behind an option.

---

## Acceptance Criteria

- [ ] `cmake --preset debug-clang-tidy` configures and builds successfully
- [ ] clang-tidy runs on compilation and reports findings to stdout
- [ ] No clang-tidy errors in `src/core/EventBus.h` (verify baseline is clean for core files)
- [ ] `.clang-tidy` file is present at project root
- [ ] `HeaderFilterRegex` filters out third-party headers
- [ ] cppcheck runs with `--std=c++23` when enabled

---

## Testing Strategy

- Configure with `debug-clang-tidy` preset and build a subset of targets
- Verify clang-tidy output appears during compilation
- Introduce an intentional violation (unused variable) and verify it is caught
- Remove the violation and verify clean build
