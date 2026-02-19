#include "LayoutPresetModel.h"

#include <algorithm>

namespace markamp::ui
{

void LayoutPresetModel::set_presets(std::vector<LayoutPreset> presets)
{
    presets_ = std::move(presets);
}
auto LayoutPresetModel::presets() const -> const std::vector<LayoutPreset>&
{
    return presets_;
}

void LayoutPresetModel::add_preset(LayoutPreset preset)
{
    presets_.push_back(std::move(preset));
}

void LayoutPresetModel::remove_preset(const std::string& preset_id)
{
    presets_.erase(std::remove_if(presets_.begin(),
                                  presets_.end(),
                                  [&](const LayoutPreset& preset)
                                  { return preset.preset_id == preset_id; }),
                   presets_.end());
    if (active_preset_id_ == preset_id)
    {
        active_preset_id_.clear();
    }
}

auto LayoutPresetModel::preset_by_id(const std::string& preset_id) const -> const LayoutPreset*
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

void LayoutPresetModel::set_active_preset(const std::string& preset_id)
{
    active_preset_id_ = preset_id;
}
auto LayoutPresetModel::active_preset_id() const -> const std::string&
{
    return active_preset_id_;
}

void LayoutPresetModel::set_controls(std::vector<ControlVisibility> controls)
{
    controls_ = std::move(controls);
}
auto LayoutPresetModel::controls() const -> const std::vector<ControlVisibility>&
{
    return controls_;
}

void LayoutPresetModel::toggle_control(const std::string& control_id)
{
    for (auto& ctrl : controls_)
    {
        if (ctrl.control_id == control_id)
        {
            ctrl.is_visible = !ctrl.is_visible;
            return;
        }
    }
}

auto LayoutPresetModel::is_control_visible(const std::string& control_id) const -> bool
{
    for (const auto& ctrl : controls_)
    {
        if (ctrl.control_id == control_id)
        {
            return ctrl.is_visible;
        }
    }
    return true; // default visible
}

void LayoutPresetModel::set_profile(ProfileType profile)
{
    profile_ = profile;
}
auto LayoutPresetModel::profile() const -> ProfileType
{
    return profile_;
}

auto LayoutPresetModel::default_preset_for(ProfileType profile) -> std::string
{
    switch (profile)
    {
        case ProfileType::kWriting:
            return "writing-default";
        case ProfileType::kCoding:
            return "coding-default";
        case ProfileType::kCanvas:
            return "canvas-default";
        case ProfileType::kCustom:
            return "custom";
    }
    return "default";
}

void LayoutPresetModel::reset_to_defaults()
{
    for (auto& ctrl : controls_)
    {
        ctrl.is_visible = true;
    }
    active_preset_id_.clear();
}

} // namespace markamp::ui
