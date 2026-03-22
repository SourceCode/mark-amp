/// @file PaletteManifestSync.cpp
/// @brief V21 Phase 02 — PaletteManifestSync implementation.

#include "PaletteManifestSync.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

// ── Synchronization ──

auto PaletteManifestSync::sync_from_manifest(const ControlActionManifest& manifest,
                                              bool include_all) const
    -> std::vector<PaletteEntry>
{
    std::vector<PaletteEntry> entries;

    auto actions = include_all
                       ? manifest.all_actions()
                       : manifest.actions_for_surface(ControlSurface::kCommandPalette);

    entries.reserve(actions.size());

    for (const auto* action : actions)
    {
        PaletteEntry entry;
        entry.action_id = action->action_id;
        entry.label = action->label;
        entry.category = action->category;
        entry.shortcut = action->shortcut_hint;
        entry.description = action->description;
        entry.source = "builtin";
        entry.is_enabled = true;
        entry.is_visible = true;
        entry.has_handler = action->has_handler();

        entries.push_back(std::move(entry));
    }

    return entries;
}

auto PaletteManifestSync::sync_context_aware(const ControlActionManifest& manifest,
                                              const ContextKeyService& context) const
    -> std::vector<PaletteEntry>
{
    std::vector<PaletteEntry> entries;

    for (const auto* action : manifest.all_actions())
    {
        // Skip actions that aren't visible in the current context
        if (!action->is_visible(context))
        {
            continue;
        }

        PaletteEntry entry;
        entry.action_id = action->action_id;
        entry.label = action->label;
        entry.category = action->category;
        entry.shortcut = action->shortcut_hint;
        entry.description = action->description;
        entry.source = "builtin";
        entry.is_enabled = action->is_enabled(context);
        entry.is_visible = true;
        entry.has_handler = action->has_handler();

        entries.push_back(std::move(entry));
    }

    return entries;
}

// ── Shortcut Consistency ──

auto PaletteManifestSync::check_shortcut_consistency(
    const ControlActionManifest& manifest,
    const std::vector<PaletteEntry>& palette_entries) const
    -> std::vector<ShortcutDiscrepancy>
{
    std::vector<ShortcutDiscrepancy> discrepancies;

    for (const auto& palette_entry : palette_entries)
    {
        const auto* action = manifest.get_action(palette_entry.action_id);
        if (action == nullptr)
        {
            continue;
        }

        // Compare shortcut_hint from manifest with what palette shows
        if (!action->shortcut_hint.empty() && !palette_entry.shortcut.empty() &&
            action->shortcut_hint != palette_entry.shortcut)
        {
            ShortcutDiscrepancy disc;
            disc.action_id = palette_entry.action_id;
            disc.manifest_shortcut = action->shortcut_hint;
            disc.palette_shortcut = palette_entry.shortcut;
            discrepancies.push_back(std::move(disc));
        }
    }

    return discrepancies;
}

// ── Menu-to-Palette Parity ──

auto PaletteManifestSync::check_menu_palette_parity(
    const ControlActionManifest& manifest) const
    -> ParityReport
{
    ParityReport report;

    std::set<std::string> menu_ids;
    std::set<std::string> palette_ids;

    for (const auto* action : manifest.all_actions())
    {
        bool on_menu = action->appears_on(ControlSurface::kMenu);
        bool on_palette = action->appears_on(ControlSurface::kCommandPalette);

        if (on_menu)
        {
            menu_ids.insert(action->action_id);
        }
        if (on_palette)
        {
            palette_ids.insert(action->action_id);
        }
    }

    report.total_menu = static_cast<int>(menu_ids.size());
    report.total_palette = static_cast<int>(palette_ids.size());

    // Find overlaps and differences
    for (const auto& id : menu_ids)
    {
        if (palette_ids.contains(id))
        {
            report.both.push_back(id);
        }
        else
        {
            report.menu_only.push_back(id);
        }
    }

    for (const auto& id : palette_ids)
    {
        if (!menu_ids.contains(id))
        {
            report.palette_only.push_back(id);
        }
    }

    // Sort for deterministic output
    std::sort(report.menu_only.begin(), report.menu_only.end());
    std::sort(report.palette_only.begin(), report.palette_only.end());
    std::sort(report.both.begin(), report.both.end());

    // Calculate parity percentage
    int total_unique =
        static_cast<int>(menu_ids.size() + palette_ids.size() - report.both.size());
    if (total_unique > 0)
    {
        report.parity_pct = (static_cast<int>(report.both.size()) * 100) / total_unique;
    }
    else
    {
        report.parity_pct = 100;
    }

    return report;
}

// ── Queries ──

auto PaletteManifestSync::palette_eligible_count(
    const ControlActionManifest& manifest) const -> std::size_t
{
    return manifest.actions_for_surface(ControlSurface::kCommandPalette).size();
}

auto PaletteManifestSync::menu_eligible_count(
    const ControlActionManifest& manifest) const -> std::size_t
{
    return manifest.actions_for_surface(ControlSurface::kMenu).size();
}

auto PaletteManifestSync::menu_without_palette(
    const ControlActionManifest& manifest) const -> std::vector<std::string>
{
    auto report = check_menu_palette_parity(manifest);
    return report.menu_only;
}

auto PaletteManifestSync::palette_without_menu(
    const ControlActionManifest& manifest) const -> std::vector<std::string>
{
    auto report = check_menu_palette_parity(manifest);
    return report.palette_only;
}

} // namespace markamp::core
