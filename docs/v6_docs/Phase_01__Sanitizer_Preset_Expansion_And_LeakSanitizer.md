# Phase 01: Sanitizer Preset Expansion & LeakSanitizer

## Metadata

| Field | Value |
|---|---|
| Phase ID | 01 |
| Prerequisites | None |
| Estimated Complexity | Low |
| Estimated File Count | 3 modified |
| PRD Sections | 6.1 Compiler Sanitizers, 9 Build Configurations |

---

## Objective

Ensure all four sanitizers (ASan, UBSan, TSan, LSan) are available as CMake options and can be combined in CI matrix builds. This is the foundation for all subsequent stability and correctness work.

---

## Background

The PRD mandates AddressSanitizer, UndefinedBehaviorSanitizer, ThreadSanitizer, and LeakSanitizer in CI. The existing `cmake/Sanitizers.cmake` supports ASan, UBSan, and TSan, but not standalone LeakSanitizer. CMakePresets.json currently only has `debug`, `release`, and `release-static` presets.

---

## Scope

### Tasks

1. **Update `cmake/Sanitizers.cmake`**:
   - Add `MARKAMP_ENABLE_LSAN` option for standalone LeakSanitizer (`-fsanitize=leak`)
   - Add mutual exclusivity guard: ASan and TSan cannot be combined (FATAL_ERROR if both enabled)
   - Ensure LSan standalone works on Linux (macOS uses ASan's built-in leak checking)
   - Add compiler/linker flag propagation for all sanitizer combinations

2. **Add sanitizer presets to `CMakePresets.json`**:
   - `debug-asan`: ASan + UBSan combined (`-fsanitize=address,undefined`)
   - `debug-tsan`: ThreadSanitizer only (`-fsanitize=thread`)
   - `debug-lsan`: LeakSanitizer standalone (`-fsanitize=leak`)
   - Each preset inherits from the `debug` base preset
   - Set `MARKAMP_ENABLE_ASAN`, `MARKAMP_ENABLE_TSAN`, `MARKAMP_ENABLE_LSAN` cache variables appropriately

3. **Update root `CMakeLists.txt`**:
   - Ensure `include(cmake/Sanitizers.cmake)` is called after project() but before targets
   - Pass sanitizer flags to all targets via interface library or global flags

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `cmake/Sanitizers.cmake` |
| Modify | `CMakePresets.json` |
| Modify | `CMakeLists.txt` |

---

## Implementation Notes

- TSan and ASan are mutually exclusive at the Clang/GCC level. The CMake config must enforce this with a `message(FATAL_ERROR ...)` if both are enabled simultaneously.
- LSan is built into ASan on most platforms, so standalone LSan is mainly useful for catching leaks without the overhead of full ASan.
- On macOS, standalone LSan may not be available (it's integrated with ASan). The preset should document this limitation.
- All sanitizer presets must set `-fno-omit-frame-pointer` for accurate stack traces.

---

## Acceptance Criteria

- [ ] `cmake --preset debug-asan` configures without error
- [ ] `cmake --preset debug-tsan` configures without error
- [ ] `cmake --preset debug-lsan` configures without error
- [ ] Attempting to enable both ASan and TSan simultaneously produces `FATAL_ERROR`
- [ ] All existing unit tests pass under `debug-asan` preset
- [ ] Sanitizer flags appear in `CMAKE_CXX_FLAGS` when enabled (verify via cmake cache)
- [ ] `-fno-omit-frame-pointer` is set for all sanitizer presets

---

## Testing Strategy

- Configure each preset and verify cmake cache contains expected flags
- Build and run `ctest --output-on-failure` under `debug-asan`
- Verify that a known leak (intentional test) is caught by LSan on Linux
