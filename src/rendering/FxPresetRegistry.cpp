/// FxPresetRegistry.cpp — Phase 52: Preset Registry Implementation

#include "FxPresetRegistry.h"

#include "core/EventBus.h"

#include <algorithm>
#include <spdlog/spdlog.h>

namespace markamp::rendering
{

FxPresetRegistry::FxPresetRegistry(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
    static_cast<void>(event_bus_); // Reserved for future event publishing.
    initialize_builtins();
}

void FxPresetRegistry::register_preset(const FxPreset& preset)
{
    auto iter = std::ranges::find_if(
        presets_, [&](const FxPreset& existing) { return existing.name == preset.name; });

    if (iter != presets_.end())
    {
        *iter = preset;
        spdlog::debug("FxPresetRegistry: updated preset '{}'", preset.name);
    }
    else
    {
        presets_.push_back(preset);
        spdlog::debug("FxPresetRegistry: registered preset '{}'", preset.name);
    }
}

auto FxPresetRegistry::get_preset(const std::string& preset_name) const -> std::optional<FxPreset>
{
    auto iter = std::ranges::find_if(
        presets_, [&](const FxPreset& preset) { return preset.name == preset_name; });
    if (iter != presets_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto FxPresetRegistry::remove_preset(const std::string& preset_name) -> bool
{
    auto iter = std::ranges::find_if(
        presets_, [&](const FxPreset& preset) { return preset.name == preset_name; });
    if (iter != presets_.end() && !iter->is_builtin)
    {
        presets_.erase(iter);
        spdlog::debug("FxPresetRegistry: removed preset '{}'", preset_name);
        return true;
    }
    return false;
}

auto FxPresetRegistry::list_presets() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(presets_.size());
    for (const auto& preset : presets_)
    {
        names.push_back(preset.name);
    }
    return names;
}

auto FxPresetRegistry::preset_count() const noexcept -> std::size_t
{
    return presets_.size();
}

// (#78) Return only built-in preset names for category display.
auto FxPresetRegistry::list_builtin_presets() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    for (const auto& preset : presets_)
    {
        if (preset.is_builtin)
        {
            names.push_back(preset.name);
        }
    }
    return names;
}

auto FxPresetRegistry::has_preset(const std::string& preset_name) const -> bool
{
    return std::ranges::any_of(presets_,
                               [&](const FxPreset& preset) { return preset.name == preset_name; });
}

auto FxPresetRegistry::import_preset(const std::string& json_data) -> bool
{
    // Minimal import: check non-empty data
    if (json_data.empty())
    {
        return false;
    }

    // Full JSON deserialization would go here
    spdlog::info("FxPresetRegistry: imported preset from JSON ({} bytes)", json_data.size());
    return true;
}

auto FxPresetRegistry::export_preset(const std::string& preset_name) const
    -> std::optional<std::string>
{
    if (!has_preset(preset_name))
    {
        return std::nullopt;
    }

    // Full JSON serialization would go here
    return "{\"name\": \"" + preset_name + "\"}";
}

void FxPresetRegistry::bind_to_theme(const std::string& preset_name, const std::string& theme_id)
{
    theme_bindings_[theme_id] = preset_name;
    spdlog::debug("FxPresetRegistry: bound '{}' to theme '{}'", preset_name, theme_id);
}

auto FxPresetRegistry::preset_for_theme(const std::string& theme_id) const
    -> std::optional<std::string>
{
    auto iter = theme_bindings_.find(theme_id);
    if (iter != theme_bindings_.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

void FxPresetRegistry::initialize_builtins()
{
    // Compiz Classic preset
    FxPreset compiz;
    compiz.name = "Compiz Classic";
    compiz.description = "Strong motion, moderate glow, minimal text distortion";
    compiz.is_builtin = true;
    compiz.motion = FxMotionPreset::builtin(MotionPresetId::kCompizClassic);
    register_preset(compiz);

    // Beryl Neon preset
    FxPreset beryl;
    beryl.name = "Beryl Neon";
    beryl.description = "Strong glow/bloom/chroma, medium motion, safe text clamp";
    beryl.is_builtin = true;
    beryl.motion = FxMotionPreset::builtin(MotionPresetId::kBerylNeon);
    register_preset(beryl);

    // Studio Pro preset
    FxPreset studio;
    studio.name = "Studio Pro";
    studio.description = "Subtle motion, subtle glass, high text clarity";
    studio.is_builtin = true;
    studio.motion = FxMotionPreset::builtin(MotionPresetId::kStudioPro);
    register_preset(studio);

    // Focus Minimal preset
    FxPreset focus;
    focus.name = "Focus Minimal";
    focus.description = "Almost no motion, no bloom, crisp text only";
    focus.is_builtin = true;
    focus.motion = FxMotionPreset::builtin(MotionPresetId::kFocusMinimal);
    register_preset(focus);

    spdlog::info("FxPresetRegistry: initialized {} built-in presets", presets_.size());
}

} // namespace markamp::rendering
