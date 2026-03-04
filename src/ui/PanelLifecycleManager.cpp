#include "PanelLifecycleManager.h"

#include "core/Events.h"

#include <algorithm>

namespace markamp::ui
{

// ── PanelSnapshot ───────────────────────────────────────────────────

auto PanelSnapshot::visible_count() const -> int
{
    return static_cast<int>(std::count_if(entries.begin(),
                                          entries.end(),
                                          [](const PanelLayoutEntry& entry)
                                          { return entry.visible; }));
}

auto PanelSnapshot::count_at(core::events::DockPosition position) const -> int
{
    return static_cast<int>(std::count_if(entries.begin(),
                                          entries.end(),
                                          [position](const PanelLayoutEntry& entry)
                                          { return entry.dock_position == position; }));
}

auto PanelSnapshot::is_empty() const -> bool
{
    return entries.empty();
}

// ── PanelLifecycleManager ───────────────────────────────────────────

void PanelLifecycleManager::save_snapshot(const std::string& name,
                                          const std::vector<PanelLayoutEntry>& entries)
{
    // Overwrite existing snapshot with same name
    for (auto& snap : snapshots_)
    {
        if (snap.snapshot_name == name)
        {
            snap.entries = entries;
            return;
        }
    }

    PanelSnapshot snapshot;
    snapshot.snapshot_name = name;
    snapshot.entries = entries;
    snapshots_.push_back(std::move(snapshot));
}

auto PanelLifecycleManager::get_snapshot(const std::string& name) const -> const PanelSnapshot*
{
    for (const auto& snap : snapshots_)
    {
        if (snap.snapshot_name == name)
        {
            return &snap;
        }
    }
    return nullptr;
}

auto PanelLifecycleManager::has_snapshot(const std::string& name) const -> bool
{
    return get_snapshot(name) != nullptr;
}

void PanelLifecycleManager::delete_snapshot(const std::string& name)
{
    snapshots_.erase(std::remove_if(snapshots_.begin(),
                                    snapshots_.end(),
                                    [&name](const PanelSnapshot& snap)
                                    { return snap.snapshot_name == name; }),
                     snapshots_.end());
}

auto PanelLifecycleManager::snapshot_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(snapshots_.size());
    for (const auto& snap : snapshots_)
    {
        names.push_back(snap.snapshot_name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

auto PanelLifecycleManager::snapshot_count() const -> int
{
    return static_cast<int>(snapshots_.size());
}

void PanelLifecycleManager::set_default_layout(const std::vector<PanelLayoutEntry>& entries)
{
    default_layout_.snapshot_name = "__default__";
    default_layout_.entries = entries;
    has_default_ = true;
}

auto PanelLifecycleManager::default_layout() const -> const PanelSnapshot*
{
    if (has_default_)
    {
        return &default_layout_;
    }
    return nullptr;
}

void PanelLifecycleManager::clear()
{
    snapshots_.clear();
    default_layout_ = {};
    has_default_ = false;
}

auto PanelLifecycleManager::builtin_defaults() -> std::vector<PanelLayoutEntry>
{
    std::vector<PanelLayoutEntry> defaults;

    // Standard IDE layout: 7 panels in bottom/left positions
    defaults.push_back(
        {"explorer", "Explorer", core::events::DockPosition::kLeft, true, true, 260, 600, 0});
    defaults.push_back(
        {"search", "Search", core::events::DockPosition::kLeft, false, false, 260, 600, 1});
    defaults.push_back(
        {"output", "Output", core::events::DockPosition::kBottom, false, false, 800, 200, 0});
    defaults.push_back(
        {"problems", "Problems", core::events::DockPosition::kBottom, false, false, 800, 200, 1});
    defaults.push_back(
        {"terminal", "Terminal", core::events::DockPosition::kBottom, true, true, 800, 250, 2});
    defaults.push_back(
        {"build", "Build", core::events::DockPosition::kBottom, false, false, 800, 200, 3});
    defaults.push_back({"debug_console",
                        "Debug Console",
                        core::events::DockPosition::kBottom,
                        false,
                        false,
                        800,
                        200,
                        4});

    return defaults;
}

} // namespace markamp::ui
