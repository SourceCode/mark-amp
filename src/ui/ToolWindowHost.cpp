/// @file ToolWindowHost.cpp
/// @brief V8 Phase 11 (Phase 23) — Tool window host implementation.

#include "ui/ToolWindowHost.h"

#include "core/Config.h"
#include "core/EventBus.h"

#include <algorithm>

namespace markamp::ui
{

ToolWindowHost::ToolWindowHost(core::EventBus& event_bus, core::Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    toggle_sub_ = event_bus_.subscribe<core::events::ToolWindowToggleRequestEvent>(
        [this](const core::events::ToolWindowToggleRequestEvent& evt)
        { set_visible(evt.panel_id, evt.visible); });

    dock_changed_sub_ = event_bus_.subscribe<core::events::ToolWindowDockPositionChangedEvent>(
        [this](const core::events::ToolWindowDockPositionChangedEvent& evt)
        { set_dock_position(evt.panel_id, evt.dock_position); });
}

void ToolWindowHost::register_panel(const std::string& panel_id,
                                    const std::string& title,
                                    core::events::DockPosition default_position)
{
    if (panels_.contains(panel_id))
    {
        return;
    }
    ToolWindowState state;
    state.panel_id = panel_id;
    state.title = title;
    state.dock_position = default_position;
    panels_[panel_id] = std::move(state);
}

void ToolWindowHost::unregister_panel(const std::string& panel_id)
{
    panels_.erase(panel_id);
}

void ToolWindowHost::toggle_panel(const std::string& panel_id)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.visible = !iter->second.visible;
}

void ToolWindowHost::set_visible(const std::string& panel_id, bool visible)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.visible = visible;
}

void ToolWindowHost::set_dock_position(const std::string& panel_id,
                                       core::events::DockPosition position)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.dock_position = position;
}

void ToolWindowHost::set_pinned(const std::string& panel_id, bool pinned)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.pinned = pinned;
}

auto ToolWindowHost::is_visible(const std::string& panel_id) const -> bool
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return false;
    }
    return iter->second.visible;
}

auto ToolWindowHost::is_pinned(const std::string& panel_id) const -> bool
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return false;
    }
    return iter->second.pinned;
}

auto ToolWindowHost::dock_position(const std::string& panel_id) const -> core::events::DockPosition
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return core::events::DockPosition::kBottom;
    }
    return iter->second.dock_position;
}

auto ToolWindowHost::panel_state(const std::string& panel_id) const -> const ToolWindowState*
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto ToolWindowHost::panel_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    ids.reserve(panels_.size());
    for (const auto& [panel_id, state] : panels_)
    {
        ids.push_back(panel_id);
    }
    return ids;
}

auto ToolWindowHost::panels_at(core::events::DockPosition position) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [panel_id, state] : panels_)
    {
        if (state.dock_position == position)
        {
            result.push_back(panel_id);
        }
    }
    return result;
}

auto ToolWindowHost::panel_count() const -> int
{
    return static_cast<int>(panels_.size());
}

auto ToolWindowHost::save_layout() const -> std::string
{
    // TODO(Phase 11 Batch 4): implement JSON serialization
    return "{}";
}

auto ToolWindowHost::restore_layout(const std::string& /*json*/) -> bool
{
    // TODO(Phase 11 Batch 4): implement JSON deserialization
    return true;
}

} // namespace markamp::ui
