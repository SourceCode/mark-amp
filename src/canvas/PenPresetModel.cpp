// ============================================================================
// File: src/canvas/PenPresetModel.cpp
// Phase 03 W02: Pen Preset model implementation
// ============================================================================

#include "canvas/PenPresetModel.h"

#include <algorithm>
#include <stdexcept>

namespace markamp::canvas
{

void PenPresetModel::add_preset(const PenPreset& preset)
{
    presets_.push_back(preset);
}

auto PenPresetModel::preset_count() const noexcept -> int
{
    return static_cast<int>(presets_.size());
}

auto PenPresetModel::preset_at(int index) const -> const PenPreset&
{
    return presets_.at(static_cast<size_t>(index));
}

void PenPresetModel::set_active_preset(const std::string& name)
{
    active_preset_name_ = name;
}

auto PenPresetModel::active_preset_name() const -> const std::string&
{
    return active_preset_name_;
}

void PenPresetModel::push_recent(const std::string& preset_name)
{
    // Remove duplicate if present
    auto iter = std::find(recent_.begin(), recent_.end(), preset_name);
    if (iter != recent_.end())
    {
        recent_.erase(iter);
    }

    recent_.insert(recent_.begin(), preset_name);

    // Trim to max
    if (static_cast<int>(recent_.size()) > max_recent_)
    {
        recent_.resize(static_cast<size_t>(max_recent_));
    }
}

auto PenPresetModel::recent_count() const noexcept -> int
{
    return static_cast<int>(recent_.size());
}

void PenPresetModel::set_max_recent(int count)
{
    max_recent_ = count;
}

auto PenPresetModel::max_recent() const noexcept -> int
{
    return max_recent_;
}

} // namespace markamp::canvas
