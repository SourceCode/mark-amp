/// TextFxRenderer.h — Phase 48: Text Visual Effects Renderer
///
/// Provides per-token/per-context text FX rendering with subpixel outline,
/// soft shadow, neon glow, gradient ink, bloom, and CRT/scanline filter.
/// Supports per-syntax-category FX channels and a clean mode fallback.

#pragma once

#include "core/Theme.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace markamp::rendering
{

/// Syntax category channels for per-token FX profiles.
enum class TextFxChannel : uint8_t
{
    kDefault,   ///< Default text (body, labels)
    kKeyword,   ///< Language keywords
    kString,    ///< String literals
    kComment,   ///< Comments
    kType,      ///< Type names
    kFunction,  ///< Function/method names
    kNumber,    ///< Numeric literals
    kOperator,  ///< Operators
    kHeading,   ///< Markdown headings
    kDiagnostic ///< Error/warning diagnostic text
};

/// Per-channel text FX profile.
struct TextFxProfile
{
    bool enabled{false};

    // Stroke
    float stroke_width{0.0F};
    core::Color stroke_color{0, 0, 0};

    // Shadow
    float shadow_offset_x{0.0F};
    float shadow_offset_y{0.0F};
    float shadow_blur{0.0F};
    uint8_t shadow_alpha{0};

    // Glow
    float glow_radius{0.0F};
    core::Color glow_color{100, 99, 255};
    uint8_t glow_alpha{0};

    // Gradient
    bool gradient_enabled{false};
    core::Color gradient_start{100, 99, 255};
    core::Color gradient_end{255, 100, 200};

    // Bloom
    float bloom_intensity{0.0F};

    auto operator==(const TextFxProfile&) const -> bool = default;
};

/// Renders text visual effects with per-token FX channels.
class TextFxRenderer
{
public:
    TextFxRenderer();

    /// Set the FX profile for a specific syntax channel.
    void set_profile(TextFxChannel channel, const TextFxProfile& profile);

    /// Get the profile for a channel (returns default if not set).
    [[nodiscard]] auto get_profile(TextFxChannel channel) const -> TextFxProfile;

    /// Check if any profiles are active.
    [[nodiscard]] auto has_active_profiles() const -> bool;

    /// Render text FX for a given channel. Returns true if effects were applied.
    [[nodiscard]] auto render_text_fx(TextFxChannel channel) const -> bool;

    /// Enable/disable clean mode (no effects, just crisp text).
    void set_clean_mode(bool enabled) noexcept;
    [[nodiscard]] auto is_clean_mode() const noexcept -> bool;

    /// Set the global text effects from theme.
    void apply_theme_text_effects(const core::TextEffects& text_effects);

    /// Get the number of configured channels.
    [[nodiscard]] auto channel_count() const noexcept -> std::size_t;

    /// Convert channel to display string.
    [[nodiscard]] static auto channel_name(TextFxChannel channel) -> std::string_view;

private:
    std::unordered_map<TextFxChannel, TextFxProfile> profiles_;
    bool clean_mode_{false};
};

} // namespace markamp::rendering
