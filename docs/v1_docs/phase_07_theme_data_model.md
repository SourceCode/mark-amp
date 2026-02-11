# Phase 07: Theme Data Model and JSON Serialization

## Objective

Implement the complete theme data model in C++, including the 10-color token system from the reference implementation, JSON serialization/deserialization with nlohmann/json, theme validation, and the 8 built-in themes. This phase creates the data layer -- the rendering engine comes in Phase 08.

## Prerequisites

- Phase 05 (Application Architecture and Module Design)

## Deliverables

1. Theme struct with all color tokens
2. JSON serialization/deserialization for themes
3. Theme validation (schema checking)
4. All 8 built-in themes as embedded data
5. Theme file I/O (load from and save to JSON files)
6. Color utility functions (hex parsing, alpha blending, contrast calculation)

## Tasks

### Task 7.1: Implement the Color type

Create `/Users/ryanrentfro/code/markamp/src/core/Color.h` and `Color.cpp`:

A color type that can represent RGBA colors and convert to/from various formats.

```cpp
namespace markamp::core {

struct Color
{
    uint8_t r{0}, g{0}, b{0}, a{255};

    // Constructors
    constexpr Color() = default;
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    // Factory functions
    static auto from_hex(std::string_view hex) -> std::expected<Color, std::string>;
    static auto from_rgba_string(std::string_view rgba) -> std::expected<Color, std::string>;
    static constexpr auto from_rgb(uint8_t r, uint8_t g, uint8_t b) -> Color;

    // Conversion
    [[nodiscard]] auto to_hex() const -> std::string;       // "#RRGGBB" or "#RRGGBBAA"
    [[nodiscard]] auto to_wx_colour() const -> wxColour;
    [[nodiscard]] auto to_rgba_string() const -> std::string; // "rgba(r, g, b, a)"

    // Operations
    [[nodiscard]] auto with_alpha(uint8_t alpha) const -> Color;
    [[nodiscard]] auto with_alpha(float alpha_01) const -> Color;
    [[nodiscard]] auto blend(const Color& other, float factor) const -> Color;
    [[nodiscard]] auto luminance() const -> float;
    [[nodiscard]] auto contrast_ratio(const Color& other) const -> float;

    // Comparison
    auto operator<=>(const Color&) const = default;
};

} // namespace markamp::core
```

The `from_hex` function must handle:
- `#RGB` (shorthand)
- `#RRGGBB` (standard)
- `#RRGGBBAA` (with alpha)
- `RGB` / `RRGGBB` / `RRGGBBAA` (without hash)

The `from_rgba_string` function must handle the CSS `rgba()` format found in some theme colors:
- `rgba(255, 0, 85, 0.2)` -- as seen in Midnight Neon's border-light

**Acceptance criteria:**
- All hex formats parse correctly
- RGBA strings parse correctly
- Round-trip: `Color::from_hex(color.to_hex()) == color`
- Luminance and contrast calculations are correct per WCAG formulas
- wxColour conversion produces correct results
- At least 15 unit tests covering all formats, edge cases, and operations

### Task 7.2: Implement the Theme struct

Create `/Users/ryanrentfro/code/markamp/src/core/Theme.h` and `Theme.cpp`:

```cpp
namespace markamp::core {

// The 10 color tokens from the reference implementation
struct ThemeColors
{
    Color bg_app;           // --bg-app
    Color bg_panel;         // --bg-panel
    Color bg_header;        // --bg-header
    Color bg_input;         // --bg-input
    Color text_main;        // --text-main
    Color text_muted;       // --text-muted
    Color accent_primary;   // --accent-primary
    Color accent_secondary; // --accent-secondary
    Color border_light;     // --border-light
    Color border_dark;      // --border-dark
};

struct Theme
{
    std::string id;
    std::string name;
    ThemeColors colors;

    // Validation
    [[nodiscard]] auto is_valid() const -> bool;
    [[nodiscard]] auto validation_errors() const -> std::vector<std::string>;

    // Derived colors (computed from the base 10)
    [[nodiscard]] auto scrollbar_track() const -> Color;    // bg_panel
    [[nodiscard]] auto scrollbar_thumb() const -> Color;    // accent_secondary
    [[nodiscard]] auto scrollbar_hover() const -> Color;    // accent_primary
    [[nodiscard]] auto selection_bg() const -> Color;       // accent_primary with 20% alpha
    [[nodiscard]] auto hover_bg() const -> Color;           // accent_primary with 10% alpha
    [[nodiscard]] auto error_color() const -> Color;        // computed red that contrasts with bg
    [[nodiscard]] auto success_color() const -> Color;      // computed green that contrasts with bg
    [[nodiscard]] auto is_dark() const -> bool;             // based on bg_app luminance

    auto operator<=>(const Theme&) const = default;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Theme struct holds all 10 color tokens
- Derived colors are computed correctly from base tokens
- `is_dark()` correctly identifies dark vs light themes
- Validation catches themes with missing or invalid colors

### Task 7.3: Implement JSON serialization with nlohmann/json

Add JSON conversion functions to Theme:

```cpp
// ADL serialization for nlohmann/json
void to_json(nlohmann::json& j, const Color& c);
void from_json(const nlohmann::json& j, Color& c);
void to_json(nlohmann::json& j, const ThemeColors& tc);
void from_json(const nlohmann::json& j, ThemeColors& tc);
void to_json(nlohmann::json& j, const Theme& t);
void from_json(const nlohmann::json& j, Theme& t);
```

**JSON format must match the reference TypeScript format exactly:**
```json
{
    "id": "midnight-neon",
    "name": "Midnight Neon",
    "colors": {
        "--bg-app": "#050510",
        "--bg-panel": "#0a0a1f",
        "--bg-header": "#0f0f2d",
        "--bg-input": "#050510",
        "--text-main": "#00ffea",
        "--text-muted": "#5e6ad2",
        "--accent-primary": "#ff0055",
        "--accent-secondary": "#00ffea",
        "--border-light": "rgba(255, 0, 85, 0.2)",
        "--border-dark": "#000000"
    }
}
```

The JSON keys must use the CSS variable names (with `--` prefix) to maintain compatibility with themes exported from the React version.

**Acceptance criteria:**
- Serialization produces JSON matching the reference format
- Deserialization reads the reference format correctly
- Round-trip: `from_json(to_json(theme)) == theme`
- Both hex and rgba() color formats are handled in deserialization
- Invalid JSON produces descriptive error messages

### Task 7.4: Embed the 8 built-in themes

Create `/Users/ryanrentfro/code/markamp/src/core/BuiltinThemes.h` and `BuiltinThemes.cpp`:

Embed all 8 themes from the reference `constants.ts` as constexpr or static data:

1. **Midnight Neon**: `#050510` bg, `#ff0055` accent, `#00ffea` text
2. **Cyber Night**: `#1a1a2e` bg, `#e94560` accent, `#533483` secondary
3. **Solarized Dark**: `#002b36` bg, `#b58900` accent, `#2aa198` secondary
4. **Classic Mono**: `#ffffff` bg, `#000000` accent (light theme)
5. **High Contrast Blue**: `#000080` bg, `#ffff00` accent, high contrast
6. **Matrix Core**: `#0d1117` bg, `#00ff41` accent, green terminal
7. **Classic Amp**: `#282828` bg, `#d4af37` accent, Winamp-inspired
8. **Vapor Wave**: `#2b1055` bg, `#ff00ff` accent, vaporwave aesthetic

Provide a function:
```cpp
auto get_builtin_themes() -> const std::vector<Theme>&;
auto get_default_theme() -> const Theme&; // Returns Midnight Neon
```

**Acceptance criteria:**
- All 8 themes match the exact color values from `constants.ts`
- Colors that use `rgba()` format are correctly parsed (e.g., Midnight Neon's border-light)
- Each theme has a unique ID matching the reference
- Default theme is Midnight Neon

### Task 7.5: Create theme JSON resource files

Write all 8 built-in themes as individual JSON files under `/Users/ryanrentfro/code/markamp/resources/themes/`:

- `midnight_neon.json`
- `cyber_night.json`
- `solarized_dark.json`
- `classic_mono.json`
- `high_contrast_blue.json`
- `matrix_core.json`
- `classic_amp.json`
- `vapor_wave.json`

Each file must be valid JSON matching the format from Task 7.3.

**Acceptance criteria:**
- All 8 JSON files exist and are valid
- Loading each file with `from_json` produces a theme equal to the embedded version
- Files can be imported by the React version (format compatibility)

### Task 7.6: Implement ThemeRegistry

Create `/Users/ryanrentfro/code/markamp/src/core/ThemeRegistry.h` and `ThemeRegistry.cpp`:

Manages the collection of available themes (built-in + user-imported).

```cpp
namespace markamp::core {

struct ThemeInfo
{
    std::string id;
    std::string name;
    bool is_builtin;
    std::optional<std::filesystem::path> file_path;
};

class ThemeRegistry
{
public:
    ThemeRegistry();

    // Load all themes (built-in + user directory)
    auto initialize() -> std::expected<void, std::string>;

    // Query
    [[nodiscard]] auto get_theme(const std::string& id) const -> std::optional<Theme>;
    [[nodiscard]] auto list_themes() const -> std::vector<ThemeInfo>;
    [[nodiscard]] auto theme_count() const -> size_t;

    // Import/Export
    auto import_theme(const std::filesystem::path& path) -> std::expected<Theme, std::string>;
    auto export_theme(const std::string& id, const std::filesystem::path& path) -> std::expected<void, std::string>;

    // User theme directory
    [[nodiscard]] static auto user_themes_directory() -> std::filesystem::path;

private:
    std::vector<Theme> themes_;
    void load_builtin_themes();
    auto load_user_themes() -> std::expected<void, std::string>;
};

} // namespace markamp::core
```

User theme directory:
- macOS: `~/Library/Application Support/MarkAmp/themes/`
- Windows: `%APPDATA%/MarkAmp/themes/`
- Linux: `~/.config/markamp/themes/`

**Acceptance criteria:**
- Registry initializes with all 8 built-in themes
- User themes from the user directory are discovered and loaded
- Duplicate theme IDs from user directory do not overwrite built-in themes
- Import validates the theme before adding
- Export writes valid JSON

### Task 7.7: Write comprehensive theme tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_theme.cpp`:

Test cases:
1. Color parsing from hex (all formats)
2. Color parsing from rgba strings
3. Color round-trip serialization
4. Color blending
5. Color luminance and contrast calculation
6. Theme JSON serialization round-trip
7. Theme validation (valid theme passes, invalid fails)
8. Built-in theme count and correctness
9. Theme registry initialization
10. Theme import with valid file
11. Theme import with invalid file (missing fields)
12. Theme import with malformed JSON
13. Theme export and re-import produces identical theme
14. Derived color calculations

**Acceptance criteria:**
- All tests pass
- At least 25 test assertions
- Edge cases for color parsing are covered (empty string, invalid hex, overflow)

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/Color.h` | Created |
| `src/core/Color.cpp` | Created |
| `src/core/Theme.h` | Created (replacing placeholder) |
| `src/core/Theme.cpp` | Created |
| `src/core/BuiltinThemes.h` | Created |
| `src/core/BuiltinThemes.cpp` | Created |
| `src/core/ThemeRegistry.h` | Created |
| `src/core/ThemeRegistry.cpp` | Created |
| `resources/themes/midnight_neon.json` | Created |
| `resources/themes/cyber_night.json` | Created |
| `resources/themes/solarized_dark.json` | Created |
| `resources/themes/classic_mono.json` | Created |
| `resources/themes/high_contrast_blue.json` | Created |
| `resources/themes/matrix_core.json` | Created |
| `resources/themes/classic_amp.json` | Created |
| `resources/themes/vapor_wave.json` | Created |
| `tests/unit/test_theme.cpp` | Created |
| `src/CMakeLists.txt` | Modified |
| `tests/CMakeLists.txt` | Modified |

## Dependencies

- nlohmann/json 3.x
- wxWidgets (for wxColour conversion)
- std::expected (C++23/26)
- std::filesystem

## Estimated Complexity

**Medium** -- The data model is well-defined by the reference implementation. The main complexity is in correct color parsing (especially rgba strings with floating-point alpha) and ensuring perfect JSON format compatibility.
