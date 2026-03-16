#pragma once

#include "IPlugin.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Configuration scope — determines where a setting value is stored.
enum class ConfigScope
{
    kApplication, ///< Global user preference
    kWorkspace,   ///< Per-workspace override
    kProject      ///< Per-project override
};

/// A single entry in the settings catalog.
/// Superset of SettingDefinition and SettingContribution with additional catalog metadata.
struct CatalogEntry
{
    std::string setting_id;                       ///< Unique key, e.g. "editor.fontSize"
    std::string label;                            ///< Display label, e.g. "Font Size"
    std::string description;                      ///< Help text
    std::string group;                            ///< Top-level group, e.g. "Editor", "Appearance"
    std::string subgroup;                         ///< Optional subgroup, e.g. "Cursor", "Minimap"
    ConfigScope scope{ConfigScope::kApplication}; ///< Default scope
    SettingType type{SettingType::Boolean};       ///< Value type
    std::string default_value;                    ///< Serialized default
    std::vector<std::string> choices;             ///< For Choice type only
    std::vector<std::string> keywords;            ///< Search keywords
    bool restart_required{false};                 ///< Whether change requires restart
    bool deprecated{false};                       ///< Whether setting is deprecated
    bool experimental{false};                     ///< Whether setting is experimental
    int min_int{0};                               ///< For Integer type
    int max_int{100};                             ///< For Integer type
    int order_priority{100};                      ///< Sort weight within category
    std::string source;                           ///< "builtin" or plugin ID
};

/// Hierarchical settings catalog with registration, query, and keyword-based search.
/// Serves as the single source of truth for all available settings.
class SettingsCatalog
{
public:
    /// Register a single setting entry.
    void register_setting(CatalogEntry entry);

    /// Register multiple setting entries at once.
    void register_settings(std::vector<CatalogEntry> entries);

    /// Register all built-in settings (migrated from SettingsPanel::RegisterBuiltinSettings).
    void register_builtins();

    /// Ingest plugin-contributed settings, converting SettingContribution → CatalogEntry.
    void ingest_plugin_settings(const std::vector<SettingContribution>& contributions);

    /// Return all registered settings.
    [[nodiscard]] auto all_settings() const -> const std::vector<CatalogEntry>&;

    /// Return settings belonging to a specific group.
    [[nodiscard]] auto settings_for_group(std::string_view group) const
        -> std::vector<const CatalogEntry*>;

    /// Find a specific setting by its ID. Returns nullptr if not found.
    [[nodiscard]] auto find_setting(std::string_view setting_id) const -> const CatalogEntry*;

    /// Return the ordered list of unique group names.
    [[nodiscard]] auto groups() const -> std::vector<std::string>;

    /// Search settings by keyword/label/description. Case-insensitive substring match.
    [[nodiscard]] auto search(std::string_view query) const -> std::vector<const CatalogEntry*>;

    /// Return the number of registered settings.
    [[nodiscard]] auto size() const -> std::size_t;

    // ── Batch 8: Additional catalog methods ──

    /// Return all settings that require a restart when changed.
    [[nodiscard]] auto settings_requiring_restart() const -> std::vector<const CatalogEntry*>;

    /// Return all settings from a specific source (e.g. "builtin" or a plugin ID).
    [[nodiscard]] auto settings_from_source(std::string_view source) const
        -> std::vector<const CatalogEntry*>;

    /// Return unique subgroup names within a group.
    [[nodiscard]] auto subgroups_for_group(std::string_view group) const
        -> std::vector<std::string>;

    /// Check whether a setting exists in the catalog.
    [[nodiscard]] auto has_setting(std::string_view setting_id) const -> bool;

    /// Remove a setting by ID (for plugin unload).
    void remove_setting(std::string_view setting_id);

    /// Remove all registered settings.
    void clear();

    // ── New Batch 8: Additional catalog query methods (#49-54) ──

    /// Return all settings marked as deprecated.
    [[nodiscard]] auto deprecated_settings() const -> std::vector<const CatalogEntry*>;

    /// Return all settings marked as experimental.
    [[nodiscard]] auto experimental_settings() const -> std::vector<const CatalogEntry*>;

    /// Return entries sorted by order_priority within each group.
    [[nodiscard]] auto settings_sorted_by_priority() const -> std::vector<const CatalogEntry*>;

    /// Return all entries of a given SettingType.
    [[nodiscard]] auto settings_by_type(SettingType type) const -> std::vector<const CatalogEntry*>;

    /// Return a deduplicated list of all group names.
    [[nodiscard]] auto unique_groups() const -> std::vector<std::string>;

    /// Merge another catalog's entries into this one (for extension contributions).
    void merge_catalog(const SettingsCatalog& other);

    // ── Batch 19-22 (#116-118) ──

    /// (#116) Return the total number of distinct groups.
    [[nodiscard]] auto group_count() const -> std::size_t;

    /// (#117) Return the number of settings in a given ConfigScope.
    [[nodiscard]] auto scope_count(ConfigScope scope) const -> std::size_t;

    /// (#118) Return the count of results matching a keyword search query.
    [[nodiscard]] auto keyword_search_count(std::string_view query) const -> std::size_t;

private:
    std::vector<CatalogEntry> entries_;
    std::unordered_map<std::string, std::size_t> id_index_; ///< setting_id → index in entries_
};

} // namespace markamp::core
