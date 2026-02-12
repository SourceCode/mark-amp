#pragma once

#include "Color.h"

#include <nlohmann/json_fwd.hpp>

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// The 10 color tokens from the reference implementation.
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

    // Editor specific
    Color editor_bg;
    Color editor_fg;
    Color editor_selection;
    Color editor_line_number;
    Color editor_cursor;
    Color editor_gutter;

    // UI specific
    Color list_hover;
    Color list_selected;
    Color scrollbar_thumb;
    Color scrollbar_track;

    auto operator<=>(const ThemeColors&) const = default;
    auto operator==(const ThemeColors&) const -> bool = default;
};

// ---------------------------------------------------------------------------
// Phase 4: Layered theme token system
// ---------------------------------------------------------------------------

/// Layer 1: UI chrome colors (window, panels, borders, title bar).
struct ThemeChromeColors
{
    Color bg_app;
    Color bg_panel;
    Color bg_header;
    Color bg_input;
    Color border_light;
    Color border_dark;
    Color accent_primary;
    Color accent_secondary;
    Color list_hover;
    Color list_selected;
    Color scrollbar_thumb;
    Color scrollbar_track;

    auto operator<=>(const ThemeChromeColors&) const = default;
    auto operator==(const ThemeChromeColors&) const -> bool = default;
};

/// Layer 2: Editor syntax-highlighting colors.
struct ThemeSyntaxColors
{
    Color editor_bg;
    Color editor_fg;
    Color editor_selection;
    Color editor_line_number;
    Color editor_cursor;
    Color editor_gutter;

    // Syntax categories
    Color keyword{140, 120, 255}; // language keywords
    Color string_literal{80, 200, 120};
    Color comment{100, 100, 130};
    Color number{240, 180, 100};
    Color type_name{100, 180, 240};
    Color function_name{220, 200, 100};
    Color operator_tok{200, 200, 220};
    Color preprocessor{180, 140, 200};

    auto operator<=>(const ThemeSyntaxColors&) const = default;
    auto operator==(const ThemeSyntaxColors&) const -> bool = default;
};

/// Layer 3: Preview/render colors for rendered markdown.
struct ThemeRenderColors
{
    Color heading{230, 230, 250};
    Color link{100, 160, 255};
    Color code_bg{30, 30, 50};
    Color code_fg{200, 200, 220};
    Color blockquote_border{100, 99, 255};
    Color blockquote_bg{25, 25, 45};
    Color table_border{60, 60, 80};
    Color table_header_bg{35, 35, 55};

    auto operator<=>(const ThemeRenderColors&) const = default;
    auto operator==(const ThemeRenderColors&) const -> bool = default;
};

/// Title bar gradient specification.
struct TitleBarGradient
{
    std::optional<std::string> start; // hex color or empty
    std::optional<std::string> end;   // hex color or empty

    [[nodiscard]] auto is_enabled() const -> bool
    {
        return start.has_value() && end.has_value();
    }

    auto operator==(const TitleBarGradient&) const -> bool = default;
};

/// Comprehensive window-level visual effects.
struct WindowEffects
{
    // Frosted glass / vibrancy (macOS only — NSVisualEffectView)
    bool frosted_glass{false};

    // Inner shadow on chrome edges
    bool inner_shadow{false};
    int inner_shadow_radius{3};     // 1–8 px
    uint8_t inner_shadow_alpha{40}; // 0–255

    // Edge glow — drawn along window border (supersedes legacy neon_edge)
    bool edge_glow{false};
    Color edge_glow_color{100, 99, 255};
    int edge_glow_width{1};      // 1–4 px
    uint8_t edge_glow_alpha{60}; // 0–255

    // Vignette — subtle radial darkening at window edges
    bool vignette{false};
    uint8_t vignette_strength{30}; // 0–255

    // Surface blur — frosted backdrop on panel dividers (macOS only)
    bool surface_blur{false};

    auto operator==(const WindowEffects&) const -> bool = default;
};

/// Full theme data with ID, name, and color tokens.
struct Theme
{
    std::string id;
    std::string name;
    ThemeColors colors;

    // Phase 4: Layered token system (optional — populated from ThemeColors if empty)
    ThemeChromeColors chrome;
    ThemeSyntaxColors syntax;
    ThemeRenderColors render;

    // Phase 4: Title bar gradient
    TitleBarGradient title_bar_gradient;

    // Phase 4: Neon edge glow (1px inner glow with accent at low alpha)
    // DEPRECATED: Use effects.edge_glow instead. Kept for backward compatibility.
    bool neon_edge{false};

    // Phase 4D+: Comprehensive window effects
    WindowEffects effects;

    // Validation
    [[nodiscard]] auto is_valid() const -> bool;
    [[nodiscard]] auto validation_errors() const -> std::vector<std::string>;

    // Derived colors (computed from the base 10)
    [[nodiscard]] auto scrollbar_track() const -> Color;
    [[nodiscard]] auto scrollbar_thumb() const -> Color;
    [[nodiscard]] auto scrollbar_hover() const -> Color;
    [[nodiscard]] auto selection_bg() const -> Color;
    [[nodiscard]] auto hover_bg() const -> Color;
    [[nodiscard]] auto error_color() const -> Color;
    [[nodiscard]] auto success_color() const -> Color;
    [[nodiscard]] auto is_dark() const -> bool;

    /// Populate chrome/syntax/render layers from the flat ThemeColors struct.
    void sync_layers_from_colors();

    auto operator==(const Theme&) const -> bool = default;
};

/// Lightweight theme metadata for listing.
struct ThemeInfo
{
    std::string id;
    std::string name;
    bool is_builtin{false};
    std::optional<std::string> file_path;
};

// JSON serialization (ADL for nlohmann/json)
void to_json(nlohmann::json& j, const Color& c);
void from_json(const nlohmann::json& j, Color& c);
void to_json(nlohmann::json& j, const ThemeColors& tc);
void from_json(const nlohmann::json& j, ThemeColors& tc);
void to_json(nlohmann::json& j, const Theme& t);
void from_json(const nlohmann::json& j, Theme& t);

} // namespace markamp::core
