#include "LayeringModel.h"

#include <algorithm>

namespace markamp::canvas
{

void LayeringModel::set_layers(std::vector<LayerEntry> layers)
{
    layers_ = std::move(layers);
    // Sort by z_order ascending
    std::sort(layers_.begin(),
              layers_.end(),
              [](const LayerEntry& lhs, const LayerEntry& rhs)
              { return lhs.z_order < rhs.z_order; });
}

auto LayeringModel::layers() const -> const std::vector<LayerEntry>&
{
    return layers_;
}

auto LayeringModel::find_index(const std::string& object_id) -> int
{
    for (int i = 0; i < static_cast<int>(layers_.size()); ++i)
    {
        if (layers_[static_cast<size_t>(i)].object_id == object_id)
        {
            return i;
        }
    }
    return -1;
}

void LayeringModel::bring_forward(const std::string& object_id)
{
    const int idx = find_index(object_id);
    if (idx >= 0 && idx < static_cast<int>(layers_.size()) - 1)
    {
        std::swap(layers_[static_cast<size_t>(idx)], layers_[static_cast<size_t>(idx + 1)]);
        // Update z_orders
        layers_[static_cast<size_t>(idx)].z_order = idx;
        layers_[static_cast<size_t>(idx + 1)].z_order = idx + 1;
    }
}

void LayeringModel::send_backward(const std::string& object_id)
{
    const int idx = find_index(object_id);
    if (idx > 0)
    {
        std::swap(layers_[static_cast<size_t>(idx)], layers_[static_cast<size_t>(idx - 1)]);
        layers_[static_cast<size_t>(idx - 1)].z_order = idx - 1;
        layers_[static_cast<size_t>(idx)].z_order = idx;
    }
}

void LayeringModel::bring_to_front(const std::string& object_id)
{
    const int idx = find_index(object_id);
    if (idx >= 0 && idx < static_cast<int>(layers_.size()) - 1)
    {
        auto entry = layers_[static_cast<size_t>(idx)];
        layers_.erase(layers_.begin() + idx);
        layers_.push_back(entry);
        for (int i = 0; i < static_cast<int>(layers_.size()); ++i)
        {
            layers_[static_cast<size_t>(i)].z_order = i;
        }
    }
}

void LayeringModel::send_to_back(const std::string& object_id)
{
    const int idx = find_index(object_id);
    if (idx > 0)
    {
        auto entry = layers_[static_cast<size_t>(idx)];
        layers_.erase(layers_.begin() + idx);
        layers_.insert(layers_.begin(), entry);
        for (int i = 0; i < static_cast<int>(layers_.size()); ++i)
        {
            layers_[static_cast<size_t>(i)].z_order = i;
        }
    }
}

void LayeringModel::set_state(const std::string& object_id, LayerState state)
{
    const int idx = find_index(object_id);
    if (idx >= 0)
    {
        layers_[static_cast<size_t>(idx)].state = state;
    }
}

auto LayeringModel::state_of(const std::string& object_id) const -> LayerState
{
    for (const auto& layer : layers_)
    {
        if (layer.object_id == object_id)
        {
            return layer.state;
        }
    }
    return LayerState::kNormal;
}

auto LayeringModel::is_locked(const std::string& object_id) const -> bool
{
    return state_of(object_id) == LayerState::kLocked;
}

auto LayeringModel::is_hidden(const std::string& object_id) const -> bool
{
    return state_of(object_id) == LayerState::kHidden;
}

auto LayeringModel::selectable(bool include_locked) const -> std::vector<LayerEntry>
{
    std::vector<LayerEntry> result;
    for (const auto& layer : layers_)
    {
        if (layer.state == LayerState::kHidden)
        {
            continue;
        }
        if (layer.state == LayerState::kLocked && !include_locked)
        {
            continue;
        }
        result.push_back(layer);
    }
    return result;
}

} // namespace markamp::canvas
