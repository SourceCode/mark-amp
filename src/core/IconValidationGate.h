/// @file IconValidationGate.h
/// @brief P10-T05: Legacy icon debt removal and migration validation.
///
/// Static checks for banned legacy icon IDs, migration completeness
/// validation, and extension compatibility boundaries.
#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// A validation result entry.
struct IconValidationResult
{
    std::string icon_id;
    std::string issue; ///< "banned_legacy", "unmapped", "missing_asset"
    std::string location;
};

/// Validates icon usage against migration rules.
class IconValidationGate
{
public:
    IconValidationGate();

    /// Add a banned legacy icon ID.
    void ban_legacy_id(const std::string& legacy_id);

    /// Check if an icon ID is banned.
    [[nodiscard]] auto is_banned(const std::string& icon_id) const -> bool;

    /// Validate an icon usage.
    [[nodiscard]] auto validate(const std::string& icon_id,
                                const std::string& location) const -> IconValidationResult;

    /// Get all banned IDs.
    [[nodiscard]] auto banned_ids() const -> const std::unordered_set<std::string>&
    {
        return banned_ids_;
    }

    /// Get banned ID count.
    [[nodiscard]] auto banned_count() const -> int
    {
        return static_cast<int>(banned_ids_.size());
    }

    /// Check if migration is complete (no banned IDs in use).
    [[nodiscard]] auto migration_complete() const -> bool { return violations_ == 0; }

    /// Get violation count.
    [[nodiscard]] auto violation_count() const -> int { return violations_; }

    /// Record a violation.
    void record_violation() { ++violations_; }

private:
    void register_default_bans();

    std::unordered_set<std::string> banned_ids_;
    int violations_{0};
};

} // namespace markamp::core
