/// @file ThemeAutoRepair.cpp
/// @brief V9 Phase 3 — Theme auto-repair implementation.

#include "core/ThemeAutoRepair.h"

#include "core/Logger.h"

#include <cmath>

namespace markamp::core
{

namespace
{

auto is_default_black(const Color& clr) -> bool
{
    return clr.r == 0 && clr.g == 0 && clr.b == 0;
}

auto color_to_hex(const Color& clr) -> std::string
{
    return clr.to_hex();
}

/// Darken a color by a factor (0.0 = no change, 1.0 = black).
auto darken(const Color& clr, double factor) -> Color
{
    auto dim = [](uint8_t val, double fac) -> uint8_t
    { return static_cast<uint8_t>(static_cast<double>(val) * (1.0 - fac)); };
    return Color{dim(clr.r, factor), dim(clr.g, factor), dim(clr.b, factor)};
}

} // anonymous namespace

auto ThemeAutoRepair::repair(Theme& theme) -> std::vector<RepairAction>
{
    std::vector<RepairAction> actions;

    auto missing = fill_missing_tokens(theme);
    actions.insert(actions.end(), missing.begin(), missing.end());

    auto contrast = fix_contrast_violations(theme);
    actions.insert(actions.end(), contrast.begin(), contrast.end());

    if (!actions.empty())
    {
        MARKAMP_LOG_INFO("ThemeAutoRepair: {} repairs applied to '{}'", actions.size(), theme.name);
    }

    return actions;
}

auto ThemeAutoRepair::needs_repair(const Theme& theme) -> bool
{
    if (is_default_black(theme.colors.sidebar_bg) || is_default_black(theme.colors.activity_bar_bg))
    {
        return true;
    }

    constexpr float kMinContrast = 4.5F; // WCAG AA
    if (theme.colors.text_main.contrast_ratio(theme.colors.bg_app) < kMinContrast)
    {
        return true;
    }

    return false;
}

auto ThemeAutoRepair::fill_missing_tokens(Theme& theme) -> std::vector<RepairAction>
{
    std::vector<RepairAction> actions;

    auto fill = [&](const std::string& name, Color& target, const Color& fallback)
    {
        if (is_default_black(target))
        {
            RepairAction action;
            action.token_name = name;
            action.repair_type = "missing_fill";
            action.old_value = "none";
            action.new_value = color_to_hex(fallback);
            target = fallback;
            actions.push_back(std::move(action));
        }
    };

    fill("sidebar_bg", theme.colors.sidebar_bg, darken(theme.colors.bg_panel, 0.1));
    fill("sidebar_fg", theme.colors.sidebar_fg, theme.colors.text_main);
    fill("activity_bar_bg", theme.colors.activity_bar_bg, darken(theme.colors.bg_app, 0.2));
    fill("activity_bar_fg", theme.colors.activity_bar_fg, theme.colors.text_muted);
    fill("activity_bar_badge_bg", theme.colors.activity_bar_badge_bg, theme.colors.accent_primary);
    fill("activity_bar_badge_fg", theme.colors.activity_bar_badge_fg, Color{255, 255, 255});
    fill("breadcrumb_fg", theme.colors.breadcrumb_fg, theme.colors.text_muted);
    fill("breadcrumb_focus_fg", theme.colors.breadcrumb_focus_fg, theme.colors.text_main);
    fill("tab_active_bg", theme.colors.tab_active_bg, theme.colors.bg_panel);
    fill("tab_inactive_bg", theme.colors.tab_inactive_bg, darken(theme.colors.bg_panel, 0.15));
    fill("tab_active_fg", theme.colors.tab_active_fg, theme.colors.text_main);
    fill("tab_inactive_fg", theme.colors.tab_inactive_fg, theme.colors.text_muted);
    fill("diff_inserted_bg", theme.colors.diff_inserted_bg, Color{30, 80, 50});
    fill("diff_removed_bg", theme.colors.diff_removed_bg, Color{80, 30, 40});
    fill("minimap_bg", theme.colors.minimap_bg, darken(theme.colors.bg_app, 0.1));
    fill("peek_view_border", theme.colors.peek_view_border, theme.colors.accent_primary);
    fill("notebook_cell_bg", theme.colors.notebook_cell_bg, theme.colors.bg_panel);

    return actions;
}

auto ThemeAutoRepair::fix_contrast_violations(Theme& theme) -> std::vector<RepairAction>
{
    std::vector<RepairAction> actions;
    constexpr float kMinContrast = 4.5F; // WCAG AA

    auto fix_pair = [&](const std::string& fg_name, Color& fg_color, const Color& bg_color)
    {
        float ratio = fg_color.contrast_ratio(bg_color);
        if (ratio < kMinContrast)
        {
            RepairAction action;
            action.token_name = fg_name;
            action.repair_type = "contrast_fix";
            action.old_value = color_to_hex(fg_color);

            // Lighten foreground if on dark background
            fg_color = fg_color.lighten(0.3F);

            action.new_value = color_to_hex(fg_color);
            actions.push_back(std::move(action));
        }
    };

    fix_pair("text_main", theme.colors.text_main, theme.colors.bg_app);
    fix_pair("text_muted", theme.colors.text_muted, theme.colors.bg_app);
    fix_pair("sidebar_fg", theme.colors.sidebar_fg, theme.colors.sidebar_bg);
    fix_pair("tab_active_fg", theme.colors.tab_active_fg, theme.colors.tab_active_bg);
    fix_pair("tab_inactive_fg", theme.colors.tab_inactive_fg, theme.colors.tab_inactive_bg);

    return actions;
}

} // namespace markamp::core
