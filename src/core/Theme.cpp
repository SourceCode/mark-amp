#include "Theme.h"

#include <nlohmann/json.hpp>

namespace markamp::core
{

// --- Theme validation ---

auto Theme::is_valid() const -> bool
{
    return validation_errors().empty();
}

auto Theme::validation_errors() const -> std::vector<std::string>
{
    std::vector<std::string> errors;

    if (id.empty())
    {
        errors.emplace_back("Theme ID is empty");
    }
    if (name.empty())
    {
        errors.emplace_back("Theme name is empty");
    }

    // Check that key colors are not fully transparent (likely a parsing error)
    if (colors.bg_app.a == 0)
    {
        errors.emplace_back("bg_app is fully transparent");
    }
    if (colors.text_main.a == 0)
    {
        errors.emplace_back("text_main is fully transparent");
    }
    if (colors.accent_primary.a == 0)
    {
        errors.emplace_back("accent_primary is fully transparent");
    }

    return errors;
}

// --- Derived colors ---

auto Theme::scrollbar_track() const -> Color
{
    return colors.scrollbar_track;
}
auto Theme::scrollbar_thumb() const -> Color
{
    return colors.scrollbar_thumb;
}
auto Theme::scrollbar_hover() const -> Color
{
    return colors.scrollbar_thumb.lighten(0.1f); // Simple lighten for hover state
}

auto Theme::selection_bg() const -> Color
{
    return colors.list_selected;
}

auto Theme::hover_bg() const -> Color
{
    return colors.list_hover;
}

auto Theme::error_color() const -> Color
{
    // A red that contrasts well with the background
    if (is_dark())
    {
        return Color::from_rgb(255, 85, 85); // bright red on dark
    }
    return Color::from_rgb(200, 0, 0); // dark red on light
}

auto Theme::success_color() const -> Color
{
    if (is_dark())
    {
        return Color::from_rgb(85, 255, 85); // bright green on dark
    }
    return Color::from_rgb(0, 150, 0); // dark green on light
}

auto Theme::is_dark() const -> bool
{
    return colors.bg_app.luminance() < 0.2f;
}

auto Theme::hover_safe() const -> Color
{
    // Derive a contrast-safe hover state from accent_primary and bg_app.
    // Lighten or darken accent to ensure WCAG AA (4.5:1) against bg_app.
    auto candidate = colors.accent_primary.with_alpha(0.15f);
    auto blended = Color{
        static_cast<uint8_t>(colors.bg_app.r +
                             static_cast<int>((candidate.r - colors.bg_app.r) * 0.15f)),
        static_cast<uint8_t>(colors.bg_app.g +
                             static_cast<int>((candidate.g - colors.bg_app.g) * 0.15f)),
        static_cast<uint8_t>(colors.bg_app.b +
                             static_cast<int>((candidate.b - colors.bg_app.b) * 0.15f)),
    };
    return blended;
}

auto Theme::selected_safe() const -> Color
{
    // Stronger accent blend for selected state.
    auto candidate = colors.accent_primary.with_alpha(0.25f);
    auto blended = Color{
        static_cast<uint8_t>(colors.bg_app.r +
                             static_cast<int>((candidate.r - colors.bg_app.r) * 0.25f)),
        static_cast<uint8_t>(colors.bg_app.g +
                             static_cast<int>((candidate.g - colors.bg_app.g) * 0.25f)),
        static_cast<uint8_t>(colors.bg_app.b +
                             static_cast<int>((candidate.b - colors.bg_app.b) * 0.25f)),
    };
    return blended;
}

auto Theme::inherits_from(const std::string& theme_id) const -> bool
{
    return !parent_theme_id.empty() && parent_theme_id == theme_id;
}

void Theme::sync_layers_from_colors()
{
    // Chrome layer — from base + UI tokens
    chrome.bg_app = colors.bg_app;
    chrome.bg_panel = colors.bg_panel;
    chrome.bg_header = colors.bg_header;
    chrome.bg_input = colors.bg_input;
    chrome.border_light = colors.border_light;
    chrome.border_dark = colors.border_dark;
    chrome.accent_primary = colors.accent_primary;
    chrome.accent_secondary = colors.accent_secondary;
    chrome.list_hover = colors.list_hover;
    chrome.list_selected = colors.list_selected;
    chrome.scrollbar_thumb = colors.scrollbar_thumb;
    chrome.scrollbar_track = colors.scrollbar_track;

    // Syntax layer — from editor tokens
    syntax.editor_bg = colors.editor_bg;
    syntax.editor_fg = colors.editor_fg;
    syntax.editor_selection = colors.editor_selection;
    syntax.editor_line_number = colors.editor_line_number;
    syntax.editor_cursor = colors.editor_cursor;
    syntax.editor_gutter = colors.editor_gutter;
    // keyword/string/comment/etc. keep their defaults from ThemeSyntaxColors

    // Render layer keeps its defaults from ThemeRenderColors
    // (heading, link, code_bg, etc.)

    // Backward compat: migrate neon_edge → effects.edge_glow
    if (neon_edge && !effects.edge_glow)
    {
        effects.edge_glow = true;
        effects.edge_glow_color = colors.accent_primary;
    }
}

// --- JSON serialization ---

void to_json(nlohmann::json& j, const Color& c)
{
    j = c.to_theme_string();
}

void from_json(const nlohmann::json& j, Color& c)
{
    auto str = j.get<std::string>();
    auto result = Color::from_string(str);
    if (result)
    {
        c = *result;
    }
    else
    {
        throw nlohmann::json::other_error::create(601, "Invalid color: " + result.error(), &j);
    }
}

void to_json(nlohmann::json& j, const ThemeColors& tc)
{
    j = nlohmann::json{
        {"--bg-app", tc.bg_app},
        {"--bg-panel", tc.bg_panel},
        {"--bg-header", tc.bg_header},
        {"--bg-input", tc.bg_input},
        {"--text-main", tc.text_main},
        {"--text-muted", tc.text_muted},
        {"--accent-primary", tc.accent_primary},
        {"--accent-secondary", tc.accent_secondary},
        {"--border-light", tc.border_light},
        {"--border-dark", tc.border_dark},

        // Editor
        {"--editor-bg", tc.editor_bg},
        {"--editor-fg", tc.editor_fg},
        {"--editor-selection", tc.editor_selection},
        {"--editor-line-number", tc.editor_line_number},
        {"--editor-cursor", tc.editor_cursor},
        {"--editor-gutter", tc.editor_gutter},

        // UI
        {"--list-hover", tc.list_hover},
        {"--list-selected", tc.list_selected},
        {"--scrollbar-thumb", tc.scrollbar_thumb},
        {"--scrollbar-track", tc.scrollbar_track},

        // V9: Semantic UI tokens
        {"--sidebar-bg", tc.sidebar_bg},
        {"--sidebar-fg", tc.sidebar_fg},
        {"--activity-bar-bg", tc.activity_bar_bg},
        {"--activity-bar-fg", tc.activity_bar_fg},
        {"--activity-bar-badge-bg", tc.activity_bar_badge_bg},
        {"--activity-bar-badge-fg", tc.activity_bar_badge_fg},
        {"--breadcrumb-fg", tc.breadcrumb_fg},
        {"--breadcrumb-focus-fg", tc.breadcrumb_focus_fg},
        {"--tab-active-bg", tc.tab_active_bg},
        {"--tab-inactive-bg", tc.tab_inactive_bg},
        {"--tab-active-fg", tc.tab_active_fg},
        {"--tab-inactive-fg", tc.tab_inactive_fg},
        {"--diff-inserted-bg", tc.diff_inserted_bg},
        {"--diff-removed-bg", tc.diff_removed_bg},
        {"--minimap-bg", tc.minimap_bg},
        {"--peek-view-border", tc.peek_view_border},
        {"--notebook-cell-bg", tc.notebook_cell_bg},
    };
}

void from_json(const nlohmann::json& j, ThemeColors& tc)
{
    // R20 Fix 36: Wrap required color fields in try-catch for descriptive error
    try
    {
        // Required base colors
        j.at("--bg-app").get_to(tc.bg_app);
        j.at("--bg-panel").get_to(tc.bg_panel);
        j.at("--bg-header").get_to(tc.bg_header);
        j.at("--bg-input").get_to(tc.bg_input);
        j.at("--text-main").get_to(tc.text_main);
        j.at("--text-muted").get_to(tc.text_muted);
        j.at("--accent-primary").get_to(tc.accent_primary);
        j.at("--accent-secondary").get_to(tc.accent_secondary);
        j.at("--border-light").get_to(tc.border_light);
        j.at("--border-dark").get_to(tc.border_dark);
    }
    catch (const nlohmann::json::out_of_range& ex)
    {
        throw nlohmann::json::other_error::create(
            602, std::string("Missing required theme color: ") + ex.what(), &j);
    }

    // Optional/New colors with fallbacks
    if (j.contains("--editor-bg"))
        j.at("--editor-bg").get_to(tc.editor_bg);
    else
        tc.editor_bg = tc.bg_input;

    if (j.contains("--editor-fg"))
        j.at("--editor-fg").get_to(tc.editor_fg);
    else
        tc.editor_fg = tc.text_main;

    if (j.contains("--editor-selection"))
        j.at("--editor-selection").get_to(tc.editor_selection);
    else
        tc.editor_selection = tc.accent_primary.with_alpha(0.2f);

    if (j.contains("--editor-line-number"))
        j.at("--editor-line-number").get_to(tc.editor_line_number);
    else
        tc.editor_line_number = tc.text_muted;

    if (j.contains("--editor-cursor"))
        j.at("--editor-cursor").get_to(tc.editor_cursor);
    else
        tc.editor_cursor = tc.accent_primary;

    if (j.contains("--editor-gutter"))
        j.at("--editor-gutter").get_to(tc.editor_gutter);
    else
        tc.editor_gutter = tc.bg_input; // Default gutter matches editor bg

    if (j.contains("--list-hover"))
        j.at("--list-hover").get_to(tc.list_hover);
    else
        tc.list_hover = tc.accent_primary.with_alpha(0.1f);

    if (j.contains("--list-selected"))
        j.at("--list-selected").get_to(tc.list_selected);
    else
        tc.list_selected = tc.accent_primary.with_alpha(0.2f);

    if (j.contains("--scrollbar-thumb"))
        j.at("--scrollbar-thumb").get_to(tc.scrollbar_thumb);
    else
        tc.scrollbar_thumb = tc.accent_secondary;

    if (j.contains("--scrollbar-track"))
        j.at("--scrollbar-track").get_to(tc.scrollbar_track);
    else
        tc.scrollbar_track = tc.bg_panel;

    // V9: Optional semantic UI tokens
    if (j.contains("--sidebar-bg"))
        j.at("--sidebar-bg").get_to(tc.sidebar_bg);
    if (j.contains("--sidebar-fg"))
        j.at("--sidebar-fg").get_to(tc.sidebar_fg);
    if (j.contains("--activity-bar-bg"))
        j.at("--activity-bar-bg").get_to(tc.activity_bar_bg);
    if (j.contains("--activity-bar-fg"))
        j.at("--activity-bar-fg").get_to(tc.activity_bar_fg);
    if (j.contains("--activity-bar-badge-bg"))
        j.at("--activity-bar-badge-bg").get_to(tc.activity_bar_badge_bg);
    if (j.contains("--activity-bar-badge-fg"))
        j.at("--activity-bar-badge-fg").get_to(tc.activity_bar_badge_fg);
    if (j.contains("--breadcrumb-fg"))
        j.at("--breadcrumb-fg").get_to(tc.breadcrumb_fg);
    if (j.contains("--breadcrumb-focus-fg"))
        j.at("--breadcrumb-focus-fg").get_to(tc.breadcrumb_focus_fg);
    if (j.contains("--tab-active-bg"))
        j.at("--tab-active-bg").get_to(tc.tab_active_bg);
    if (j.contains("--tab-inactive-bg"))
        j.at("--tab-inactive-bg").get_to(tc.tab_inactive_bg);
    if (j.contains("--tab-active-fg"))
        j.at("--tab-active-fg").get_to(tc.tab_active_fg);
    if (j.contains("--tab-inactive-fg"))
        j.at("--tab-inactive-fg").get_to(tc.tab_inactive_fg);
    if (j.contains("--diff-inserted-bg"))
        j.at("--diff-inserted-bg").get_to(tc.diff_inserted_bg);
    if (j.contains("--diff-removed-bg"))
        j.at("--diff-removed-bg").get_to(tc.diff_removed_bg);
    if (j.contains("--minimap-bg"))
        j.at("--minimap-bg").get_to(tc.minimap_bg);
    if (j.contains("--peek-view-border"))
        j.at("--peek-view-border").get_to(tc.peek_view_border);
    if (j.contains("--notebook-cell-bg"))
        j.at("--notebook-cell-bg").get_to(tc.notebook_cell_bg);
}

void to_json(nlohmann::json& j, const Theme& t)
{
    nlohmann::json effects_json;
    effects_json["frosted_glass"] = t.effects.frosted_glass;
    effects_json["inner_shadow"] = t.effects.inner_shadow;
    effects_json["inner_shadow_radius"] = t.effects.inner_shadow_radius;
    effects_json["inner_shadow_alpha"] = t.effects.inner_shadow_alpha;
    effects_json["edge_glow"] = t.effects.edge_glow;
    effects_json["edge_glow_color"] = t.effects.edge_glow_color;
    effects_json["edge_glow_width"] = t.effects.edge_glow_width;
    effects_json["edge_glow_alpha"] = t.effects.edge_glow_alpha;
    effects_json["vignette"] = t.effects.vignette;
    effects_json["vignette_strength"] = t.effects.vignette_strength;
    effects_json["surface_blur"] = t.effects.surface_blur;

    j = nlohmann::json{
        {"id", t.id},
        {"name", t.name},
        {"colors", t.colors},
        {"effects", effects_json},
        {"format_version", t.format_version},
    };
    if (!t.parent_theme_id.empty())
    {
        j["parent_theme_id"] = t.parent_theme_id;
    }
}

void from_json(const nlohmann::json& j, Theme& t)
{
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    j.at("colors").get_to(t.colors);

    // Parse WindowEffects if present
    if (j.contains("effects"))
    {
        const auto& eff = j.at("effects");
        if (eff.contains("frosted_glass"))
            eff.at("frosted_glass").get_to(t.effects.frosted_glass);
        if (eff.contains("inner_shadow"))
            eff.at("inner_shadow").get_to(t.effects.inner_shadow);
        if (eff.contains("inner_shadow_radius"))
            eff.at("inner_shadow_radius").get_to(t.effects.inner_shadow_radius);
        if (eff.contains("inner_shadow_alpha"))
            eff.at("inner_shadow_alpha").get_to(t.effects.inner_shadow_alpha);
        if (eff.contains("edge_glow"))
            eff.at("edge_glow").get_to(t.effects.edge_glow);
        if (eff.contains("edge_glow_color"))
            eff.at("edge_glow_color").get_to(t.effects.edge_glow_color);
        if (eff.contains("edge_glow_width"))
            eff.at("edge_glow_width").get_to(t.effects.edge_glow_width);
        if (eff.contains("edge_glow_alpha"))
            eff.at("edge_glow_alpha").get_to(t.effects.edge_glow_alpha);
        if (eff.contains("vignette"))
            eff.at("vignette").get_to(t.effects.vignette);
        if (eff.contains("vignette_strength"))
            eff.at("vignette_strength").get_to(t.effects.vignette_strength);
        if (eff.contains("surface_blur"))
            eff.at("surface_blur").get_to(t.effects.surface_blur);
    }

    // Backward compat: read legacy neon_edge
    if (j.contains("neon_edge"))
    {
        j.at("neon_edge").get_to(t.neon_edge);
    }

    // V9: format_version and parent_theme_id
    if (j.contains("format_version"))
    {
        j.at("format_version").get_to(t.format_version);
    }
    if (j.contains("parent_theme_id"))
    {
        j.at("parent_theme_id").get_to(t.parent_theme_id);
    }
}

} // namespace markamp::core
