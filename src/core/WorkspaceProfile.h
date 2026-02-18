/// @file WorkspaceProfile.h
/// @brief V9 Phase 38 — Workspace profile save/restore/diff.
#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// WorkspaceProfile — a complete workspace configuration snapshot
// ============================================================================

struct WorkspaceProfile
{
    std::string name;
    std::string description;
    std::string settings_json;   ///< Serialized workspace settings
    std::string layout_json;     ///< Serialized panel layout
    std::string vault_css;       ///< Custom vault CSS
    std::string font_config;     ///< Serialized font configuration
    std::string theme_overrides; ///< Serialized theme overrides
    std::chrono::system_clock::time_point created_at;
    bool is_builtin{false};
};

// ============================================================================
// ProfileDiff — differences between two workspace profiles
// ============================================================================

struct ProfileDiffEntry
{
    std::string section;   ///< "settings", "layout", "css", "fonts", "theme"
    std::string key;       ///< Specific changed key or "content"
    std::string left_val;  ///< Value in profile A
    std::string right_val; ///< Value in profile B
};

struct ProfileDiff
{
    std::string left_name;
    std::string right_name;
    std::vector<ProfileDiffEntry> differences;
    [[nodiscard]] auto has_differences() const -> bool
    {
        return !differences.empty();
    }
    [[nodiscard]] auto diff_count() const -> std::size_t
    {
        return differences.size();
    }
};

// ============================================================================
// WorkspaceProfileManager
// ============================================================================

/// Manages complete workspace profiles for save/restore of entire configurations.
///
/// Usage:
/// ```cpp
/// WorkspaceProfileManager mgr;
/// WorkspaceProfile profile;
/// profile.name = "Writing Mode";
/// profile.settings_json = "{...}";
/// mgr.save_profile(std::move(profile));
/// auto* p = mgr.get_profile("Writing Mode");
/// ```
class WorkspaceProfileManager
{
public:
    WorkspaceProfileManager() = default;

    // ── CRUD ──

    /// Save a profile. Replaces if name already exists.
    void save_profile(WorkspaceProfile profile);

    /// Get a profile by name. Returns nullptr if not found.
    [[nodiscard]] auto get_profile(const std::string& name) const -> const WorkspaceProfile*;

    /// Delete a profile by name. Returns true if found. Built-in profiles cannot be deleted.
    auto delete_profile(const std::string& name) -> bool;

    /// Rename a profile. Returns true on success.
    auto rename_profile(const std::string& old_name, const std::string& new_name) -> bool;

    /// List all profiles.
    [[nodiscard]] auto list_profiles() const -> std::vector<std::string>;

    /// Total profile count.
    [[nodiscard]] auto profile_count() const -> std::size_t;

    /// Check if a profile exists.
    [[nodiscard]] auto has_profile(const std::string& name) const -> bool;

    // ── Diff ──

    /// Compare two profiles and list all differences.
    [[nodiscard]] auto diff_profiles(const std::string& name_a, const std::string& name_b) const
        -> ProfileDiff;

    // ── Export / Import ──

    /// Export a single profile as JSON.
    [[nodiscard]] auto export_profile(const std::string& name) const -> std::string;

    /// Import a profile from JSON. Returns the imported profile name.
    auto import_profile(const std::string& json_data) -> std::string;

    /// Export all profiles as JSON array.
    [[nodiscard]] auto export_all() const -> std::string;

    // ── Builtins ──

    /// Load built-in profiles.
    void load_builtins();

private:
    std::vector<WorkspaceProfile> profiles_;
};

} // namespace markamp::core
