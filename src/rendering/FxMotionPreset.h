/// FxMotionPreset.h — Phase 47: Compiz/Beryl Motion Presets
///
/// Defines motion preset profiles that replicate Compiz/Beryl interaction
/// feel across the app shell. Each preset configures transition effects,
/// easing curves, and intensity levels.

#pragma once

#include "core/Theme.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::rendering
{

/// Built-in motion preset identifiers.
enum class MotionPresetId : uint8_t
{
    kCompizClassic, ///< Strong motion, moderate glow, minimal text distortion
    kBerylNeon,     ///< Strong glow/bloom/chroma, medium motion, safe text clamp
    kStudioPro,     ///< Subtle motion, subtle glass, high text clarity
    kFocusMinimal,  ///< Almost no motion, no bloom, crisp text only
    kCustom,        ///< User-defined preset
};

/// Full motion preset configuration.
struct MotionPreset
{
    std::string name;
    MotionPresetId preset_id{MotionPresetId::kCustom};

    // Transition intensities
    float motion_intensity{1.0F}; ///< 0.0–1.0 global motion scale
    float glow_intensity{0.5F};   ///< 0.0–1.0 global glow scale
    float bloom_intensity{0.3F};  ///< 0.0–1.0 global bloom scale
    float chroma_intensity{0.0F}; ///< 0.0–1.0 chromatic aberration

    // Motion params
    float stiffness{0.5F};      ///< Spring stiffness for wobbly effects
    float damping{0.7F};        ///< Spring damping
    float overshoot{0.05F};     ///< Easing overshoot
    float duration_scale{1.0F}; ///< Multiplier for all durations

    // Safety
    bool text_distortion_allowed{false}; ///< Whether text can wobble
    bool safe_text_clamp{true};          ///< Readability clamp active

    auto operator==(const MotionPreset&) const -> bool = default;
};

/// Manages built-in and custom motion presets.
class FxMotionPreset
{
public:
    explicit FxMotionPreset(core::EventBus& event_bus);

    /// Apply a built-in preset by ID. Returns the preset applied.
    auto apply(MotionPresetId preset_id) -> MotionPreset;

    /// Apply a custom preset.
    void apply_custom(const MotionPreset& preset);

    /// Reset to default (Balanced/StudioPro).
    void reset();

    /// Get the currently active preset.
    [[nodiscard]] auto current() const noexcept -> const MotionPreset&;

    /// Get a built-in preset definition.
    [[nodiscard]] static auto builtin(MotionPresetId preset_id) -> MotionPreset;

    /// List all built-in preset names.
    [[nodiscard]] static auto builtin_names() -> std::vector<std::string>;

    /// Convert preset ID to display string.
    [[nodiscard]] static auto preset_name(MotionPresetId preset_id) -> std::string_view;

private:
    core::EventBus& event_bus_;
    MotionPreset current_preset_;
};

} // namespace markamp::rendering
