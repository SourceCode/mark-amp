# Phase 35: Large File Mode

## Metadata

| Field | Value |
|---|---|
| Phase ID | 35 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 5 modified |
| PRD Sections | 9.1 Large File Mode |

---

## Objective

Implement automatic large file mode that activates when files exceed a configurable threshold. Reduce rendering and analysis scope to maintain smooth performance on large documents.

---

## Background

The PRD requires: "Automatically enable: Lazy rendering, Token window limits, Disable minimap for >X MB files, Limit diagnostics frequency." Large files (>1MB) can overwhelm the rendering pipeline, syntax highlighter, and markdown parser if processed fully. Large file mode selectively disables expensive features to maintain the 16ms frame budget.

---

## Scope

### Tasks

1. **Create `src/core/LargeFileMode.h` / `LargeFileMode.cpp`**:
   - `LargeFileMode` class:
     - `check(file_size_bytes) -> bool`: returns true if large file mode should activate
     - `is_active() -> bool`: current state for the open document
     - `activate()` / `deactivate()`: state transitions
   - Configurable thresholds (from Config):
     - `large_file.threshold_bytes`: default 1MB (1,048,576)
     - `large_file.disable_minimap_threshold_bytes`: default 5MB
     - `large_file.max_line_length_for_highlighting`: default 10,000 chars
   - Feature toggles when active:
     - `should_disable_minimap() -> bool`
     - `should_limit_token_window() -> bool`
     - `should_reduce_diagnostics_frequency() -> bool`
     - `should_disable_link_preview() -> bool`
     - `should_limit_line_highlighting(line_length) -> bool`
     - `token_window_range() -> {start_line, end_line}` (viewport + prefetch only)
     - `diagnostics_interval_ms() -> uint32_t` (1000ms in large file mode, normal otherwise)

2. **Integrate into file open path**:
   - When a file is opened, check size against threshold
   - If large file mode activates:
     - Publish `LargeFileModeEvent` with file path and mode (on/off)
     - EditorPanel subscribes and adjusts rendering behavior
     - Minimap subscribes and hides itself
     - SyntaxHighlighter subscribes and limits token window
     - PreviewPanel subscribes and reduces update frequency

3. **Add event to `Events.h`**:
   - `LargeFileModeEvent`: { file_path, is_active, file_size_bytes, threshold_bytes }

4. **Add Config defaults**:
   - `large_file.threshold_bytes`: 1048576
   - `large_file.disable_minimap_threshold_bytes`: 5242880
   - `large_file.max_line_length_for_highlighting`: 10000
   - `large_file.diagnostics_interval_ms`: 1000

5. **Create `tests/unit/test_large_file_mode.cpp`**:
   - Test activation at threshold
   - Test deactivation below threshold
   - Test feature toggle queries
   - Test token window calculation
   - Test configurable thresholds
   - Test LargeFileModeEvent publication

6. **Create `benchmarks/bench_large_file.cpp`**:
   - `BM_LargeFile_Open_10MB`: open and initial render of 10MB document
   - `BM_LargeFile_Scroll`: scroll through 10MB document
   - `BM_LargeFile_Edit`: type in 10MB document
   - `BM_LargeFile_WithMode`: same operations with large file mode active
   - `BM_LargeFile_WithoutMode`: same operations without (baseline)

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/LargeFileMode.h` |
| Create | `src/core/LargeFileMode.cpp` |
| Modify | `src/core/Events.h` |
| Modify | `src/core/Config.cpp` |
| Modify | `src/CMakeLists.txt` |
| Create | `tests/unit/test_large_file_mode.cpp` |
| Create | `benchmarks/bench_large_file.cpp` |
| Modify | `tests/CMakeLists.txt` |
| Modify | `benchmarks/CMakeLists.txt` |

---

## Implementation Notes

- File size check is cheap (`std::filesystem::file_size()`) and should be done once on file open.
- The token window should be centered on the viewport: `[viewport_start - prefetch, viewport_end + prefetch]` where prefetch is configurable (default: 100 lines).
- Lines exceeding `max_line_length_for_highlighting` should be rendered as plain text (no syntax tokens).
- Diagnostics frequency reduction: instead of running diagnostics on every keystroke, run every 1000ms in large file mode.
- Consider showing a status bar indicator when large file mode is active ("Large File Mode" badge).
- The feature should be transparent to the user — it activates automatically and deactivates when a smaller file is opened.

---

## Acceptance Criteria

- [ ] Files >1MB automatically trigger large file mode
- [ ] Minimap is disabled in large file mode (for >5MB files)
- [ ] Token window is limited to viewport + prefetch lines
- [ ] Diagnostics frequency reduced to 1Hz in large file mode
- [ ] `bench_large_file` shows 10MB document opens in <500ms with large file mode
- [ ] `test_large_file_mode` validates threshold and feature toggling
- [ ] Scrolling remains smooth (16ms frame budget) in large files
- [ ] `LargeFileModeEvent` is published on activation/deactivation
- [ ] Thresholds are configurable via Config

---

## Testing Strategy

- Run test_large_file_mode for threshold and toggle validation
- Run bench_large_file and compare with/without large file mode
- Manual test: open a 10MB file, verify minimap hidden, scrolling smooth
- Verify status bar shows large file mode indicator
