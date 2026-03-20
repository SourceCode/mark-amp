/// @file IconInventory.h
/// @brief P10-T01: Icon inventory and canonical MUI mapping.
///
/// Code-backed inventory of all icon call sites grouped by semantic role.
/// Maps legacy icon names to MUI replacements.
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
};

/// Migration status of an icon.
enum class IconMigrationStatus
{
    kMapped,
    kPending,
    kCustom,
    kNotApplicable,
};

/// An icon inventory entry.
struct IconInventoryEntry
{
    std::string legacy_id;
    std::string mui_id;
    IconRole role{IconRole::kCommand};
    IconMigrationStatus status{IconMigrationStatus::kPending};
    std::string notes;
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

private:
    void register_built_in_mappings();

    std::vector<IconInventoryEntry> entries_;
    std::unordered_map<std::string, std::string> legacy_to_mui_;
};

} // namespace markamp::core
