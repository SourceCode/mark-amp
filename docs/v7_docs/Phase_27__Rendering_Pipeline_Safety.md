# Phase 27: Rendering Pipeline Safety

## Metadata

| Field | Value |
|---|---|
| Phase ID | 27 |
| Prerequisites | Phase 01 (error types) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PI-35 (damage rect validation), PI-36 (null-safe rendering), PI-37 (safe glyph cache) |

---

## Objective

Add null-safety, damage rect validation, and font/theme fallbacks to the rendering pipeline. Ensure that missing fonts, undefined theme tokens, and invalid damage rectangles never cause crashes — they degrade gracefully with visible fallback values.

---

## Background

The PRD mandates full damage rect validation (PI-35), a null-safe rendering pipeline (PI-36), and safe glyph cache access (PI-37). Rendering code is particularly crash-prone because it deals with platform-specific graphics APIs, font metrics that may not exist, and theme tokens that may be undefined. A missing font or an out-of-bounds damage rect can cause a null pointer dereference or painting outside the viewport.

---

## Scope

### Tasks

1. **Create `src/rendering/RenderGuards.h` / `RenderGuards.cpp`**:
   ```cpp
   namespace markamp::rendering {

   // Damage rect validation
   struct SafeRect {
       int x, y, width, height;
   };

   [[nodiscard]] auto clamp_damage_rect(const SafeRect& rect,
                                         const SafeRect& viewport) -> SafeRect;

   [[nodiscard]] auto is_valid_rect(const SafeRect& rect) -> bool;

   // Null-safe theme token lookup
   [[nodiscard]] auto safe_theme_color(const ThemeEngine& theme,
                                        std::string_view token_name) -> wxColour;

   [[nodiscard]] auto safe_theme_font(const ThemeEngine& theme,
                                       std::string_view token_name) -> wxFont;

   [[nodiscard]] auto safe_theme_brush(const ThemeEngine& theme,
                                        std::string_view token_name) -> wxBrush;

   [[nodiscard]] auto safe_theme_pen(const ThemeEngine& theme,
                                      std::string_view token_name) -> wxPen;

   // Glyph cache safety
   [[nodiscard]] auto safe_glyph_width(const wxFont& font, wchar_t ch) -> int;

   // Fallback colors
   namespace fallback {
   #ifndef NDEBUG
       constexpr auto kMissingColor = wxColour(255, 0, 255);  // Magenta (visible in debug)
   #else
       constexpr auto kMissingColor = wxColour(51, 51, 51);   // Dark gray (subtle in release)
   #endif
       auto default_font() -> wxFont;
   }

   } // namespace markamp::rendering
   ```
   - **Damage rect clamping**: Clamp negative coordinates to 0, clamp width/height to viewport bounds
   - **Theme fallbacks**: If a theme token is not defined, return a fallback value instead of crashing
   - **Font fallbacks**: If a requested font is not available, return the system default font
   - **Glyph cache**: If font metrics are unavailable for a character, return a zero-width (not crash)

2. **Modify `src/rendering/HtmlRenderer.cpp`**:
   - Apply `clamp_damage_rect()` before any painting operation
   - Use `safe_theme_color()` / `safe_theme_font()` instead of direct theme lookups
   - Wrap rendering operations in try/catch that falls back to simple text rendering

3. **Modify `src/core/ThemeEngine.cpp`**:
   - Add fallback for missing tokens:
     ```cpp
     auto ThemeEngine::get_color(std::string_view token) const -> wxColour {
         auto it = colors_.find(token);
         if (it == colors_.end()) {
             MARKAMP_LOG_DEBUG_S(SubsystemId::Rendering,
                 "Missing theme token: {}, using fallback", token);
             return rendering::fallback::kMissingColor;
         }
         return it->second;
     }
     ```
   - Same pattern for fonts, brushes, and pens

4. **Create `tests/unit/test_render_guards.cpp`**:
   - TEST_CASE: "clamp_damage_rect handles negative coordinates"
   - TEST_CASE: "clamp_damage_rect handles oversized rect"
   - TEST_CASE: "clamp_damage_rect preserves valid rect"
   - TEST_CASE: "is_valid_rect rejects zero-size rect"
   - TEST_CASE: "is_valid_rect rejects negative dimensions"
   - TEST_CASE: "safe_theme_color returns fallback for missing token"
   - TEST_CASE: "safe_theme_color returns actual color for existing token"
   - TEST_CASE: "safe_theme_font returns default for missing font"
   - TEST_CASE: "safe_glyph_width returns 0 for unknown character"
   - TEST_CASE: "Fallback color is magenta in debug, dark gray in release"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/rendering/RenderGuards.h` |
| Create | `src/rendering/RenderGuards.cpp` |
| Modify | `src/rendering/HtmlRenderer.cpp` |
| Modify | `src/core/ThemeEngine.cpp` |
| Create | `tests/unit/test_render_guards.cpp` |

---

## Implementation Notes

- **Damage rect origin**: In wxWidgets, damage rects come from `wxPaintEvent::GetUpdateRegion()`. These can have negative coordinates if the window is partially off-screen.
- **Clamping math**: Use `std::max(0, rect.x)` for negative coords. Use `std::min(rect.x + rect.width, viewport.width)` for oversized rects.
- **Fallback colors**: Magenta (#FF00FF) in debug builds makes missing tokens immediately visible. Dark gray (#333333) in release provides a usable (if ugly) fallback.
- **Default font**: Use `wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)` as the fallback.
- **Glyph width**: Use `wxDC::GetTextExtent()` with error handling. If the font doesn't have the glyph, return 0 (the character will be skipped rather than crashing).
- **Performance**: Fallback lookups are only hit on error paths. The normal path (theme token exists) is the same speed as before.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Negative damage rect coordinates clamped to 0
- [ ] Oversized damage rects clamped to viewport bounds
- [ ] Missing font returns system default (not crash)
- [ ] Missing theme token returns fallback color (magenta debug / gray release)
- [ ] No null pointer dereference possible in rendering pipeline
- [ ] Missing glyph returns zero-width (not crash)
- [ ] Rendering continues with degraded visuals when theme is incomplete
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test damage rect clamping with edge cases: all negative, all zero, partially off-screen
- Test theme fallback with empty theme (no tokens defined)
- Test font fallback with non-existent font name
- Test glyph width with characters outside basic Latin range
- Verify debug vs release fallback color difference
- Visual test: load the app with a theme that has missing tokens, verify it doesn't crash
