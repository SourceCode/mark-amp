#pragma once

/// @file SettingsCompareView.h
/// @brief Phase 36 – Model for comparing settings between scopes.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Comparison status for a setting between two scopes.
enum class SettingCompareStatus : std::uint8_t
{
    kSame = 0,   ///< Same value in both scopes.
    kDifferent,  ///< Different values.
    kOnlyInLeft, ///< Setting only exists in left scope.
    kOnlyInRight ///< Setting only exists in right scope.
};

/// A single setting comparison entry.
struct SettingCompareEntry
{
    std::string setting_id;
    std::string setting_label;
    std::string left_value;  ///< Value in the left scope (e.g., User).
    std::string right_value; ///< Value in the right scope (e.g., Workspace).
    SettingCompareStatus status{SettingCompareStatus::kSame};
};

/// Model for comparing settings between two scopes.
class SettingsCompareModel
{
public:
    /// Compute comparison between two sets of {id, value} pairs.
    void compute(const std::vector<std::pair<std::string, std::string>>& left_settings,
                 const std::vector<std::pair<std::string, std::string>>& right_settings);

    /// Get all comparison entries.
    [[nodiscard]] auto entries() const -> const std::vector<SettingCompareEntry>&
    {
        return entries_;
    }

    /// Get only entries that differ.
    [[nodiscard]] auto different_entries() const -> std::vector<SettingCompareEntry>;

    /// Number of differences.
    [[nodiscard]] auto diff_count() const -> std::size_t;

    /// Total entries.
    [[nodiscard]] auto total_count() const -> std::size_t
    {
        return entries_.size();
    }

    /// Summary text: "5 differences found".
    [[nodiscard]] auto summary_text() const -> std::string;

    /// Clear comparison data.
    void clear();

private:
    std::vector<SettingCompareEntry> entries_;
};

} // namespace markamp::ui
