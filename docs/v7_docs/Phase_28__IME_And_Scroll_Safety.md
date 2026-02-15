# Phase 28: IME and Scroll Safety

## Metadata

| Field | Value |
|---|---|
| Phase ID | 28 |
| Prerequisites | Phase 11 (numeric guards) |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 1 modified, 1 test |
| PRD Sections | PI-38 (IME defensive handling), PI-39 (safe scroll calculations) |

---

## Objective

Guard IME composition ranges against invalid offsets and clamp scroll calculations to prevent negative offsets, past-end scrolling, and fractional pixel errors. These are common sources of crashes on CJK input methods and high-DPI displays.

---

## Background

The PRD mandates IME defensive handling (PI-38) and safe scroll calculations (PI-39). IME (Input Method Editor) composition events can provide ranges that extend beyond the document bounds, especially during rapid composition on CJK input methods. Scroll offsets can go negative (overscroll), past the document end, or produce fractional pixel values that cause rendering artifacts.

---

## Scope

### Tasks

1. **Create `src/ui/IMEGuard.h` / `IMEGuard.cpp`**:
   ```cpp
   namespace markamp::ui {

   struct CompositionRange {
       size_t start;
       size_t length;
   };

   // Validate and clamp IME composition range to document bounds
   [[nodiscard]] auto clamp_composition_range(
       const CompositionRange& range,
       size_t document_length
   ) -> CompositionRange;

   // Validate IME cursor position
   [[nodiscard]] auto clamp_composition_cursor(
       size_t cursor_pos,
       const CompositionRange& range
   ) -> size_t;

   // Check if a composition range is valid
   [[nodiscard]] auto is_valid_composition(
       const CompositionRange& range,
       size_t document_length
   ) -> bool;

   } // namespace markamp::ui
   ```
   - Clamp `start` to `[0, document_length]`
   - Clamp `start + length` to not exceed `document_length`
   - Clamp cursor position to within composition range
   - Handle edge cases: empty document, composition at end of document

2. **Create `src/ui/ScrollGuard.h` / `ScrollGuard.cpp`**:
   ```cpp
   namespace markamp::ui {

   struct ScrollBounds {
       double min_x{0.0};
       double min_y{0.0};
       double max_x;
       double max_y;
   };

   // Clamp scroll offset to valid range
   [[nodiscard]] auto clamp_scroll(double offset_x, double offset_y,
                                    const ScrollBounds& bounds) -> std::pair<double, double>;

   // Snap fractional scroll to pixel boundary (for crisp rendering)
   [[nodiscard]] auto snap_to_pixel(double offset, double dpi_scale = 1.0) -> double;

   // Calculate safe scroll delta (prevents overshoot)
   [[nodiscard]] auto safe_scroll_delta(double current, double delta,
                                         double min, double max) -> double;

   // Calculate max scroll position from content and viewport size
   [[nodiscard]] auto max_scroll(double content_size, double viewport_size) -> double;

   } // namespace markamp::ui
   ```
   - Negative offset clamped to 0
   - Past-end offset clamped to max (content_size - viewport_size, minimum 0)
   - Fractional offsets snapped to nearest pixel boundary for DPI scale
   - Delta calculations prevent overshoot beyond bounds

3. **Modify `src/core/IMECompositionOverlay.h` (or equivalent IME handler)**:
   - Apply `clamp_composition_range()` before using any composition range
   - Apply `clamp_composition_cursor()` before positioning the cursor
   - Log WARN when clamping is needed (indicates potential IME bug)

4. **Create `tests/unit/test_ime_scroll_safety.cpp`**:
   - TEST_CASE: "IME range clamped when past document end"
   - TEST_CASE: "IME range preserved when within bounds"
   - TEST_CASE: "IME cursor clamped to within composition range"
   - TEST_CASE: "Empty document IME handling"
   - TEST_CASE: "Scroll negative offset clamped to 0"
   - TEST_CASE: "Scroll past end clamped to max"
   - TEST_CASE: "Scroll snap to pixel boundary"
   - TEST_CASE: "Scroll delta prevents overshoot"
   - TEST_CASE: "max_scroll returns 0 when content smaller than viewport"
   - TEST_CASE: "Fractional scroll on 2x DPI"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/ui/IMEGuard.h` |
| Create | `src/ui/IMEGuard.cpp` |
| Create | `src/ui/ScrollGuard.h` |
| Create | `src/ui/ScrollGuard.cpp` |
| Modify | `src/core/IMECompositionOverlay.h` |
| Create | `tests/unit/test_ime_scroll_safety.cpp` |

---

## Implementation Notes

- **IME composition ranges**: On macOS, `NSTextInputClient` can report composition ranges. On Windows, `ImmGetCompositionString` provides ranges. On Linux, IBus/Fcitx provide ranges via GTK IM context. All can produce out-of-bounds ranges under rapid input.
- **Scroll clamping**: Use `std::clamp()` for offset clamping. For `max_scroll`, the formula is `max(0, content_size - viewport_size)`.
- **Pixel snapping**: For DPI scale `s`, snap to nearest `1/s` pixel: `round(offset * s) / s`. This ensures crisp text rendering on high-DPI displays.
- **Overflow prevention**: Use `safe_subtract()` from Phase 11 when computing scroll ranges to prevent unsigned underflow.
- **Log clamping**: Log at WARN level when IME range clamping occurs — this indicates an IME driver bug or an application bug that should be investigated.
- **Testing DPI**: Test pixel snapping at various DPI scales: 1.0 (96 DPI), 1.5 (144 DPI), 2.0 (192 DPI), 2.5 (240 DPI).
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] IME composition range outside document bounds clamped correctly
- [ ] IME composition on empty document handled without crash
- [ ] Scroll offset negative value clamped to 0
- [ ] Scroll past end clamped to max
- [ ] Fractional scroll offsets snapped to pixel boundaries
- [ ] Scroll delta prevents overshoot beyond bounds
- [ ] `max_scroll()` returns 0 when content fits in viewport
- [ ] DPI-aware pixel snapping works at 1x, 1.5x, 2x scales
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test IME with ranges at document boundaries (start=0, end=document_length)
- Test IME with ranges beyond document (start > document_length)
- Test scroll clamping at boundaries (0, max)
- Test scroll delta with small and large values
- Test pixel snapping at various DPI scales
- Integration test: verify smooth scrolling with no jitter from rounding errors
