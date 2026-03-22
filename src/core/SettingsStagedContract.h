/// @file SettingsStagedContract.h
/// @brief V21 Phase 09 — Staged settings contract for UI, persistence, and JSON editor.
///
/// Provides:
///   - Staged (dirty) settings state management
///   - Apply/cancel/revert operations
///   - Dirty-state detection and close confirmation
///   - JSON editor validation model
///   - Import/export contract
///   - Settings search model
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// StagedSetting — a single staged setting change
// ============================================================================

struct StagedSetting
{
    std::string setting_id;
    std::string original_value;
    std::string staged_value;

    [[nodiscard]] auto is_dirty() const noexcept -> bool
    {
        return staged_value != original_value;
    }
};

// ============================================================================
// JsonValidationResult — result of JSON settings validation
// ============================================================================

struct JsonValidationResult
{
    bool is_valid{true};
    int error_line{-1};
    int error_column{-1};
    std::string error_message;
    std::vector<std::string> warnings;
};

// ============================================================================
// SettingsSearchResult — a search match in settings
// ============================================================================

struct SettingsSearchResult
{
    std::string setting_id;
    std::string label;
    std::string category;
    std::string match_context;  ///< Where the match was found
};

// ============================================================================
// SettingsStagedContract — the staged settings engine
// ============================================================================

class SettingsStagedContract
{
public:
    SettingsStagedContract() = default;

    // ── Staged State ──

    void stage_change(const std::string& setting_id, const std::string& original,
                      const std::string& staged);
    void unstage(const std::string& setting_id);
    [[nodiscard]] auto get_staged(const std::string& setting_id) const -> const StagedSetting*;
    [[nodiscard]] auto all_staged() const -> std::vector<const StagedSetting*>;
    [[nodiscard]] auto dirty_count() const -> std::size_t;
    [[nodiscard]] auto has_dirty() const -> bool;

    // ── Apply/Cancel/Revert ──

    /// Apply all staged changes. Returns IDs that were applied.
    auto apply_all() -> std::vector<std::string>;
    /// Cancel all staged changes.
    void cancel_all();
    /// Revert a single setting to its original value.
    void revert(const std::string& setting_id);

    /// Set the callback for when a setting is applied.
    void set_apply_callback(std::function<void(const std::string&, const std::string&)> cb);

    // ── JSON Validation ──

    [[nodiscard]] static auto validate_json(const std::string& json) -> JsonValidationResult;

    // ── Import/Export ──

    /// Export all current staged values as key=value pairs.
    [[nodiscard]] auto export_staged() const -> std::vector<std::pair<std::string, std::string>>;
    /// Import key=value pairs into staged state.
    void import_values(const std::vector<std::pair<std::string, std::string>>& values);

    // ── Settings Search ──

    void register_searchable(const std::string& setting_id, const std::string& label,
                              const std::string& category);
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<SettingsSearchResult>;

    // ── State ──

    [[nodiscard]] auto total_tracked() const -> std::size_t;

private:
    std::unordered_map<std::string, StagedSetting> staged_;
    std::vector<std::string> staged_order_;
    std::function<void(const std::string&, const std::string&)> apply_callback_;

    struct SearchEntry { std::string setting_id; std::string label; std::string category; };
    std::vector<SearchEntry> search_entries_;
};

} // namespace markamp::core
