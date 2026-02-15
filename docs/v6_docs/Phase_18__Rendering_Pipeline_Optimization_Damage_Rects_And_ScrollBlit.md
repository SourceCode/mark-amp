# Phase 18: Rendering Pipeline Optimization -- Damage Rects & ScrollBlit

## Metadata

| Field | Value |
|---|---|
| Phase ID | 18 |
| Prerequisites | Phase 09 |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 5 modified |
| PRD Sections | 4.5 Rendering Performance |

---

## Objective

Wire the existing DirtyRegionAccumulator and ScrollBlit infrastructure into the actual rendering pipeline to eliminate full-viewport repaints. Single-character edits must not repaint the entire viewport.

---

## Background

The PRD mandates: "Damage-rect merging, ScrollBlit first repaint second, Avoid full layout invalidation." And: "Never recompute full layout on minor edits, never re-tokenize entire document on single-line changes." The codebase already has DirtyRegionAccumulator, ScrollBlit, IncrementalLineWrap, ViewportCache, and DoubleBufferedPaint infrastructure, but they may not be fully wired into the rendering pipeline.

---

## Scope

### Tasks

1. **Integrate DirtyRegionAccumulator into EditorPanel/PreviewPanel**:
   - On text edit: invalidate only the affected line range (not entire viewport)
   - DirtyRegionAccumulator merges overlapping dirty rects
   - Paint handler checks dirty region and only repaints affected area
   - Track dirty state per line: `std::vector<bool> line_dirty_`

2. **Integrate ScrollBlit into scroll handling**:
   - On scroll: use ScrollBlit to bitblit existing content to new position
   - Only paint the newly revealed strip (top or bottom)
   - Avoid full repaint on scroll operations
   - Handle scroll by N lines: blit N-lines worth, paint revealed strip

3. **Integrate IncrementalLineWrap**:
   - On edit: re-wrap only changed lines, not entire document
   - Line wrap changes invalidate affected lines and all subsequent lines (if line count changes)
   - Optimize: if line count doesn't change, only invalidate the edited line

4. **Wire ViewportCache into paint path**:
   - Only render lines in visible range + prefetch margin (e.g., +/- 10 lines)
   - Skip rendering for lines completely outside viewport
   - Maintain rendered line cache for off-screen prefetched lines

5. **Ensure DoubleBufferedPaint is used for all paint operations**:
   - All paint goes through double buffer to prevent flicker
   - Damage rects applied to the off-screen buffer
   - Final blit copies only changed regions to screen

6. **Create `benchmarks/bench_rendering_pipeline.cpp`**:
   - `BM_Render_SingleCharEdit`: frame time for single character insertion
   - `BM_Render_ScrollOneLine`: frame time for one-line scroll
   - `BM_Render_FullRepaint`: frame time for full viewport repaint (baseline)
   - `BM_Render_ThemeChange`: frame time for theme switch (full repaint)
   - Parameterize by document size

7. **Create `tests/unit/test_incremental_rendering.cpp`**:
   - Test that single-line edit only invalidates affected region
   - Test that scroll blit produces correct content
   - Test damage rect merging with overlapping regions
   - Test viewport cache correctly limits rendering scope

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/ui/EditorPanel.cpp` |
| Modify | `src/ui/PreviewPanel.cpp` |
| Modify | `src/rendering/HtmlRenderer.cpp` |
| Create | `benchmarks/bench_rendering_pipeline.cpp` |
| Create | `tests/unit/test_incremental_rendering.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- DirtyRegionAccumulator should merge adjacent dirty rects that are within a threshold (e.g., 5 pixels) to avoid many small repaints which are worse than one larger one.
- ScrollBlit uses `wxDC::Blit()` or platform-native scroll APIs for efficient content shifting.
- The viewport cache should maintain a sliding window of rendered lines. When the viewport scrolls, slide the window and only render newly visible lines.
- Double buffering: use `wxBufferedPaintDC` or a custom off-screen bitmap. Paint to the bitmap, then blit to screen.
- For PreviewPanel (HTML preview), damage tracking may be coarser (repaint entire preview on content change, but not on scroll).
- The key optimization target is EditorPanel: typing a single character should repaint at most 1-3 lines.

---

## Acceptance Criteria

- [ ] Single-character edit does NOT repaint entire viewport (verified by dirty region count in benchmark)
- [ ] Scroll operations use blit + strip paint (verified by dirty region count)
- [ ] `bench_rendering_pipeline` shows >50% frame time reduction for small edits vs full repaint
- [ ] `test_incremental_rendering` validates damage rect correctness
- [ ] No visual artifacts from incremental rendering
- [ ] ViewportCache correctly limits rendering to visible + prefetch lines
- [ ] Double buffering prevents visible flicker

---

## Testing Strategy

- Run bench_rendering_pipeline and compare single-edit vs full-repaint timings
- Run test_incremental_rendering for unit validation
- Run full test suite for regression checking
- Manual visual inspection for rendering artifacts
- Run under ASan to detect any buffer issues in blit operations
