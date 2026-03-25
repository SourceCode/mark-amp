#include "LayoutPresetController.h"

#include <algorithm>

namespace markamp::ui
{

auto LayoutPreset::profile_name() const -> std::string
{
    switch (profile)
    {
        case ProfileType::kCoding:
            return "coding";
        case ProfileType::kWriting:
            return "writing";
        case ProfileType::kCustom:
            return "custom";
    }
    return "unknown";
}

void LayoutPresetController::create_preset(const LayoutPreset& preset)
{
    // Replace if exists
    delete_preset(preset.preset_id);
    presets_.push_back(preset);
}

void LayoutPresetController::rename_preset(const std::string& preset_id,
                                           const std::string& new_name)
{
    for (auto& preset : presets_)
    {
        if (preset.preset_id == preset_id)
        {
            preset.name = new_name;
            return;
        }
    }
}

void LayoutPresetController::delete_preset(const std::string& preset_id)
{
    presets_.erase(std::remove_if(presets_.begin(),
                                  presets_.end(),
                                  [&preset_id](const LayoutPreset& preset)
                                  { return preset.preset_id == preset_id; }),
                   presets_.end());
}

auto LayoutPresetController::find_preset(const std::string& preset_id) const -> const LayoutPreset*
{
    for (const auto& preset : presets_)
    {
        if (preset.preset_id == preset_id)
        {
            return &preset;
        }
    }
    return nullptr;
}

auto LayoutPresetController::all_presets() const -> const std::vector<LayoutPreset>&
{
    return presets_;
}

auto LayoutPresetController::presets_for_profile(ProfileType profile) const
    -> std::vector<LayoutPreset>
{
    std::vector<LayoutPreset> result;
    for (const auto& preset : presets_)
    {
        if (preset.profile == profile)
        {
            result.push_back(preset);
        }
    }
    return result;
}

auto LayoutPresetController::preset_count() const -> int
{
    return static_cast<int>(presets_.size());
}

void LayoutPresetController::apply_preset(const std::string& preset_id)
{
    if (find_preset(preset_id) != nullptr)
    {
        active_preset_id_ = preset_id;
    }
}

auto LayoutPresetController::active_preset_id() const -> const std::string&
{
    return active_preset_id_;
}

void LayoutPresetController::reset_to_default()
{
    const auto* def = default_for_profile(current_profile_);
    if (def != nullptr)
    {
        active_preset_id_ = def->preset_id;
    }
}

auto LayoutPresetController::default_for_profile(ProfileType profile) const -> const LayoutPreset*
{
    for (const auto& preset : presets_)
    {
        if (preset.profile == profile && preset.is_default)
        {
            return &preset;
        }
    }
    return nullptr;
}

void LayoutPresetController::set_profile(ProfileType profile)
{
    current_profile_ = profile;
}

auto LayoutPresetController::current_profile() const -> ProfileType
{
    return current_profile_;
}

void LayoutPresetController::register_standard_presets()
{
    create_preset({"coding_default",
                   "Default Coding",
                   ProfileType::kCoding,
                   true,
                   {"explorer", "editor", "terminal", "output"},
                   {"explorer", "editor", "terminal", "output"}});

    create_preset({"writing_default",
                   "Default Writing",
                   ProfileType::kWriting,
                   true,
                   {"outline", "editor", "preview"},
                   {"outline", "editor", "preview"}});
}

} // namespace markamp::ui
