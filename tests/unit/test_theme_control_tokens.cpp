// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "core/BuiltInThemes.h"
#include "core/Theme.h"
#include "core/ThemeEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ── Task 1: Control state tokens exist ──────────────────────────────

TEST_CASE("Theme struct has V10 control state color fields", "[theme][control][tokens]")
{
    const Theme theme = get_default_theme();

    // Verify all 11 control state colors have non-zero defaults
    REQUIRE(theme.control_bg_normal.r > 0);
    REQUIRE(theme.control_bg_hover.r > 0);
    REQUIRE(theme.control_bg_pressed.r > 0);
    REQUIRE(theme.control_bg_focus.r > 0);
    REQUIRE(theme.control_bg_disabled.r > 0);
    REQUIRE(theme.control_bg_selected.r > 0);
    REQUIRE(theme.control_fg_normal.r > 0);
    REQUIRE(theme.control_fg_disabled.r > 0);
    REQUIRE(theme.control_border_normal.r > 0);
    REQUIRE(theme.control_border_focus.r > 0);
    REQUIRE(theme.focus_ring_color.r > 0);
}

TEST_CASE("ThemeColorToken enum has all V10 control tokens", "[theme][control][tokens]")
{
    // Verify token values are sequential and within bounds
    auto check_token = [](ThemeColorToken token)
    {
        auto idx = static_cast<std::size_t>(token);
        REQUIRE(idx < kColorTokenCount);
    };

    check_token(ThemeColorToken::ControlBgNormal);
    check_token(ThemeColorToken::ControlBgHover);
    check_token(ThemeColorToken::ControlBgPressed);
    check_token(ThemeColorToken::ControlBgFocus);
    check_token(ThemeColorToken::ControlBgDisabled);
    check_token(ThemeColorToken::ControlBgSelected);
    check_token(ThemeColorToken::ControlFgNormal);
    check_token(ThemeColorToken::ControlFgDisabled);
    check_token(ThemeColorToken::ControlBorderNormal);
    check_token(ThemeColorToken::ControlBorderFocus);
    check_token(ThemeColorToken::FocusRingColor);
}

// ── Task 2: Focus ring token set ────────────────────────────────────

TEST_CASE("Focus ring color is visible and non-zero", "[theme][control][focusring]")
{
    const Theme theme = get_default_theme();
    // Focus ring should be a visible, non-black color
    int ring_sum = theme.focus_ring_color.r + theme.focus_ring_color.g + theme.focus_ring_color.b;
    REQUIRE(ring_sum > 0);
    // Focus ring should have reasonable contrast against bg_app
    float contrast = theme.focus_ring_color.contrast_ratio(theme.colors.bg_app);
    REQUIRE(contrast >= 3.0F); // WCAG AA for non-text: 3:1 minimum
}

// ── Task 3: Hover vs Pressed vs Normal differentiation ──────────────

TEST_CASE("Control bg hover is lighter than normal on dark theme", "[theme][control][elevation]")
{
    const Theme theme = get_default_theme();
    // On dark themes, hover bg should be visually lighter (higher RGB sum)
    if (theme.is_dark())
    {
        int normal_sum =
            theme.control_bg_normal.r + theme.control_bg_normal.g + theme.control_bg_normal.b;
        int hover_sum =
            theme.control_bg_hover.r + theme.control_bg_hover.g + theme.control_bg_hover.b;
        REQUIRE(hover_sum > normal_sum);
    }
}

TEST_CASE("Control bg pressed is darker than normal on dark theme", "[theme][control][elevation]")
{
    const Theme theme = get_default_theme();
    if (theme.is_dark())
    {
        int normal_sum =
            theme.control_bg_normal.r + theme.control_bg_normal.g + theme.control_bg_normal.b;
        int pressed_sum =
            theme.control_bg_pressed.r + theme.control_bg_pressed.g + theme.control_bg_pressed.b;
        REQUIRE(pressed_sum < normal_sum);
    }
}

// ── Task 4: Token fallback validation ───────────────────────────────

TEST_CASE("ThemeEngine caches all V10 control tokens", "[theme][control][engine]")
{
    EventBus event_bus;
    ThemeRegistry registry;
    ThemeEngine engine(event_bus, registry);

    // All control tokens should be in the cache (not missing)
    auto missing = engine.missing_tokens();
    for (auto token : missing)
    {
        REQUIRE(token != ThemeColorToken::ControlBgNormal);
        REQUIRE(token != ThemeColorToken::ControlBgHover);
        REQUIRE(token != ThemeColorToken::ControlBgPressed);
        REQUIRE(token != ThemeColorToken::ControlBgFocus);
        REQUIRE(token != ThemeColorToken::ControlBgDisabled);
        REQUIRE(token != ThemeColorToken::ControlBgSelected);
        REQUIRE(token != ThemeColorToken::ControlFgNormal);
        REQUIRE(token != ThemeColorToken::ControlFgDisabled);
        REQUIRE(token != ThemeColorToken::ControlBorderNormal);
        REQUIRE(token != ThemeColorToken::ControlBorderFocus);
        REQUIRE(token != ThemeColorToken::FocusRingColor);
    }
}

TEST_CASE("ThemeEngine control token colors are accessible via color()", "[theme][control][engine]")
{
    EventBus event_bus;
    ThemeRegistry registry;
    ThemeEngine engine(event_bus, registry);

    // Verify control tokens return non-black colors (which would indicate missing fallback)
    auto hover_color = engine.color(ThemeColorToken::ControlBgHover);
    REQUIRE(hover_color.IsOk());
    REQUIRE(hover_color != wxColour(0, 0, 0));

    auto focus_ring = engine.color(ThemeColorToken::FocusRingColor);
    REQUIRE(focus_ring.IsOk());
    REQUIRE(focus_ring != wxColour(0, 0, 0));
}

TEST_CASE("ThemeEngine control tokens work with color_fast()", "[theme][control][engine]")
{
    EventBus event_bus;
    ThemeRegistry registry;
    ThemeEngine engine(event_bus, registry);

    auto fast_color = engine.color_fast(ThemeColorToken::ControlBgSelected);
    REQUIRE(fast_color.IsOk());
}

// ── Control foreground disabled is dimmer than normal ────────────────

TEST_CASE("Control fg disabled is dimmer than normal", "[theme][control][disabled]")
{
    const Theme theme = get_default_theme();
    int normal_sum =
        theme.control_fg_normal.r + theme.control_fg_normal.g + theme.control_fg_normal.b;
    int disabled_sum =
        theme.control_fg_disabled.r + theme.control_fg_disabled.g + theme.control_fg_disabled.b;
    REQUIRE(disabled_sum < normal_sum);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
