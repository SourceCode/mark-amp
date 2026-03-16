// ============================================================================
// File: src/rendering/FxCssGenerator.h
// Phase 28: FX Visual Effects System — CSS effects generation
// ============================================================================
#pragma once

#include "FxPresetRegistry.h"

#include <string>

namespace markamp::rendering
{

/// Generates CSS stylesheets for FX effects applied in the preview panel
/// and HTML export. Maps FxPreset settings to CSS text-shadow, box-shadow,
/// filters, @keyframes, and CSS custom properties.
class FxCssGenerator
{
public:
    FxCssGenerator() = default;

    // ── CSS generation from component settings ──

    /// Generate CSS for text effects (shadow, glow, gradient, bloom).
    [[nodiscard]] auto generate_text_effects_css(const core::TextEffects& effects) const
        -> std::string;

    /// Generate CSS for UI element effects (hover glow, button depth, etc.).
    [[nodiscard]] auto generate_ui_effects_css(const core::UiElementEffects& effects) const
        -> std::string;

    /// Generate CSS for editor effects (cursor glow, line highlight bloom).
    [[nodiscard]] auto generate_editor_effects_css(const core::EditorEffects& effects) const
        -> std::string;

    /// Generate CSS for canvas effects (grid glow, connector glow).
    [[nodiscard]] auto generate_canvas_effects_css(const core::CanvasEffects& effects) const
        -> std::string;

    // ── Full preset CSS ──

    /// Generate a complete CSS stylesheet from an FxPreset.
    [[nodiscard]] auto generate_full_stylesheet(const FxPreset& preset) const -> std::string;

    /// Generate CSS custom properties (--fx-*) for theme integration.
    [[nodiscard]] auto generate_css_variables(const FxPreset& preset) const -> std::string;

    // ── Animation keyframes ──

    /// Generate @keyframes for motion preset transitions.
    [[nodiscard]] auto generate_animation_keyframes(const MotionPreset& motion) const
        -> std::string;

    // ── Utilities ──

    /// Generate a CSS color string (rgba format) from a Color.
    [[nodiscard]] static auto css_color(const core::Color& color, uint8_t alpha = 255)
        -> std::string;

    /// Generate a CSS text-shadow declaration from parameters.
    [[nodiscard]] static auto css_text_shadow(float offset_x,
                                              float offset_y,
                                              float blur_radius,
                                              const core::Color& color,
                                              uint8_t alpha) -> std::string;

    /// Check if a preset has any visual effects active.
    [[nodiscard]] static auto has_active_effects(const FxPreset& preset) -> bool;

    /// Number of CSS generation methods (text, ui, editor, canvas).
    [[nodiscard]] static constexpr auto total_generators() noexcept -> int
    {
        return 4;
    }

    /// Number of static utility methods (css_color, css_text_shadow, has_active_effects).
    [[nodiscard]] static constexpr auto total_utilities() noexcept -> int
    {
        return 3;
    }
};

} // namespace markamp::rendering
