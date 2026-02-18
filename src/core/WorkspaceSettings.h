/// @file WorkspaceSettings.h
/// @brief Phase 20 – VS Code-style cascading workspace settings.

#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Enums & Data Structures
// ============================================================================

/// Scope for a setting value (ordered by priority: Folder > Workspace > User > Default).
enum class SettingScope : std::uint8_t
{
    kDefault,
    kUser,
    kWorkspace,
    kFolder
};

/// A single setting value with scope information.
struct SettingValue
{
    std::string key;
    SettingScope scope{SettingScope::kDefault};
    std::string string_value;
    int int_value{0};
    bool bool_value{false};
    std::string source_path; ///< Path of the settings file that defined this value.
};

// ============================================================================
// WorkspaceSettings
// ============================================================================

/// Provides VS Code-style cascading settings with scope resolution.
///
/// Settings cascade from Default → User → Workspace → Folder,
/// with higher scopes overriding lower ones.
///
/// Usage:
/// ```cpp
/// WorkspaceSettings settings(event_bus);
/// settings.set("editor.fontSize", "14", SettingScope::kUser);
/// auto val = settings.effective_value("editor.fontSize");
/// ```
class WorkspaceSettings
{
public:
    explicit WorkspaceSettings(EventBus& event_bus);

    // ── Getters ──

    /// Get a string setting at the given scope.
    [[nodiscard]] auto get_string(const std::string& key,
                                  SettingScope scope = SettingScope::kDefault) const
        -> std::optional<std::string>;

    /// Get an int setting at the given scope.
    [[nodiscard]] auto get_int(const std::string& key,
                               SettingScope scope = SettingScope::kDefault) const
        -> std::optional<int>;

    /// Get a bool setting at the given scope.
    [[nodiscard]] auto get_bool(const std::string& key,
                                SettingScope scope = SettingScope::kDefault) const
        -> std::optional<bool>;

    // ── Setters ──

    /// Set a string value at the given scope.
    void
    set(const std::string& key, const std::string& value, SettingScope scope = SettingScope::kUser);

    /// Set an int value at the given scope.
    void set_int(const std::string& key, int value, SettingScope scope = SettingScope::kUser);

    /// Set a bool value at the given scope.
    void set_bool(const std::string& key, bool value, SettingScope scope = SettingScope::kUser);

    // ── Management ──

    /// Remove a setting at the given scope.
    auto remove(const std::string& key, SettingScope scope) -> bool;

    /// Check if a key exists at any scope.
    [[nodiscard]] auto has(const std::string& key,
                           SettingScope scope = SettingScope::kDefault) const -> bool;

    /// Get all keys at a given scope.
    [[nodiscard]] auto all_keys(SettingScope scope) const -> std::vector<std::string>;

    /// Get the number of settings at a scope.
    [[nodiscard]] auto count(SettingScope scope) const -> std::size_t;

    // ── Cascade Resolution ──

    /// Get the effective value for a key by cascading through all scopes.
    /// Returns the value from the highest-priority scope that has the key.
    [[nodiscard]] auto effective_value(const std::string& key) const -> std::optional<SettingValue>;

    // ── Persistence ──

    /// Load workspace settings from a JSON file.
    auto load_workspace_settings(const std::string& path) -> std::expected<void, std::string>;

    /// Save workspace settings to a JSON file.
    auto save_workspace_settings(const std::string& path) const -> std::expected<void, std::string>;

    // ── Reset ──

    /// Reset all settings to defaults.
    void reset_to_defaults();

    /// Clear settings at a specific scope.
    void clear_scope(SettingScope scope);

private:
    EventBus& event_bus_;

    /// Storage: scope -> (key -> value).
    std::unordered_map<std::string, SettingValue> default_settings_;
    std::unordered_map<std::string, SettingValue> user_settings_;
    std::unordered_map<std::string, SettingValue> workspace_settings_;
    std::unordered_map<std::string, SettingValue> folder_settings_;

    /// Get the map for a given scope.
    [[nodiscard]] auto scope_map(SettingScope scope) const
        -> const std::unordered_map<std::string, SettingValue>&;

    /// Get the mutable map for a given scope.
    auto scope_map_mut(SettingScope scope) -> std::unordered_map<std::string, SettingValue>&;
};

} // namespace markamp::core
