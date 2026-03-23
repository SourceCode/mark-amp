/// @file V27IconMigrationTracker.h
/// @brief V27 Phase 03 T02 — Migration progress tracking across all surfaces.
///
/// Tracks icon migration progress per-surface, reports completion percentages,
/// and integrates with IconValidationGate to enforce migration completeness.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Migration status for a single surface.
struct V27SurfaceMigrationStatus
{
    std::string surface_name;
    int total_icons{0};        ///< Total icons on this surface
    int migrated{0};           ///< Icons migrated to MUI canonical path
    int remaining_emoji{0};    ///< Emoji icons not yet replaced
    int remaining_legacy{0};   ///< Other legacy icons not yet replaced

    /// Completion percentage (0-100).
    [[nodiscard]] auto percent_complete() const noexcept -> int
    {
        return total_icons > 0 ? (migrated * 100) / total_icons : 100;
    }

    /// True if all icons on this surface have been migrated.
    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return remaining_emoji == 0 && remaining_legacy == 0;
    }
};

/// Tracks V27 icon migration progress across the entire product.
class V27IconMigrationTracker
{
public:
    V27IconMigrationTracker() = default;

    /// Register a surface with its icon counts.
    void register_surface(const V27SurfaceMigrationStatus& status);

    /// Update migration progress for a surface.
    void update_surface(const std::string& surface_name, int newly_migrated);

    /// Get status for a specific surface.
    [[nodiscard]] auto surface_status(const std::string& surface_name) const
        -> const V27SurfaceMigrationStatus*;

    /// Get all surface statuses.
    [[nodiscard]] auto all_surfaces() const -> const std::vector<V27SurfaceMigrationStatus>&
    {
        return surfaces_;
    }

    /// Total number of surfaces tracked.
    [[nodiscard]] auto surface_count() const noexcept -> int
    {
        return static_cast<int>(surfaces_.size());
    }

    /// Overall migration progress (0-100).
    [[nodiscard]] auto overall_progress() const noexcept -> int;

    /// True if all tracked surfaces are fully migrated.
    [[nodiscard]] auto is_migration_complete() const noexcept -> bool;

    /// Total icons across all surfaces.
    [[nodiscard]] auto total_icons() const noexcept -> int;

    /// Total migrated icons across all surfaces.
    [[nodiscard]] auto total_migrated() const noexcept -> int;

    /// Total remaining emoji icons across all surfaces.
    [[nodiscard]] auto total_remaining_emoji() const noexcept -> int;

    /// Total remaining legacy icons across all surfaces.
    [[nodiscard]] auto total_remaining_legacy() const noexcept -> int;

    /// Surfaces that are not yet fully migrated.
    [[nodiscard]] auto incomplete_surfaces() const -> std::vector<std::string>;

    /// Populate with V27 baseline data.
    void populate_v27_baseline();

private:
    std::vector<V27SurfaceMigrationStatus> surfaces_;
    std::unordered_map<std::string, int> surface_index_; ///< name -> index in surfaces_
};

} // namespace markamp::core
