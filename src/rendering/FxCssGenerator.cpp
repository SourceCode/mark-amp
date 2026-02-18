// ============================================================================
// File: src/rendering/FxCssGenerator.cpp
// Phase 28: FX Visual Effects System — CSS effects generation
// ============================================================================

#include "FxCssGenerator.h"

#include <cstdio>
#include <sstream>

namespace markamp::rendering
{

auto FxCssGenerator::generate_text_effects_css(const core::TextEffects& effects) const
    -> std::string
{
    std::ostringstream css;

    css << "/* FX Text Effects */\n";

    if (effects.stroke_enabled)
    {
        css << ".fx-text {\n"
            << "  -webkit-text-stroke: " << effects.stroke_width << "px "
            << css_color(effects.stroke_color) << ";\n"
            << "}\n";
    }

    if (effects.shadow_enabled)
    {
        css << ".fx-text-shadow {\n"
            << "  text-shadow: "
            << css_text_shadow(effects.shadow_offset_x,
                               effects.shadow_offset_y,
                               effects.shadow_blur,
                               {0, 0, 0},
                               effects.shadow_alpha)
            << ";\n}\n";
    }

    if (effects.outer_glow_enabled)
    {
        css << ".fx-text-glow {\n"
            << "  text-shadow: 0 0 " << effects.outer_glow_radius << "px "
            << css_color(effects.outer_glow_color, effects.outer_glow_alpha) << ";\n}\n";
    }

    if (effects.gradient_fill_enabled)
    {
        css << ".fx-text-gradient {\n"
            << "  background: linear-gradient(" << effects.gradient_angle << "deg, "
            << css_color(effects.gradient_start) << ", " << css_color(effects.gradient_end)
            << ");\n"
            << "  -webkit-background-clip: text;\n"
            << "  -webkit-text-fill-color: transparent;\n"
            << "}\n";
    }

    if (effects.bloom_enabled)
    {
        css << ".fx-text-bloom {\n"
            << "  filter: brightness(" << (1.0F + effects.bloom_intensity) << ");\n"
            << "}\n";
    }

    if (effects.crt_mode)
    {
        css << ".fx-crt-overlay {\n"
            << "  background: repeating-linear-gradient(\n"
            << "    0deg,\n"
            << "    rgba(0,0,0," << (static_cast<float>(effects.crt_scanline_alpha) / 255.0F)
            << ") 0px,\n"
            << "    transparent 1px,\n"
            << "    transparent 2px\n"
            << "  );\n"
            << "}\n";
    }

    return css.str();
}

auto FxCssGenerator::generate_ui_effects_css(const core::UiElementEffects& effects) const
    -> std::string
{
    std::ostringstream css;

    css << "/* FX UI Element Effects */\n";

    css << ".fx-hover-glow:hover {\n"
        << "  box-shadow: 0 0 " << effects.hover_glow_radius << "px "
        << css_color(effects.hover_glow_color, effects.hover_glow_alpha) << ";\n"
        << "}\n";

    css << ".fx-button:active {\n"
        << "  transform: translateY(" << effects.button_press_depth << "px);\n"
        << "}\n";

    css << ".fx-focus-ring:focus {\n"
        << "  outline: " << effects.focus_ring_thickness << "px solid "
        << css_color(effects.focus_ring_color) << ";\n"
        << "  outline-offset: 2px;\n"
        << "}\n";

    css << ".fx-panel-shadow {\n"
        << "  box-shadow: 0 0 " << effects.panel_shadow_blur << "px " << effects.panel_shadow_spread
        << "px " << css_color({0, 0, 0}, effects.panel_shadow_alpha) << ";\n"
        << "}\n";

    if (effects.tooltip_border_glow)
    {
        css << ".fx-tooltip {\n"
            << "  backdrop-filter: blur(" << effects.tooltip_blur << "px);\n"
            << "  border: 1px solid "
            << css_color(effects.hover_glow_color, effects.hover_glow_alpha) << ";\n"
            << "}\n";
    }

    return css.str();
}

auto FxCssGenerator::generate_editor_effects_css(const core::EditorEffects& effects) const
    -> std::string
{
    std::ostringstream css;

    css << "/* FX Editor Effects */\n";

    if (effects.caret_glow_enabled)
    {
        css << ".fx-caret {\n"
            << "  box-shadow: 0 0 " << effects.caret_glow_radius << "px "
            << "rgba(100, 99, 255, " << (static_cast<float>(effects.caret_glow_alpha) / 255.0F)
            << ");\n}\n";
    }

    if (effects.active_line_glow)
    {
        css << ".fx-active-line {\n"
            << "  box-shadow: inset 0 0 " << effects.active_line_glow_spread << "px "
            << "rgba(100, 99, 255, " << effects.active_line_glow_strength << ");\n"
            << "}\n";
    }

    if (effects.selection_shimmer)
    {
        css << "@keyframes fx-shimmer {\n"
            << "  0% { opacity: 0.5; }\n"
            << "  50% { opacity: 0.8; }\n"
            << "  100% { opacity: 0.5; }\n"
            << "}\n"
            << ".fx-selection {\n"
            << "  animation: fx-shimmer " << (2.0F / effects.selection_shimmer_speed) << "s "
            << "ease-in-out infinite;\n"
            << "}\n";
    }

    return css.str();
}

auto FxCssGenerator::generate_canvas_effects_css(const core::CanvasEffects& effects) const
    -> std::string
{
    std::ostringstream css;

    css << "/* FX Canvas Effects */\n";

    css << ".fx-canvas-object {\n"
        << "  box-shadow: 0 " << effects.object_shadow_elevation << "px "
        << effects.object_shadow_blur << "px " << css_color({0, 0, 0}, effects.object_shadow_alpha)
        << ";\n"
        << "}\n";

    css << ".fx-canvas-selected {\n"
        << "  outline: " << effects.selection_glow_width << "px solid "
        << css_color(effects.selection_glow_color) << ";\n";
    if (effects.selection_glow_pulse)
    {
        css << "  animation: fx-pulse 2s ease-in-out infinite;\n";
    }
    css << "}\n";

    if (effects.connector_neon_intensity > 0.0F)
    {
        css << ".fx-connector {\n"
            << "  filter: drop-shadow(0 0 2px rgba(100, 99, 255, "
            << effects.connector_neon_intensity << "));\n"
            << "}\n";
    }

    return css.str();
}

auto FxCssGenerator::generate_full_stylesheet(const FxPreset& preset) const -> std::string
{
    std::ostringstream css;

    css << "/* ===== MarkAmp FX Stylesheet: " << preset.name << " ===== */\n\n";
    css << generate_css_variables(preset) << "\n";
    css << generate_text_effects_css(preset.text) << "\n";
    css << generate_ui_effects_css(preset.ui_elements) << "\n";
    css << generate_editor_effects_css(preset.editor) << "\n";
    css << generate_canvas_effects_css(preset.canvas) << "\n";
    css << generate_animation_keyframes(preset.motion) << "\n";

    return css.str();
}

auto FxCssGenerator::generate_css_variables(const FxPreset& preset) const -> std::string
{
    std::ostringstream css;

    css << ":root {\n";
    css << "  /* FX Motion */\n";
    css << "  --fx-motion-intensity: " << preset.motion.motion_intensity << ";\n";
    css << "  --fx-glow-intensity: " << preset.motion.glow_intensity << ";\n";
    css << "  --fx-bloom-intensity: " << preset.motion.bloom_intensity << ";\n";
    css << "  --fx-chroma-intensity: " << preset.motion.chroma_intensity << ";\n";
    css << "  --fx-duration-scale: " << preset.motion.duration_scale << ";\n";

    css << "\n  /* FX Text */\n";
    css << "  --fx-text-stroke-width: " << preset.text.stroke_width << "px;\n";
    css << "  --fx-text-shadow-blur: " << preset.text.shadow_blur << "px;\n";
    css << "  --fx-text-glow-radius: " << preset.text.outer_glow_radius << "px;\n";

    css << "\n  /* FX UI */\n";
    css << "  --fx-hover-glow-radius: " << preset.ui_elements.hover_glow_radius << "px;\n";
    css << "  --fx-button-depth: " << preset.ui_elements.button_press_depth << "px;\n";
    css << "  --fx-panel-shadow-blur: " << preset.ui_elements.panel_shadow_blur << "px;\n";

    css << "}\n";

    return css.str();
}

auto FxCssGenerator::generate_animation_keyframes(const MotionPreset& motion) const -> std::string
{
    std::ostringstream css;

    css << "/* FX Animation Keyframes */\n";

    const float duration = 300.0F * motion.duration_scale;

    css << "@keyframes fx-glow-pulse {\n"
        << "  0%   { opacity: " << (motion.glow_intensity * 0.5F) << "; }\n"
        << "  50%  { opacity: " << motion.glow_intensity << "; }\n"
        << "  100% { opacity: " << (motion.glow_intensity * 0.5F) << "; }\n"
        << "}\n";

    css << "@keyframes fx-bloom-pulse {\n"
        << "  0%   { filter: brightness(1.0); }\n"
        << "  50%  { filter: brightness(" << (1.0F + motion.bloom_intensity) << "); }\n"
        << "  100% { filter: brightness(1.0); }\n"
        << "}\n";

    css << "@keyframes fx-pulse {\n"
        << "  0%   { transform: scale(1.0); }\n"
        << "  50%  { transform: scale(1.02); }\n"
        << "  100% { transform: scale(1.0); }\n"
        << "}\n";

    css << ".fx-animated {\n"
        << "  transition-duration: " << duration << "ms;\n"
        << "  transition-timing-function: cubic-bezier(0.4, 0.0, 0.2, 1.0);\n"
        << "}\n";

    return css.str();
}

auto FxCssGenerator::css_color(const core::Color& color, uint8_t alpha) -> std::string
{
    char buf[32];
    if (alpha == 255)
    {
        std::snprintf(buf, sizeof(buf), "rgb(%d, %d, %d)", color.r, color.g, color.b);
    }
    else
    {
        std::snprintf(buf,
                      sizeof(buf),
                      "rgba(%d, %d, %d, %.2f)",
                      color.r,
                      color.g,
                      color.b,
                      static_cast<double>(alpha) / 255.0);
    }
    return buf;
}

auto FxCssGenerator::css_text_shadow(float offset_x,
                                     float offset_y,
                                     float blur_radius,
                                     const core::Color& color,
                                     uint8_t alpha) -> std::string
{
    std::ostringstream shadow;
    shadow << offset_x << "px " << offset_y << "px " << blur_radius << "px "
           << css_color(color, alpha);
    return shadow.str();
}

auto FxCssGenerator::has_active_effects(const FxPreset& preset) -> bool
{
    // Check text effects.
    if (preset.text.stroke_enabled || preset.text.shadow_enabled ||
        preset.text.outer_glow_enabled || preset.text.inner_glow_enabled ||
        preset.text.gradient_fill_enabled || preset.text.bloom_enabled || preset.text.crt_mode)
    {
        return true;
    }

    // Check editor effects.
    if (preset.editor.caret_glow_enabled || preset.editor.selection_shimmer ||
        preset.editor.active_line_glow || preset.editor.diagnostic_pulse)
    {
        return true;
    }

    // Check canvas effects.
    if (preset.canvas.selection_glow_pulse || preset.canvas.sticky_note_lighting)
    {
        return true;
    }

    // Check motion intensity.
    if (preset.motion.motion_intensity > 0.0F || preset.motion.glow_intensity > 0.0F ||
        preset.motion.bloom_intensity > 0.0F)
    {
        return true;
    }

    return false;
}

} // namespace markamp::rendering
