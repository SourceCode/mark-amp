/// @file MuiIconMigration.h
/// @brief P10-T03: Batch icon migration tracking.
///
/// Tracks migration progress by surface category with replacement
/// status per icon and progress reporting.
#pragma once

#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Surface category for icon migration.
enum class IconSurface
{
    kShellChrome,
    kFileTree,
    kPanelHeaders,
    kStatusBar,
    kDialogs,
    kCommands,
    kEmptyStates,
};

/// Replacement status per icon.
enum class ReplacementStatus
{
    kMigrated,
    kPending,
    kCustomAdapter,
    kSkipped,
};

/// Tracks icon migration progress.
class MuiIconMigration
{
public:
    MuiIconMigration() = default;

    /// Mark an icon as migrated for a surface.
    void mark_migrated(IconSurface surface, const std::string& icon_id);

    /// Mark an icon as pending.
    void mark_pending(IconSurface surface, const std::string& icon_id);

    /// Get migration progress for a surface (0.0 to 1.0).
    [[nodiscard]] auto progress(IconSurface surface) const -> double;

    /// Get overall migration progress.
    [[nodiscard]] auto overall_progress() const -> double;

    /// Get total migrated count.
    [[nodiscard]] auto migrated_count() const -> int { return migrated_; }

    /// Get total icon count.
    [[nodiscard]] auto total_count() const -> int { return total_; }

private:
    std::unordered_map<int, int> surface_migrated_;
    std::unordered_map<int, int> surface_total_;
    int migrated_{0};
    int total_{0};
};

} // namespace markamp::core
