# Phase 08: Theme Engine and Runtime Switching

## Objective

Implement the theme engine that applies theme colors to all wxWidgets components in real-time, enabling instant theme hot-swapping without application restart. This is the rendering/application side of the theme system -- Phase 07 provided the data model.

## Prerequisites

- Phase 06 (Custom Window Chrome)
- Phase 07 (Theme Data Model and JSON Serialization)

## Deliverables

1. ThemeEngine class implementing IThemeEngine interface
2. Theme-aware base classes for all UI components
3. Real-time theme switching with smooth transitions
4. Font management system (JetBrains Mono + Rajdhani)
5. Custom scrollbar rendering with theme colors
6. Theme change notification and propagation system

## Tasks

### Task 8.1: Implement the ThemeEngine

Create `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.h` and `ThemeEngine.cpp`:

```cpp
namespace markamp::core {

class ThemeEngine : public IThemeEngine
{
public:
    explicit ThemeEngine(EventBus& event_bus, ThemeRegistry& registry);

    // IThemeEngine interface
    auto current_theme() const -> const Theme& override;
    void apply_theme(const std::string& theme_id) override;
    auto available_themes() const -> std::vector<ThemeInfo> override;
    void import_theme(const std::filesystem::path& path) override;
    void export_theme(const std::string& theme_id, const std::filesystem::path& path) override;

    // Apply current theme to a specific window and all its children
    void apply_to_window(wxWindow* window);

    // Get themed brushes, pens, and colors for custom drawing
    [[nodiscard]] auto brush(ThemeColorToken token) const -> const wxBrush&;
    [[nodiscard]] auto pen(ThemeColorToken token, int width = 1) const -> const wxPen&;
    [[nodiscard]] auto color(ThemeColorToken token) const -> const wxColour&;
    [[nodiscard]] auto font(ThemeFontToken token) const -> const wxFont&;

private:
    EventBus& event_bus_;
    ThemeRegistry& registry_;
    Theme current_theme_;

    // Cached wxWidgets rendering objects
    struct CachedResources {
        std::unordered_map<ThemeColorToken, wxBrush> brushes;
        std::unordered_map<ThemeColorToken, wxPen> pens;
        std::unordered_map<ThemeColorToken, wxColour> colours;
        std::unordered_map<ThemeFontToken, wxFont> fonts;
    };
    CachedResources cache_;

    void rebuild_cache();
    void propagate_theme(wxWindow* window);
};

enum class ThemeColorToken {
    BgApp, BgPanel, BgHeader, BgInput,
    TextMain, TextMuted,
    AccentPrimary, AccentSecondary,
    BorderLight, BorderDark,
    // Derived tokens
    SelectionBg, HoverBg, ErrorColor, SuccessColor,
    ScrollbarTrack, ScrollbarThumb, ScrollbarHover
};

enum class ThemeFontToken {
    SansRegular, SansSemiBold, SansBold,
    MonoRegular, MonoBold,
    UISmall, UILabel, UIHeading
};

} // namespace markamp::core
```

**Acceptance criteria:**
- ThemeEngine initializes with the default theme
- `apply_theme()` switches themes and publishes `ThemeChangedEvent`
- Cached rendering resources are rebuilt on theme change
- All 10 base tokens + derived tokens are accessible

### Task 8.2: Implement font management

Add font loading and management to the ThemeEngine:

**Required fonts (matching reference):**
- JetBrains Mono: Regular, Bold (monospace -- editor, code blocks, status bar)
- Rajdhani: Regular, SemiBold, Bold (sans-serif -- UI chrome, headings, labels)

**Font loading strategy:**
1. Embed fonts as resources in the application binary
2. Register fonts at startup using `wxFont::AddPrivateFont()` (wxWidgets 3.1.6+)
3. Fall back to system fonts if loading fails:
   - Mono fallback: Consolas (Win), Menlo (Mac), DejaVu Sans Mono (Linux)
   - Sans fallback: Segoe UI (Win), SF Pro (Mac), Noto Sans (Linux)

**Font size tokens:**
- `UISmall`: 10px (status bar, labels)
- `UILabel`: 12px (toolbar buttons, sidebar items)
- `UIHeading`: 14px (section headers)
- `MonoRegular`: 13px (editor, code blocks)
- `MonoBold`: 13px (bold code)
- `SansRegular` / `SansSemiBold` / `SansBold`: based on context

Create font resource embedding in CMake:
- Add font files to `resources/fonts/`
- Use CMake's resource embedding or `wxMemoryInputStream` for loading

**Acceptance criteria:**
- JetBrains Mono and Rajdhani are loaded and usable
- Fallback fonts are used when custom fonts are unavailable
- Font sizes are DPI-aware (scale with display scaling)
- Font tokens are accessible via `ThemeEngine::font()`

### Task 8.3: Create ThemeAwareWindow base class

Create `/Users/ryanrentfro/code/markamp/src/ui/ThemeAwareWindow.h` and `ThemeAwareWindow.cpp`:

A base class (or mixin) that all theme-responsive UI components inherit from.

```cpp
namespace markamp::ui {

class ThemeAwareWindow : public wxPanel
{
public:
    ThemeAwareWindow(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~ThemeAwareWindow() override;

protected:
    // Called when the theme changes -- subclasses override to update their appearance
    virtual void OnThemeChanged(const core::Theme& new_theme);

    // Convenience accessors
    [[nodiscard]] auto theme() const -> const core::Theme&;
    [[nodiscard]] auto theme_engine() const -> core::ThemeEngine&;

    // Common themed drawing helpers
    void DrawBevelBorder(wxDC& dc, const wxRect& rect, core::BevelPanel::BevelStyle style);
    void FillBackground(wxDC& dc, core::ThemeColorToken token);

private:
    core::ThemeEngine& theme_engine_;
    core::Subscription theme_subscription_;
};

} // namespace markamp::ui
```

**Acceptance criteria:**
- Theme changes propagate to all ThemeAwareWindow instances
- Subscription is automatically cleaned up on destruction
- No dangling references after window destruction

### Task 8.4: Implement recursive theme propagation

When `ThemeEngine::apply_theme()` is called, it must update every visible window:

1. Publish `ThemeChangedEvent` on the event bus
2. Walk the window tree starting from the top-level frame
3. For each window:
   - Set background color to the appropriate theme color
   - Set foreground color to the appropriate text color
   - If the window is a `ThemeAwareWindow`, call its `OnThemeChanged()`
   - Recursively process all children
4. Force repaint of the entire application

**Implementation details:**
- Use `wxWindow::GetChildren()` to traverse the tree
- Use `wxWindow::SetBackgroundColour()` / `SetForegroundColour()` for basic windows
- Call `Refresh()` and `Update()` to force repaint
- Use `wxWindowUpdateLocker` to prevent flickering during the update

**Acceptance criteria:**
- Switching themes updates all visible components instantly
- No visible flickering during theme transition
- Components that were hidden and later shown get the correct theme
- Performance: theme switch completes in under 50ms for typical window hierarchy

### Task 8.5: Implement custom scrollbar rendering

Create `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.h` and `ThemedScrollbar.cpp`:

Replace the OS scrollbar with a custom-drawn scrollbar matching the reference:
- Track: `bg_panel` color
- Thumb: `accent_secondary` color, 4px border radius
- Thumb hover: `accent_primary` color
- Width: 8px

**Implementation options:**
1. Custom wxScrollBar subclass with owner-draw
2. Overlay panel that intercepts scroll events and draws on top
3. Use `wxScrolledWindow` with custom paint

**Recommended approach:** Create a `ThemedScrolledWindow` class:
```cpp
class ThemedScrolledWindow : public ThemeAwareWindow
{
public:
    // Wraps content in a scrollable area with custom scrollbars
    // Handles mouse wheel, touch scroll, keyboard scroll
    // Draws custom scrollbar overlay
};
```

**Acceptance criteria:**
- Scrollbar track, thumb, and hover colors match theme
- Scrollbar is 8px wide
- Smooth scrolling with mouse wheel
- Scrollbar auto-hides when content fits (or always visible, per design spec)
- Theme changes update scrollbar colors

### Task 8.6: Implement theme transition animation (optional enhancement)

The reference uses `transition-colors duration-300` for smooth color transitions. Implement an optional animated theme switch:

- Use a `wxTimer` to interpolate between old and new theme colors over 300ms
- At each step, compute intermediate colors using `Color::blend()`
- Apply the interpolated theme
- After the transition completes, set the final theme

This can be gated behind a config option `theme.animate_transitions`.

**Acceptance criteria:**
- When enabled, theme switch animates smoothly over 300ms
- When disabled, theme switch is instant
- Animation does not cause performance issues
- Animation can be interrupted by another theme switch

### Task 8.7: Update CustomChrome to be theme-aware

Modify `CustomChrome` (from Phase 06) to:

- Inherit from `ThemeAwareWindow` (or subscribe to theme events)
- Redraw with current theme colors when theme changes
- Use `theme_engine().color(ThemeColorToken::BgHeader)` for background
- Use `theme_engine().color(ThemeColorToken::AccentPrimary)` for the logo dot
- Use `theme_engine().color(ThemeColorToken::TextMuted)` for text

**Acceptance criteria:**
- Chrome updates immediately when theme changes
- All chrome elements use theme colors (no hardcoded colors)
- Logo dot glow effect uses accent-primary
- Window control hover colors update with theme

### Task 8.8: Write theme engine integration tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_theme_engine.cpp`:

Test cases:
1. Engine initializes with default theme
2. `apply_theme()` changes current theme
3. `apply_theme()` with invalid ID returns error / does nothing
4. Theme change publishes event
5. Cached resources are rebuilt after theme change
6. Color token lookup returns correct colors for each theme
7. Font tokens return valid fonts
8. Available themes list includes all built-in themes
9. Import theme adds to available list
10. Export theme writes valid JSON file

**Acceptance criteria:**
- All tests pass
- Theme switching is tested with at least 3 different themes
- Error cases are tested (invalid theme ID, missing theme file)

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/ThemeEngine.h` | Created (replacing placeholder) |
| `src/core/ThemeEngine.cpp` | Created |
| `src/ui/ThemeAwareWindow.h` | Created |
| `src/ui/ThemeAwareWindow.cpp` | Created |
| `src/ui/ThemedScrollbar.h` | Created |
| `src/ui/ThemedScrollbar.cpp` | Created |
| `src/ui/CustomChrome.h` | Modified (theme-aware) |
| `src/ui/CustomChrome.cpp` | Modified (theme-aware) |
| `resources/fonts/JetBrainsMono-Regular.ttf` | Added |
| `resources/fonts/JetBrainsMono-Bold.ttf` | Added |
| `resources/fonts/Rajdhani-Regular.ttf` | Added |
| `resources/fonts/Rajdhani-SemiBold.ttf` | Added |
| `resources/fonts/Rajdhani-Bold.ttf` | Added |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_theme_engine.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (wxFont::AddPrivateFont, wxGraphicsContext)
- Font files (JetBrains Mono from JetBrains, Rajdhani from Google Fonts -- both OFL licensed)
- Phase 05 EventBus
- Phase 07 Theme, ThemeRegistry, Color

## Estimated Complexity

**High** -- Runtime theme switching with correct propagation across the entire window hierarchy is complex. Font management with embedded resources requires platform-specific handling. Custom scrollbar rendering is non-trivial.
