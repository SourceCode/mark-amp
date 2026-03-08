#pragma once

#include "Color.h"
#include "ThemeTokens.h"

#include <nlohmann/json_fwd.hpp>
#include <wx/colour.h>

#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
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

    // V8 Phase 9: Semantic editor state colors
    Color editor_active_line{30, 30, 55};      // Active line highlight
    Color editor_gutter_error{220, 60, 60};    // Gutter error marker
    Color editor_gutter_warn{220, 180, 50};    // Gutter warning marker
    Color editor_gutter_info{80, 160, 240};    // Gutter info marker
    Color editor_match_highlight{60, 60, 100}; // Word match highlight
    Color editor_find_hit{120, 100, 40};       // Find search hit
    Color editor_quick_fix{60, 120, 60};       // Quick-fix lightbulb

    // UI specific
    Color list_hover;
    Color list_selected;
    Color scrollbar_thumb;
    Color scrollbar_track;

    // V9 Phase 3: Extended semantic UI colors
    Color sidebar_bg{30, 30, 50};
    Color sidebar_fg{200, 200, 220};
    Color activity_bar_bg{22, 22, 40};
    Color activity_bar_fg{180, 180, 200};
    Color activity_bar_badge_bg{100, 99, 255};
    Color activity_bar_badge_fg{255, 255, 255};
    Color breadcrumb_fg{160, 160, 180};
    Color breadcrumb_focus_fg{220, 220, 240};
    Color tab_active_bg{35, 35, 55};
    Color tab_inactive_bg{25, 25, 42};
    Color tab_active_fg{220, 220, 240};
    Color tab_inactive_fg{140, 140, 160};
    Color diff_inserted_bg{30, 80, 50};
    Color diff_removed_bg{80, 30, 40};
    Color minimap_bg{20, 20, 38};
    Color peek_view_border{100, 99, 255};
    Color notebook_cell_bg{28, 28, 48};

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

    // Syntax categories (original 8)
    Color keyword{140, 120, 255}; // language keywords
    Color string_literal{80, 200, 120};
    Color comment{100, 100, 130};
    Color number{240, 180, 100};
    Color type_name{100, 180, 240};
    Color function_name{220, 200, 100};
    Color operator_tok{200, 200, 220};
    Color preprocessor{180, 140, 200};

    // V16 Phase 16: Fine-grained syntax colors
    Color variable{200, 200, 220};       // variable.other
    Color constant{220, 120, 120};       // constant.language
    Color tag{240, 120, 100};            // entity.name.tag (HTML/XML)
    Color attribute{160, 200, 120};      // entity.other.attribute-name
    Color property{180, 180, 240};       // variable.other.property
    Color namespace_name{140, 200, 200}; // entity.name.namespace
    Color enum_name{120, 200, 180};      // entity.name.type.enum
    Color interface_name{140, 220, 220}; // entity.name.type.interface
    Color struct_name{120, 180, 220};    // entity.name.type.struct
    Color parameter{200, 160, 160};      // variable.parameter
    Color decorator{220, 160, 100};      // meta.decorator
    Color regex{200, 100, 140};          // string.regexp
    Color escape{220, 140, 160};         // constant.character.escape
    Color doc_comment{120, 140, 100};    // comment.block.documentation
    Color macro{200, 140, 220};          // entity.name.function.macro

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

/// V8 Phase 13 (Phase 47): Compiz/Beryl-style transition/motion effects.
struct TransitionEffects
{
    // Wobbly drag
    float wobbly_stiffness{0.5F};        ///< 0.0–1.0
    float wobbly_damping{0.7F};          ///< 0.0–1.0
    float wobbly_max_displacement{8.0F}; ///< pixels

    // Glide transitions
    float glide_duration_ms{250.0F}; ///< milliseconds
    float glide_overshoot{0.05F};    ///< 0.0–0.3

    // Zoom/fade
    float zoom_fade_scale_min{0.9F}; ///< 0.5–1.0
    float zoom_fade_scale_max{1.0F};

    // Magic lamp minimize
    float magic_lamp_bend{0.4F}; ///< 0.0–1.0
    float magic_lamp_duration_ms{350.0F};

    // Burn/fade close
    float burn_close_particle_density{0.6F};
    float burn_close_fade_speed{1.0F};

    // Expo overview
    float expo_spacing{0.1F};
    float expo_blur{0.3F};
    float expo_anim_speed{1.0F};

    // Cube workspace
    float cube_perspective{0.8F};
    float cube_rotation_speed{1.0F};

    auto operator==(const TransitionEffects&) const -> bool = default;
};

/// V8 Phase 13 (Phase 48): Text visual effects.
struct TextEffects
{
    // Stroke / outline
    bool stroke_enabled{false};
    float stroke_width{1.0F};
    Color stroke_color{0, 0, 0};

    // Soft shadow
    bool shadow_enabled{false};
    float shadow_offset_x{1.0F};
    float shadow_offset_y{1.0F};
    float shadow_blur{2.0F};
    uint8_t shadow_alpha{80};

    // Outer glow
    bool outer_glow_enabled{false};
    float outer_glow_radius{3.0F};
    Color outer_glow_color{100, 99, 255};
    uint8_t outer_glow_alpha{40};

    // Inner glow
    bool inner_glow_enabled{false};
    float inner_glow_radius{2.0F};
    Color inner_glow_color{255, 255, 255};
    uint8_t inner_glow_alpha{30};

    // Gradient fill
    bool gradient_fill_enabled{false};
    Color gradient_start{100, 99, 255};
    Color gradient_end{255, 100, 200};
    float gradient_angle{90.0F}; ///< degrees

    // Bloom
    bool bloom_enabled{false};
    float bloom_threshold{0.8F};
    float bloom_intensity{0.3F};

    // CRT / scanline mode
    bool crt_mode{false};
    uint8_t crt_scanline_alpha{20};
    float crt_curvature{0.02F};

    // Subpixel hinting
    bool subpixel_hinting{true};
    float subpixel_strength{0.8F};

    // Readability clamp
    float readability_min_contrast{4.5F}; ///< WCAG AA target
    bool readability_halo_guard{true};

    auto operator==(const TextEffects&) const -> bool = default;
};

/// V8 Phase 13 (Phase 49): UI element effects.
struct UiElementEffects
{
    // Hover glow
    float hover_glow_radius{4.0F};
    Color hover_glow_color{100, 99, 255};
    uint8_t hover_glow_alpha{30};

    // Active pill bloom
    float active_pill_bloom{0.3F};
    float active_pill_corner_radius{4.0F};

    // Button press depth
    float button_press_depth{1.0F};
    float button_shadow_reduction{0.3F};

    // Focus ring
    Color focus_ring_color{100, 99, 255};
    float focus_ring_thickness{2.0F};
    bool focus_ring_pulse{false};

    // Panel drop shadow
    float panel_shadow_blur{8.0F};
    float panel_shadow_spread{2.0F};
    uint8_t panel_shadow_alpha{40};

    // Tooltip frost
    float tooltip_blur{6.0F};
    Color tooltip_tint{30, 30, 50};
    bool tooltip_border_glow{false};

    // Badge glow
    float badge_glow_alpha{0.4F};
    bool badge_glow_pulse{false};

    auto operator==(const UiElementEffects&) const -> bool = default;
};

/// V8 Phase 13: Editor-specific effects.
struct EditorEffects
{
    // Caret glow
    bool caret_glow_enabled{false};
    float caret_glow_radius{3.0F};
    uint8_t caret_glow_alpha{40};

    // Selection shimmer
    bool selection_shimmer{false};
    float selection_shimmer_speed{1.0F};

    // Active line glow
    bool active_line_glow{false};
    float active_line_glow_strength{0.2F};
    float active_line_glow_spread{2.0F};

    // Diagnostic pulse (per-severity)
    bool diagnostic_pulse{false};

    auto operator==(const EditorEffects&) const -> bool = default;
};

/// V8 Phase 13: Canvas-specific effects.
struct CanvasEffects
{
    // Object shadow
    float object_shadow_blur{6.0F};
    uint8_t object_shadow_alpha{50};
    float object_shadow_elevation{2.0F};

    // Selection outline glow
    Color selection_glow_color{100, 99, 255};
    float selection_glow_width{2.0F};
    bool selection_glow_pulse{false};

    // Connector neon
    float connector_neon_intensity{0.3F};

    // Sticky note lighting
    bool sticky_note_lighting{false};

    // Minimap heat
    float minimap_heat_strength{0.3F};

    auto operator==(const CanvasEffects&) const -> bool = default;
};

/// V8 Phase 13: Top-level FX settings grouping all effect categories.
struct FxSettings
{
    // Global controls
    bool master_enabled{false};            ///< Global FX on/off
    bool reduced_motion{false};            ///< Accessibility: force static
    bool low_power_mode{false};            ///< Disable expensive passes
    bool text_safety_mode{false};          ///< No distortion/chroma on text
    bool high_contrast_guard{true};        ///< Auto-adjust for contrast
    float max_frame_time_budget_ms{16.0F}; ///< Hard cap for adaptive downgrade

    // Per-category effect settings
    TransitionEffects transition;
    TextEffects text;
    UiElementEffects ui_elements;
    EditorEffects editor;
    CanvasEffects canvas;

    auto operator==(const FxSettings&) const -> bool = default;
};

/// Full theme data with ID, name, and color tokens.
struct Theme
{
    std::string id;
    std::string name;
    std::string author;          ///< Theme author (for VSCode imports)
    std::string description;     ///< Theme description
    std::string source;          ///< "built-in", "custom", "extension", "vscode-import"
    int format_version{1};       ///< V9: Persistence format version
    std::string parent_theme_id; ///< V9: Inheritance chain (empty = no parent)
    ThemeColors colors;

    // V2 Phase 3: Arbitrary scoped semantic tokens
    std::unordered_map<std::string, wxColour> semantic_tokens;

    // V2 Phase 3: TextMate scope customizations (from tokenColors)
    struct TokenColorRule
    {
        std::string scope;
        std::optional<Color> foreground;
        std::optional<std::string> font_style;

        auto operator==(const TokenColorRule&) const -> bool = default;
    };
    std::vector<TokenColorRule> token_colors;

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

    // V8 Phase 13 (Phase 46–56): Full FX pipeline settings
    FxSettings fx_settings;

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

    /// V9: Contrast-safe hover state (WCAG AA compliant against bg_app).
    [[nodiscard]] auto hover_safe() const -> Color;
    /// V9: Contrast-safe selection state (WCAG AA compliant against bg_app).
    [[nodiscard]] auto selected_safe() const -> Color;

    /// V9: Check if this theme inherits from a parent.
    [[nodiscard]] auto inherits_from(const std::string& theme_id) const -> bool;

    /// Populate chrome/syntax/render layers from the flat ThemeColors struct.
    void sync_layers_from_colors();

    // V8 Phase 12 (Phase 41): Interactive state tokens
    Color hover_bg_token{50, 50, 80};       ///< Hover background
    Color active_bg_token{40, 40, 70};      ///< Active/pressed background
    Color pressed_bg_token{35, 35, 65};     ///< Pressed state background
    Color focus_border_token{100, 99, 255}; ///< Focus ring/border
    Color error_fg_token{220, 60, 60};      ///< Error foreground
    Color warning_fg_token{220, 180, 50};   ///< Warning foreground
    Color info_fg_token{80, 160, 240};      ///< Info foreground
    Color success_fg_token{60, 180, 90};    ///< Success foreground

    // V10 Phase 02: Control state colors (defaults derived from chrome tokens)
    Color control_bg_normal{35, 35, 55};      ///< Control resting background
    Color control_bg_hover{50, 50, 80};       ///< Control hover background
    Color control_bg_pressed{30, 30, 50};     ///< Control pressed background
    Color control_bg_focus{40, 40, 70};       ///< Control focus background
    Color control_bg_disabled{35, 35, 55};    ///< Control disabled background (same as normal)
    Color control_bg_selected{45, 45, 85};    ///< Control selected background
    Color control_fg_normal{220, 220, 240};   ///< Control normal foreground
    Color control_fg_disabled{120, 120, 140}; ///< Control disabled foreground (dimmed)
    Color control_border_normal{60, 60, 80};  ///< Control normal border
    Color control_border_focus{100, 99, 255}; ///< Control focus border
    Color focus_ring_color{100, 99, 255};     ///< Focus ring color (typically accent)

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
