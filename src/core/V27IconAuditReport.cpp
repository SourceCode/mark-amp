/// @file V27IconAuditReport.cpp
/// @brief V27 Phase 01 — Icon audit report implementation.
#include "core/V27IconAuditReport.h"

#include <algorithm>

namespace markamp::core
{

void V27IconAuditReport::record(const IconAuditEntry& entry)
{
    entries_.push_back(entry);
}

auto V27IconAuditReport::count_by_source(IconSource source) const -> int
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(),
        [source](const IconAuditEntry& e) { return e.source == source; }));
}

auto V27IconAuditReport::count_by_surface(IconSurface surface) const -> int
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(),
        [surface](const IconAuditEntry& e) { return e.surface == surface; }));
}

auto V27IconAuditReport::migrated_count() const -> int
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(),
        [](const IconAuditEntry& e) { return e.is_migrated; }));
}

auto V27IconAuditReport::legacy_count() const -> int
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(),
        [](const IconAuditEntry& e) { return !e.is_migrated && e.source != IconSource::kMui; }));
}

auto V27IconAuditReport::surface_summary(IconSurface surface) const -> SurfaceMigrationSummary
{
    SurfaceMigrationSummary result{};
    result.surface = surface;
    for (const auto& e : entries_)
    {
        if (e.surface != surface) continue;
        ++result.total_icons;
        if (e.is_migrated)
            ++result.migrated_icons;
        else if (e.source == IconSource::kEmoji)
            ++result.emoji_remaining;
        else if (e.source != IconSource::kMui)
            ++result.legacy_remaining;
    }
    return result;
}

auto V27IconAuditReport::surfaces_with_legacy() const -> std::vector<IconSurface>
{
    std::vector<IconSurface> result;
    // Check all surfaces
    constexpr IconSurface all_surfaces[] = {
        IconSurface::kActivityBar, IconSurface::kToolbar, IconSurface::kTabBar,
        IconSurface::kBreadcrumb, IconSurface::kCommandPalette, IconSurface::kContextMenu,
        IconSurface::kFileTree, IconSurface::kPanelHeader, IconSurface::kStatusBar,
        IconSurface::kSettingsCategory, IconSurface::kSettingsRow, IconSurface::kDialog,
        IconSurface::kNotification, IconSurface::kEmptyState, IconSurface::kNotebook,
        IconSurface::kCanvas, IconSurface::kThemeGallery, IconSurface::kStartup,
    };
    for (auto surf : all_surfaces)
    {
        auto summary = surface_summary(surf);
        if (summary.legacy_remaining > 0 || summary.emoji_remaining > 0)
            result.push_back(surf);
    }
    return result;
}

void V27IconAuditReport::populate_v27_baseline()
{
    // Baseline audit: register known emoji icons from IconProvider.h
    auto add_emoji = [this](const std::string& id,
                            IconSurface surface, const std::string& mui) {
        record({id, IconSource::kEmoji, surface, "src/ui/IconProvider.h", mui, false});
    };

    add_emoji("file_icon",      IconSurface::kCommandPalette, "mui-file-text");
    add_emoji("edit_icon",      IconSurface::kCommandPalette, "mui-pencil");
    add_emoji("view_icon",      IconSurface::kCommandPalette, "mui-eye");
    add_emoji("navigation_icon",IconSurface::kCommandPalette, "mui-compass");
    add_emoji("terminal_icon",  IconSurface::kCommandPalette, "mui-terminal");
    add_emoji("extension_icon", IconSurface::kCommandPalette, "mui-puzzle-piece");
    add_emoji("editor_icon",    IconSurface::kCommandPalette, "mui-edit-3");
    add_emoji("search_icon",    IconSurface::kCommandPalette, "mui-search");
    add_emoji("debug_icon",     IconSurface::kCommandPalette, "mui-bug");
    add_emoji("settings_icon",  IconSurface::kSettingsCategory, "mui-settings");
    add_emoji("default_icon",   IconSurface::kCommandPalette, "mui-circle-dot");
}

} // namespace markamp::core
