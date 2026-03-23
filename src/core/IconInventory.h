/// @file IconInventory.h
/// @brief P10-T01 + V27-P01-T01: Icon inventory and canonical MUI mapping.
///
/// Code-backed inventory of all icon call sites grouped by semantic role.
/// Maps legacy icon names to MUI replacements. V27 adds source tracking
/// and per-surface migration progress.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Semantic role for an icon.
enum class IconRole
{
    kActivityBar,
    kToolbar,
    kFileTree,
    kPanelHeader,
    kStatusBar,
    kContextMenu,
    kDialog,
    kEmptyState,
    kCommand,
    kTab,
    // V27 additions
    kBreadcrumb,       ///< V27: breadcrumb path segments
    kCommandPalette,   ///< V27: command palette result icons
    kSettingsCategory, ///< V27: settings category sidebar
    kNotification,     ///< V27: notification/toast severity icons
    kNotebook,         ///< V27: notebook cell and toolbar icons
    kCanvas,           ///< V27: canvas tool palette and object icons
    kStartup,          ///< V27: startup/welcome surface icons
    kThemeGallery,     ///< V27: theme gallery card icons
};

/// Total number of icon roles.
[[nodiscard]] constexpr auto icon_role_count() noexcept -> int { return 18; }

/// Migration status of an icon.
enum class IconMigrationStatus
{
    kMapped,
    kPending,
    kCustom,
    kNotApplicable,
};

/// V27: Icon source system classification.
enum class IconInventorySource
{
    kMui,        ///< Already using MUI canonical path
    kLegacyChar, ///< Legacy icon-char constant
    kEmoji,      ///< Emoji/unicode used as UI chrome
    kInlineSvg,  ///< Inline SVG in source
    kLucide,     ///< Lucide icon file
    kManifest,   ///< Icon manifest system
};

/// An icon inventory entry.
struct IconInventoryEntry
{
    std::string legacy_id;
    std::string mui_id;
    IconRole role{IconRole::kCommand};
    IconMigrationStatus status{IconMigrationStatus::kPending};
    std::string notes;
    // V27 additions
    IconInventorySource source{IconInventorySource::kLegacyChar}; ///< V27: origin system
    std::string surface_name;  ///< V27: UI surface where this icon appears
};

/// Manages the icon inventory and legacy-to-MUI mapping.
class IconInventory
{
public:
    IconInventory();

    /// Register a legacy-to-MUI mapping.
    void register_mapping(const IconInventoryEntry& entry);

    /// Query MUI ID for a legacy icon.
    [[nodiscard]] auto mui_for(const std::string& legacy_id) const -> std::string;

    /// Check if a legacy icon has been mapped.
    [[nodiscard]] auto is_mapped(const std::string& legacy_id) const -> bool;

    /// Get all entries.
    [[nodiscard]] auto entries() const -> const std::vector<IconInventoryEntry>&
    {
        return entries_;
    }

    /// Get count of mapped icons.
    [[nodiscard]] auto mapped_count() const -> int;

    /// Get count of pending icons.
    [[nodiscard]] auto pending_count() const -> int;

    /// Get total entry count.
    [[nodiscard]] auto total_count() const -> int
    {
        return static_cast<int>(entries_.size());
    }

    // ── V27 additions ─────────────────────────────────────────────────────

    /// Register the full V27 canonical MUI replacement set.
    void register_v27_canonical_mappings();

    /// V27 migration progress percentage (0–100).
    [[nodiscard]] auto v27_migration_progress() const noexcept -> int;

    /// Count entries by source type.
    [[nodiscard]] auto count_by_source(IconInventorySource source) const -> int;

    /// Count entries by role.
    [[nodiscard]] auto count_by_role(IconRole role) const -> int;

private:
    void register_built_in_mappings();

    std::vector<IconInventoryEntry> entries_;
    std::unordered_map<std::string, std::string> legacy_to_mui_;
};

} // namespace markamp::core
