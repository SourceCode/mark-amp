# Phase 04: LTO Build Presets & Release Optimization Flags

## Metadata

| Field | Value |
|---|---|
| Phase ID | 04 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 1 created, 2 modified |
| PRD Sections | 3.4 Link-Time Optimization, 9 Build Configurations |

---

## Objective

Add Release+LTO and Profile build configurations as specified in PRD section 9. Enable ThinLTO for Clang, regular LTO for GCC, and whole-program optimization for MSVC.

---

## Background

The PRD requires four build configurations: Debug + Sanitizers, Release, Release + LTO, and Profile build (Tracy enabled). The project currently has `debug`, `release`, and `release-static` presets but is missing `release-lto` and `profile`. LTO can significantly reduce binary size and improve performance through cross-translation-unit optimization.

---

## Scope

### Tasks

1. **Create `cmake/LTO.cmake` module**:
   - `MARKAMP_ENABLE_LTO` option (OFF by default, ON for `release-lto` preset)
   - Auto-detect ThinLTO support (Clang >=6):
     - `-flto=thin` for Clang
     - `-flto` for GCC
     - `/GL` + `/LTCG` for MSVC
   - Set `CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE` when enabled
   - Prefer `lld` linker when available for faster LTO linking
   - Add `-fdata-sections -ffunction-sections` + `--gc-sections` for dead code stripping
   - Fallback to regular LTO if ThinLTO not supported

2. **Add `release-lto` preset to `CMakePresets.json`**:
   - Inherits from `release`
   - Sets `MARKAMP_ENABLE_LTO=ON`
   - Sets `CMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE`

3. **Add `profile` preset to `CMakePresets.json`**:
   - Release optimizations with debug info (`-O2 -g`)
   - Frame pointer preserved (`-fno-omit-frame-pointer`)
   - Define `MARKAMP_TRACY_ENABLED=ON` (placeholder for Phase 05)
   - Define `MARKAMP_PROFILE_BUILD=ON`

4. **Update root `CMakeLists.txt`**:
   - `include(cmake/LTO.cmake)`
   - Conditionally apply LTO settings

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `cmake/LTO.cmake` |
| Modify | `CMakeLists.txt` |
| Modify | `CMakePresets.json` |

---

## Implementation Notes

- ThinLTO produces faster link times than full LTO while providing most of the optimization benefit.
- Use `check_ipo_supported(RESULT ipo_supported)` from CMake to verify LTO is available before enabling.
- The profile preset must keep frame pointers so profilers can capture accurate call stacks.
- On macOS, the default `ld` linker supports LTO natively. On Linux, prefer `lld` for faster ThinLTO linking.
- MSVC uses `/GL` for compile and `/LTCG` for link — these are set differently than GCC/Clang flags.

---

## Acceptance Criteria

- [ ] `cmake --preset release-lto` configures and builds to completion
- [ ] `cmake --preset profile` configures and builds to completion
- [ ] LTO build produces a smaller binary than regular Release (compare file sizes)
- [ ] Profile build includes debug symbols (verify with `file` or `dwarfdump` on macOS)
- [ ] Profile build preserves frame pointers (verify `-fno-omit-frame-pointer` in flags)
- [ ] `check_ipo_supported()` is used to gracefully handle platforms without LTO
- [ ] All existing tests pass under both new presets

---

## Testing Strategy

- Build with `release-lto` and compare binary size to `release`
- Build with `profile` and verify debug symbols present
- Run test suite under both presets
- Verify cmake cache contains expected optimization flags
