# Phase 29: Fallback Theme Safety

## Metadata

| Field | Value |
|---|---|
| Phase ID | 29 |
| Prerequisites | Phase 27 (rendering pipeline safety) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PI-40 (fallback theme safety), PII-2 (soft-fail rendering mode) |

---

## Objective

Ensure the application always has a valid theme, even if all theme files are corrupted or deleted. Implement a hardcoded minimal theme compiled into the binary that provides all required visual tokens. Detect theme corruption and automatically activate the fallback.

---

## Background

The PRD mandates fallback theme safety (PI-40) and soft-fail rendering mode (PII-2). Themes are loaded from YAML files on disk. If a user deletes their themes directory, if a theme file is corrupted, or if a theme update introduces a parse error, the application must continue to function with a usable (if plain) visual appearance. Currently a missing or corrupted theme could cause null pointer dereferences in the rendering pipeline.

---

## Scope

### Tasks

1. **Create `src/core/FallbackTheme.h` / `FallbackTheme.cpp`**:
   ```cpp
   namespace markamp::core {

   class FallbackTheme {
   public:
       // Get the hardcoded fallback theme as a YAML string
       static auto yaml() -> std::string_view;

       // Get the fallback theme as a populated ThemeData structure
       static auto data() -> ThemeData;

       // Apply fallback theme to ThemeEngine
       static void apply_to(ThemeEngine& engine);

       // Check if the current theme is the fallback
       static auto is_fallback_active(const ThemeEngine& engine) -> bool;
   };

   } // namespace markamp::core
   ```
   - Hardcoded theme with all required tokens (compiled into binary, no file I/O):
     ```
     # Fallback Theme (built-in)
     editor_background: "#1e1e1e"
     editor_foreground: "#d4d4d4"
     editor_selection: "#264f78"
     editor_cursor: "#ffffff"
     editor_line_highlight: "#2a2d2e"
     sidebar_background: "#252526"
     sidebar_foreground: "#cccccc"
     statusbar_background: "#007acc"
     statusbar_foreground: "#ffffff"
     comment_color: "#6a9955"
     keyword_color: "#569cd6"
     string_color: "#ce9178"
     number_color: "#b5cea8"
     type_color: "#4ec9b0"
     function_color: "#dcdcaa"
     error_color: "#f44747"
     warning_color: "#cca700"
     info_color: "#75beff"
     border_color: "#3c3c3c"
     ```
   - These colors follow VS Code's Dark+ theme for familiar appearance

2. **Modify `src/core/ThemeEngine.cpp`**:
   - In `load_theme()`, catch parse errors and fall back:
     ```cpp
     auto ThemeEngine::load_theme(const fs::path& theme_path) -> Result<void> {
         auto parse_result = BoundedParser{}.parse_yaml_file(theme_path);
         if (!parse_result) {
             MARKAMP_LOG_WARN_S(SubsystemId::Rendering,
                 "Theme file corrupted: {}, falling back to built-in theme",
                 theme_path.string());
             FallbackTheme::apply_to(*this);
             emit_event(ThemeChangedEvent{.theme_name = "fallback"});
             return {};  // Success (degraded)
         }
         // ... normal theme loading ...
     }
     ```
   - If no theme files exist at all, use fallback at startup

3. **Modify `src/core/ThemeRegistry.cpp`** (or equivalent):
   - Add corruption detection: validate theme YAML before loading
   - If all themes in registry are corrupted, activate fallback
   - Emit `ThemeCorruptionDetectedEvent` for user notification

4. **Create `tests/unit/test_fallback_theme.cpp`**:
   - TEST_CASE: "Fallback theme YAML is valid parseable YAML"
   - TEST_CASE: "Fallback theme has all required color tokens"
   - TEST_CASE: "FallbackTheme::apply_to sets all theme values"
   - TEST_CASE: "Corrupted theme file triggers fallback"
   - TEST_CASE: "Deleted theme directory triggers fallback"
   - TEST_CASE: "is_fallback_active returns true when fallback is active"
   - TEST_CASE: "Fallback theme produces usable rendering"
   - TEST_CASE: "User notified when fallback theme is active"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/FallbackTheme.h` |
| Create | `src/core/FallbackTheme.cpp` |
| Modify | `src/core/ThemeEngine.cpp` |
| Modify | `src/core/ThemeRegistry.cpp` |
| Create | `tests/unit/test_fallback_theme.cpp` |

---

## Implementation Notes

- **Hardcoded theme**: The fallback theme is a `constexpr std::string_view` in the source code. No file I/O is needed. This is critical — if the filesystem is corrupted, we must still have a usable theme.
- **Required tokens**: Audit the codebase for all `ThemeEngine::get_color()` calls to determine the complete set of required tokens. The fallback must have ALL of them.
- **User notification**: When the fallback is activated, show a non-blocking notification: "Using built-in fallback theme. Your theme file may be corrupted." Use `ErrorReportingService::report_and_notify()` from Phase 30 if available.
- **Soft-fail rendering mode (PII-2)**: If rendering fails even with the fallback theme, disable advanced visual effects (minimap, live preview) and continue with basic text rendering.
- **Theme corruption**: A theme is "corrupted" if it fails YAML parsing or is missing required tokens. Missing optional tokens (decoration colors, etc.) use the render guards from Phase 27.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Deleting all theme files still produces a usable UI
- [ ] Corrupted theme YAML triggers fallback + user notification
- [ ] Fallback theme has all required tokens (minimum 20 color tokens)
- [ ] `is_fallback_active()` correctly reports fallback state
- [ ] User notified that fallback theme is active
- [ ] Fallback theme is visually usable (dark theme, readable text)
- [ ] No null pointer dereference with fallback theme
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test with no theme files on disk (empty themes directory)
- Test with corrupted theme file (invalid YAML, missing tokens)
- Test with partially valid theme (some tokens present, others missing)
- Verify all required tokens are present in fallback
- Visual test: load app with fallback theme, verify readability
- Test transition from corrupted theme to fallback mid-session
