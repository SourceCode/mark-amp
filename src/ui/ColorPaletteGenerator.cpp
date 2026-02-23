#include "ColorPaletteGenerator.h"

#include "ColorUtils.h"

namespace markamp::ui
{

auto ColorPaletteGenerator::hover_variant(const wxColour& base, bool is_dark_mode) -> wxColour
{
    if (is_dark_mode)
    {
        return lighten(base, 0.10F);
    }
    return darken(base, 0.10F);
}

auto ColorPaletteGenerator::pressed_variant(const wxColour& base, bool is_dark_mode) -> wxColour
{
    if (is_dark_mode)
    {
        return lighten(base, 0.20F);
    }
    return darken(base, 0.20F);
}

auto ColorPaletteGenerator::disabled_variant(const wxColour& base) -> wxColour
{
    const wxColour kDesat = desaturate(base, 0.50F);
    return with_alpha(kDesat, 0.50F);
}

auto ColorPaletteGenerator::focus_ring(const wxColour& accent) -> wxColour
{
    return with_alpha(accent, 0.40F);
}

auto ColorPaletteGenerator::generate_extended_palette(const core::Theme& base_theme)
    -> ExtendedPalette
{
    ExtendedPalette ext;

    // Determine if theme is dark mode based on background vs text luminosity
    const HSLColor kBgHsl = rgb_to_hsl(base_theme.colors.bg_app.to_wx_colour());
    const HSLColor kFgHsl = rgb_to_hsl(base_theme.colors.text_main.to_wx_colour());
    const bool kIsDarkMode = (kBgHsl.l < kFgHsl.l);

    const wxColour kBgPanel = base_theme.colors.bg_panel.to_wx_colour();
    const wxColour kTextMain = base_theme.colors.text_main.to_wx_colour();
    const wxColour kAccent1 = base_theme.colors.accent_primary.to_wx_colour();
    const wxColour kBorderLight = base_theme.colors.border_light.to_wx_colour();

    // Control States
    ext[core::ThemeColorToken::ControlBgNormal] = kBgPanel;
    ext[core::ThemeColorToken::ControlBgHover] = hover_variant(kBgPanel, kIsDarkMode);
    ext[core::ThemeColorToken::ControlBgPressed] = pressed_variant(kBgPanel, kIsDarkMode);
    ext[core::ThemeColorToken::ControlBgFocus] = ext[core::ThemeColorToken::ControlBgHover];
    ext[core::ThemeColorToken::ControlBgDisabled] = disabled_variant(kBgPanel);
    ext[core::ThemeColorToken::ControlBgSelected] = with_alpha(kAccent1, 0.20F);

    ext[core::ThemeColorToken::ControlFgNormal] = kTextMain;
    ext[core::ThemeColorToken::ControlFgDisabled] = disabled_variant(kTextMain);

    ext[core::ThemeColorToken::ControlBorderNormal] = kBorderLight;
    ext[core::ThemeColorToken::ControlBorderFocus] = kAccent1;

    ext[core::ThemeColorToken::FocusRingColor] = focus_ring(kAccent1);

    // Some missing V9 phase 3 tokens
    ext[core::ThemeColorToken::SidebarBg] = base_theme.colors.bg_app.to_wx_colour();
    ext[core::ThemeColorToken::SidebarFg] = base_theme.colors.text_muted.to_wx_colour();

    ext[core::ThemeColorToken::ActivityBarBg] = base_theme.colors.bg_app.to_wx_colour();
    ext[core::ThemeColorToken::ActivityBarFg] = base_theme.colors.text_muted.to_wx_colour();
    ext[core::ThemeColorToken::ActivityBarBadgeBg] = kAccent1;
    const wxColour kAccFg = meets_wcag_aa(wxColour(255, 255, 255), kAccent1)
                                ? wxColour(255, 255, 255)
                                : wxColour(0, 0, 0);
    ext[core::ThemeColorToken::ActivityBarBadgeFg] = kAccFg;

    ext[core::ThemeColorToken::BreadcrumbFg] = base_theme.colors.text_muted.to_wx_colour();
    ext[core::ThemeColorToken::BreadcrumbFocusFg] = kTextMain;

    ext[core::ThemeColorToken::HoverBg] = with_alpha(kTextMain, 0.10F);
    ext[core::ThemeColorToken::SelectionBg] = ext[core::ThemeColorToken::ControlBgSelected];

    // Defaults for tabs
    ext[core::ThemeColorToken::TabActiveBg] = base_theme.colors.bg_panel.to_wx_colour();
    ext[core::ThemeColorToken::TabInactiveBg] = darken(kBgPanel, 0.05F);
    ext[core::ThemeColorToken::TabActiveFg] = kTextMain;
    ext[core::ThemeColorToken::TabInactiveFg] = base_theme.colors.text_muted.to_wx_colour();

    return ext;
}

} // namespace markamp::ui
