/// @file V27IconMigrationTracker.cpp
/// @brief V27 Phase 03 T02 — Migration tracker implementation.
#include "core/V27IconMigrationTracker.h"

#include <algorithm>
#include <numeric>

namespace markamp::core
{

void V27IconMigrationTracker::register_surface(const V27SurfaceMigrationStatus& status)
{
    auto it = surface_index_.find(status.surface_name);
    if (it != surface_index_.end())
    {
        surfaces_[static_cast<size_t>(it->second)] = status;
    }
    else
    {
        surface_index_[status.surface_name] = static_cast<int>(surfaces_.size());
        surfaces_.push_back(status);
    }
}

void V27IconMigrationTracker::update_surface(const std::string& surface_name, int newly_migrated)
{
    auto it = surface_index_.find(surface_name);
    if (it == surface_index_.end()) return;
    auto& s = surfaces_[static_cast<size_t>(it->second)];
    s.migrated += newly_migrated;
    // Reduce from whichever bucket has remaining
    int reduce = newly_migrated;
    if (s.remaining_emoji > 0)
    {
        int from_emoji = std::min(reduce, s.remaining_emoji);
        s.remaining_emoji -= from_emoji;
        reduce -= from_emoji;
    }
    if (reduce > 0 && s.remaining_legacy > 0)
    {
        s.remaining_legacy -= std::min(reduce, s.remaining_legacy);
    }
}

auto V27IconMigrationTracker::surface_status(const std::string& surface_name) const
    -> const V27SurfaceMigrationStatus*
{
    auto it = surface_index_.find(surface_name);
    if (it == surface_index_.end()) return nullptr;
    return &surfaces_[static_cast<size_t>(it->second)];
}

auto V27IconMigrationTracker::overall_progress() const noexcept -> int
{
    int total = total_icons();
    return total > 0 ? (total_migrated() * 100) / total : 100;
}

auto V27IconMigrationTracker::is_migration_complete() const noexcept -> bool
{
    return std::all_of(surfaces_.begin(), surfaces_.end(),
        [](const V27SurfaceMigrationStatus& s) { return s.is_complete(); });
}

auto V27IconMigrationTracker::total_icons() const noexcept -> int
{
    return std::accumulate(surfaces_.begin(), surfaces_.end(), 0,
        [](int acc, const V27SurfaceMigrationStatus& s) { return acc + s.total_icons; });
}

auto V27IconMigrationTracker::total_migrated() const noexcept -> int
{
    return std::accumulate(surfaces_.begin(), surfaces_.end(), 0,
        [](int acc, const V27SurfaceMigrationStatus& s) { return acc + s.migrated; });
}

auto V27IconMigrationTracker::total_remaining_emoji() const noexcept -> int
{
    return std::accumulate(surfaces_.begin(), surfaces_.end(), 0,
        [](int acc, const V27SurfaceMigrationStatus& s) { return acc + s.remaining_emoji; });
}

auto V27IconMigrationTracker::total_remaining_legacy() const noexcept -> int
{
    return std::accumulate(surfaces_.begin(), surfaces_.end(), 0,
        [](int acc, const V27SurfaceMigrationStatus& s) { return acc + s.remaining_legacy; });
}

auto V27IconMigrationTracker::incomplete_surfaces() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& s : surfaces_)
    {
        if (!s.is_complete())
            result.push_back(s.surface_name);
    }
    return result;
}

void V27IconMigrationTracker::populate_v27_baseline()
{
    // Register known surfaces with baseline icon counts
    register_surface({"CommandPalette", 11, 0, 11, 0});
    register_surface({"ActivityBar",    8,  0, 0,  8});
    register_surface({"Toolbar",        12, 0, 0,  12});
    register_surface({"TabBar",         4,  0, 0,  4});
    register_surface({"FileTree",       6,  0, 0,  6});
    register_surface({"PanelHeader",    8,  0, 0,  8});
    register_surface({"StatusBar",      6,  0, 0,  6});
    register_surface({"ContextMenu",    10, 0, 0,  10});
    register_surface({"SettingsCategory",5, 0, 1,  4});
    register_surface({"Dialog",         4,  0, 0,  4});
    register_surface({"Notification",   4,  0, 0,  4});
    register_surface({"EmptyState",     3,  0, 0,  3});
    register_surface({"Breadcrumb",     3,  0, 0,  3});
    register_surface({"Notebook",       6,  0, 0,  6});
    register_surface({"Canvas",         8,  0, 0,  8});
    register_surface({"ThemeGallery",   2,  0, 0,  2});
    register_surface({"Startup",        4,  0, 0,  4});
}

} // namespace markamp::core
