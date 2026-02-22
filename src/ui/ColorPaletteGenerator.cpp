#include "ColorPaletteGenerator.h"

#include "ColorUtils.h"

namespace markamp::ui
{

auto ColorPaletteGenerator::hover_variant(const wxColour& base, bool is_dark_mode) const -> wxColour
{
    if (is_dark_mode)
    {
        return lighten(base, 0.10f);
    }
    else
    {
        return darken(base, 0.10f);
    }
}

auto ColorPaletteGenerator::pressed_variant(const wxColour& base, bool is_dark_mode) const
    -> wxColour
{
    if (is_dark_mode)
    {
        return lighten(base, 0.20f);
    }
    else
    {
        return darken(base, 0.20f);
    }
}

auto ColorPaletteGenerator::disabled_variant(const wxColour& base) const -> wxColour
{
    wxColour desat = desaturate(base, 0.50f);
    return with_alpha(desat, 0.50f);
}

auto ColorPaletteGenerator::focus_ring(const wxColour& accent) const -> wxColour
{
    return with_alpha(accent, 0.40f);
}

auto ColorPaletteGenerator::generate_extended_palette(const core::Theme& base_theme) const
    -> ExtendedPalette
{
    ExtendedPalette ext;

    // Determine if theme is dark mode based on background vs text luminosity
    bool is_dark_mode = true;
    HSLColor bg_hsl = rgb_to_hsl(base_theme.colors.bg_app.to_wx_colour());
    HSLColor fg_hsl = rgb_to_hsl(base_theme.colors.text_main.to_wx_colour());
    is_dark_mode = (bg_hsl.l < fg_hsl.l);

    wxColour bg_panel = base_theme.colors.bg_panel.to_wx_colour();
    wxColour text_main = base_theme.colors.text_main.to_wx_colour();
    const wxColour accent1 = base_theme.colors.accent_primary.to_wx_colour();
    const wxColour border_light = base_theme.colors.border_light.to_wx_colour();

    // Control States
    ext[core::ThemeColorToken::ControlBgNormal] = bg_panel;
    ext[core::ThemeColorToken::ControlBgHover] = hover_variant(bg_panel, is_dark_mode);
    ext[core::ThemeColorToken::ControlBgPressed] = pressed_variant(bg_panel, is_dark_mode);
    ext[core::ThemeColorToken::ControlBgFocus] = ext[core::ThemeColorToken::ControlBgHover];
    ext[core::ThemeColorToken::ControlBgDisabled] = disabled_variant(bg_panel);
    ext[core::ThemeColorToken::ControlBgSelected] = with_alpha(accent1, 0.20f);

    ext[core::ThemeColorToken::ControlFgNormal] = text_main;
    ext[core::ThemeColorToken::ControlFgDisabled] = disabled_variant(text_main);

    ext[core::ThemeColorToken::ControlBorderNormal] = border_light;
    ext[core::ThemeColorToken::ControlBorderFocus] = accent1;

    ext[core::ThemeColorToken::FocusRingColor] = focus_ring(accent1);

    // Some missing V9 phase 3 tokens
    ext[core::ThemeColorToken::SidebarBg] = base_theme.colors.bg_app.to_wx_colour();
    ext[core::ThemeColorToken::SidebarFg] = base_theme.colors.text_muted.to_wx_colour();

    ext[core::ThemeColorToken::ActivityBarBg] = base_theme.colors.bg_app.to_wx_colour();
    ext[core::ThemeColorToken::ActivityBarFg] = base_theme.colors.text_muted.to_wx_colour();
    ext[core::ThemeColorToken::ActivityBarBadgeBg] = accent1;
    const wxColour acc_fg = meets_wcag_aa(wxColour(255, 255, 255), accent1)
                                ? wxColour(255, 255, 255)
                                : wxColour(0, 0, 0);
    ext[core::ThemeColorToken::ActivityBarBadgeFg] = acc_fg;

    ext[core::ThemeColorToken::BreadcrumbFg] = base_theme.colors.text_muted.to_wx_colour();
    ext[core::ThemeColorToken::BreadcrumbFocusFg] = text_main;

    ext[core::ThemeColorToken::HoverBg] = with_alpha(text_main, 0.10f);
    ext[core::ThemeColorToken::SelectionBg] = ext[core::ThemeColorToken::ControlBgSelected];

    // Defaults for tabs
    ext[core::ThemeColorToken::TabActiveBg] = base_theme.colors.bg_panel.to_wx_colour();
    ext[core::ThemeColorToken::TabInactiveBg] = darken(bg_panel, 0.05f);
    ext[core::ThemeColorToken::TabActiveFg] = text_main;
    ext[core::ThemeColorToken::TabInactiveFg] = base_theme.colors.text_muted.to_wx_colour();

    return ext;
}

} // namespace markamp::ui
