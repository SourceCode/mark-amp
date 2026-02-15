# Phase 15: Dead Code Elimination & Translation Unit Splitting

## Metadata

| Field | Value |
|---|---|
| Phase ID | 15 |
| Prerequisites | Phase 11 |
| Estimated Complexity | Medium |
| Estimated File Count | 1 created, 4+ modified |
| PRD Sections | 3.5 Dead Code Elimination |

---

## Objective

Identify and remove dead code, split oversized translation units, and verify that unused plugin code is not linked. Reduce binary size and improve build times.

---

## Background

The PRD mandates: "Ensure unused plugin code is not eagerly linked. Avoid RTTI-heavy polymorphic hierarchies on startup path. Split large modules into smaller translation units." Dead code increases binary size, degrades instruction cache performance, and increases link times. Oversized translation units slow incremental builds.

---

## Scope

### Tasks

1. **Create `scripts/dead_code_report.sh`**:
   - Build the Release binary with `-fdata-sections -ffunction-sections` and `--gc-sections`
   - Analyze link map output for unreferenced symbols
   - Cross-reference symbols with source files
   - Report unreachable functions and unused data
   - Produce sorted report by section size

2. **Audit `PluginContext.h` header includes**:
   - Replace direct includes with forward declarations where possible
   - PluginContext.h likely includes all 21+ service headers — heavy include chain
   - Move service type definitions to implementation files
   - Use `class ServiceName;` forward declarations in the header
   - This reduces compilation dependencies and speeds up incremental builds

3. **Split large translation units** (>1000 lines):
   - Identify candidates via line count analysis
   - Common targets: `MarkAmpApp.cpp`, `MainFrame.cpp`, `EditorPanel.cpp`
   - Split into focused .cpp files grouped by functionality
   - Example: `MarkAmpApp.cpp` -> `MarkAmpApp.cpp` (init) + `MarkAmpApp_Commands.cpp` (command handlers)

4. **Verify linker dead code stripping**:
   - Ensure `-fdata-sections -ffunction-sections` is set in `cmake/LTO.cmake` (Phase 04)
   - Ensure `--gc-sections` (or `-Wl,--gc-sections`) is set as linker flag
   - On macOS: `-dead_strip` linker flag
   - Verify unused functions are stripped from final binary

5. **Remove identified dead code**:
   - Delete unreachable functions found by link map analysis
   - Remove unused includes identified by `include-what-you-use` (if available)
   - Update CMakeLists.txt source lists

6. **Update `src/CMakeLists.txt`**:
   - Add any new .cpp files from splitting
   - Remove any deleted files
   - Keep source_group() synchronized

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `scripts/dead_code_report.sh` |
| Modify | `src/core/PluginContext.h` |
| Modify | `src/CMakeLists.txt` |
| Modify | `cmake/LTO.cmake` |
| Modify | Various source files (split or trimmed) |

---

## Implementation Notes

- On macOS, the linker supports `-dead_strip` to remove unreachable code. GCC/Clang on Linux use `--gc-sections`.
- The link map can be generated with `-Wl,-Map,output.map` (Linux) or `-Wl,-map,output.map` (macOS).
- `include-what-you-use` (iwyu) can identify unnecessary includes but may require manual review of suggestions (it can be overly aggressive with wxWidgets headers).
- Forward declarations in PluginContext.h are safe because PluginContext stores pointers (`T*`), not values. Pointers to incomplete types are valid in C++.
- When splitting translation units, keep related functionality together. Don't split too aggressively — aim for 200-600 lines per file.
- After removing dead code, verify that the full test suite still passes.

---

## Acceptance Criteria

- [ ] `dead_code_report.sh` produces a report with identified unreferenced symbols
- [ ] PluginContext.h uses forward declarations (fewer direct includes)
- [ ] No translation unit in `src/core/` exceeds 1000 lines
- [ ] Binary size does not increase (ideally decreases by >5%)
- [ ] `-fdata-sections -ffunction-sections` and dead code stripping enabled in Release/LTO builds
- [ ] All tests pass after dead code removal
- [ ] `src/CMakeLists.txt` is synchronized with actual source files

---

## Testing Strategy

- Run full test suite after each change
- Compare binary size before and after
- Run dead_code_report.sh and verify reduced symbol count
- Verify PluginContext.h compiles correctly with forward declarations
- Build with LTO and verify dead stripping is active
