/// FxMotionPreset.cpp — Phase 47: Motion Preset Implementation

#include "FxMotionPreset.h"

#include "core/EventBus.h"

#include <spdlog/spdlog.h>

namespace markamp::rendering
{

FxMotionPreset::FxMotionPreset(core::EventBus& event_bus)
    : event_bus_(event_bus)
    , current_preset_(builtin(MotionPresetId::kStudioPro))
{
    static_cast<void>(event_bus_); // Reserved for future event publishing.
}

auto FxMotionPreset::apply(MotionPresetId preset_id) -> MotionPreset
{
    current_preset_ = builtin(preset_id);
    spdlog::info("FxMotionPreset: applied '{}'", preset_name(preset_id));
    return current_preset_;
}

void FxMotionPreset::apply_custom(const MotionPreset& preset)
{
    current_preset_ = preset;
    current_preset_.preset_id = MotionPresetId::kCustom;
    spdlog::info("FxMotionPreset: applied custom preset '{}'", preset.name);
}

void FxMotionPreset::reset()
{
    current_preset_ = builtin(MotionPresetId::kStudioPro);
    spdlog::info("FxMotionPreset: reset to Studio Pro");
}

auto FxMotionPreset::current() const noexcept -> const MotionPreset&
{
    return current_preset_;
}

auto FxMotionPreset::builtin(MotionPresetId preset_id) -> MotionPreset
{
    MotionPreset preset;

    switch (preset_id)
    {
        case MotionPresetId::kCompizClassic:
            preset.name = "Compiz Classic";
            preset.preset_id = MotionPresetId::kCompizClassic;
            preset.motion_intensity = 0.9F;
            preset.glow_intensity = 0.5F;
            preset.bloom_intensity = 0.2F;
            preset.chroma_intensity = 0.0F;
            preset.stiffness = 0.4F;
            preset.damping = 0.6F;
            preset.overshoot = 0.1F;
            preset.duration_scale = 1.0F;
            preset.text_distortion_allowed = false;
            preset.safe_text_clamp = true;
            break;

        case MotionPresetId::kBerylNeon:
            preset.name = "Beryl Neon";
            preset.preset_id = MotionPresetId::kBerylNeon;
            preset.motion_intensity = 0.7F;
            preset.glow_intensity = 0.9F;
            preset.bloom_intensity = 0.6F;
            preset.chroma_intensity = 0.3F;
            preset.stiffness = 0.5F;
            preset.damping = 0.7F;
            preset.overshoot = 0.08F;
            preset.duration_scale = 1.1F;
            preset.text_distortion_allowed = false;
            preset.safe_text_clamp = true;
            break;

        case MotionPresetId::kStudioPro:
            preset.name = "Studio Pro";
            preset.preset_id = MotionPresetId::kStudioPro;
            preset.motion_intensity = 0.3F;
            preset.glow_intensity = 0.2F;
            preset.bloom_intensity = 0.1F;
            preset.chroma_intensity = 0.0F;
            preset.stiffness = 0.7F;
            preset.damping = 0.85F;
            preset.overshoot = 0.02F;
            preset.duration_scale = 0.8F;
            preset.text_distortion_allowed = false;
            preset.safe_text_clamp = true;
            break;

        case MotionPresetId::kFocusMinimal:
            preset.name = "Focus Minimal";
            preset.preset_id = MotionPresetId::kFocusMinimal;
            preset.motion_intensity = 0.0F;
            preset.glow_intensity = 0.0F;
            preset.bloom_intensity = 0.0F;
            preset.chroma_intensity = 0.0F;
            preset.stiffness = 1.0F;
            preset.damping = 1.0F;
            preset.overshoot = 0.0F;
            preset.duration_scale = 0.5F;
            preset.text_distortion_allowed = false;
            preset.safe_text_clamp = true;
            break;

        case MotionPresetId::kCustom:
            preset.name = "Custom";
            preset.preset_id = MotionPresetId::kCustom;
            break;
    }

    return preset;
}

auto FxMotionPreset::builtin_names() -> std::vector<std::string>
{
    return {"Compiz Classic", "Beryl Neon", "Studio Pro", "Focus Minimal"};
}

auto FxMotionPreset::preset_name(MotionPresetId preset_id) -> std::string_view
{
    switch (preset_id)
    {
        case MotionPresetId::kCompizClassic:
            return "Compiz Classic";
        case MotionPresetId::kBerylNeon:
            return "Beryl Neon";
        case MotionPresetId::kStudioPro:
            return "Studio Pro";
        case MotionPresetId::kFocusMinimal:
            return "Focus Minimal";
        case MotionPresetId::kCustom:
            return "Custom";
    }
    return "Unknown";
}

} // namespace markamp::rendering
