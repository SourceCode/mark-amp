# Phase 37 -- Color Palettes and Dark Mode

## Objective

Implement predefined color palettes for consistent board styling, a palette picker panel, and a dark mode theme toggle that inverts the canvas background and adjusts object colors for readability. Integrates with the existing ThemeEngine.

## Prerequisites

- Phase 01 (CanvasColor)
- Phase 02 (CanvasRenderer with background color)
- Existing ThemeEngine (`src/core/ThemeEngine.h`)

## Feature References (PRD)

- PRD #38: Color Palettes
- PRD #83: Dark Mode
- PRD #86: Focus Mode

## Data Structures to Implement

### File: `src/canvas/ColorPalette.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

struct ColorPalette
{
    std::string id;
    std::string name;
    std::vector<CanvasColor> colors;
    CanvasColor background;
    CanvasColor grid_color;
    CanvasColor text_color;
    bool is_dark{false};

    [[nodiscard]] static auto default_palette() -> ColorPalette;
    [[nodiscard]] static auto dark_palette() -> ColorPalette;
    [[nodiscard]] static auto all_palettes() -> std::vector<ColorPalette>;
};

} // namespace markamp::canvas
```

### File: `src/canvas/PalettePanel.h`

```cpp
#pragma once

#include "ColorPalette.h"

#include <functional>

class wxGraphicsContext;

namespace markamp::canvas
{

class PalettePanel
{
public:
    using OnPaletteSelected = std::function<void(const std::string& palette_id)>;
    using OnColorPicked = std::function<void(const CanvasColor& color)>;

    auto set_palettes(const std::vector<ColorPalette>& palettes) -> void;
    auto set_active_palette(const std::string& palette_id) -> void;
    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;

    auto set_on_palette_selected(OnPaletteSelected cb) -> void;
    auto set_on_color_picked(OnColorPicked cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<ColorPalette> palettes_;
    std::string active_palette_id_;
    bool visible_{false};
    OnPaletteSelected on_palette_selected_;
    OnColorPicked on_color_picked_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/DarkModeManager.h`

```cpp
#pragma once

#include "CanvasTypes.h"

namespace markamp::core
{
class EventBus;
class Config;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::canvas
{

class DarkModeManager
{
public:
    DarkModeManager(core::EventBus& event_bus, core::Config& config,
                     core::ThemeEngine& theme_engine);

    auto toggle_dark_mode() -> void;
    [[nodiscard]] auto is_dark_mode() const -> bool;
    auto set_dark_mode(bool enabled) -> void;

    /// Apply dark mode color adjustments to the canvas renderer.
    auto apply_to_canvas(class CanvasRenderer& renderer) -> void;

    /// Adjust a color for dark mode (invert lightness, preserve hue).
    [[nodiscard]] static auto adjust_for_dark(const CanvasColor& color) -> CanvasColor;

private:
    core::EventBus& event_bus_;
    core::Config& config_;
    core::ThemeEngine& theme_engine_;
    bool dark_mode_{false};
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `ColorPalette::all_palettes()` -- Return predefined palettes: Default (light), Dark, Ocean (blue tones), Sunset (warm tones), Forest (green tones), Minimal (grayscale), Pastel, Neon.

2. `DarkModeManager::toggle_dark_mode()` -- Switch dark mode flag. Update canvas background to dark gray (#1e1e1e). Update grid to subtle light. Publish ThemeChangedEvent.

3. `DarkModeManager::adjust_for_dark()` -- Convert color to HSL, invert lightness (L = 1 - L), convert back to RGB. Preserve hue and saturation. Ensure text remains readable.

4. `apply_to_canvas()` -- Set renderer background, grid color. Iterate all objects and adjust colors if auto-adjust is enabled.

5. Focus mode integration: Focus mode (already has FocusModeChangedEvent) hides all chrome. In canvas context, it hides the toolbar, panels, and minimap, leaving only the canvas.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+Shift+D | Toggle dark mode | global |
| Cmd/Ctrl+Shift+F | Toggle focus mode | canvas |

## Test Cases

File: `tests/unit/test_color_palette.cpp`

1. **Default palette** -- Verify light background, expected colors.
2. **Dark palette** -- Verify dark background, adjusted colors.
3. **All palettes** -- Verify 8+ palettes returned.
4. **Color adjustment** -- White -> nearly black in dark mode. Black -> nearly white.
5. **Dark mode toggle** -- Toggle on, verify background changes. Toggle off, verify restored.
6. **Palette selection** -- Select "Ocean", verify blue tones.
7. **Config persistence** -- Enable dark mode, verify config key set.

## Acceptance Criteria

- [ ] 8+ predefined color palettes
- [ ] Palette picker panel with color swatches
- [ ] Dark mode toggle with canvas background inversion
- [ ] Color adjustment for readability in dark mode
- [ ] Focus mode hides canvas chrome
- [ ] Dark mode preference persisted in config
- [ ] All 7 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/ColorPalette.h` | Palette definitions |
| CREATE | `src/canvas/ColorPalette.cpp` | Palette implementations |
| CREATE | `src/canvas/PalettePanel.h` | Palette picker UI |
| CREATE | `src/canvas/PalettePanel.cpp` | Panel implementation |
| CREATE | `src/canvas/DarkModeManager.h` | Dark mode management |
| CREATE | `src/canvas/DarkModeManager.cpp` | Dark mode implementation |
| MODIFY | `src/canvas/CanvasRenderer.cpp` | Apply palette/dark mode to rendering |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_color_palette.cpp` | 7 Catch2 tests |

## Architecture Notes

- DarkModeManager integrates with the existing ThemeEngine for consistency between the editor UI and the canvas.
- Color palettes are separate from the ThemeEngine's editor themes. Palettes control canvas object colors; themes control the application chrome.
- Focus mode reuses the existing FocusModeChangedEvent from Events.h.

## Estimated Complexity

**M** -- Palette system, dark mode with color adjustment, focus mode integration, 7 tests.
