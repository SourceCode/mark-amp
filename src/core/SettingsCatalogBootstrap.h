/// @file SettingsCatalogBootstrap.h
/// @brief P06-T02: Registers built-in settings catalog at startup.
///
/// Creates the canonical settings catalog and attaches it to Config and
/// ScopedConfig consumers so that defaults, validation, and search
/// all reflect the same schema.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
class EventBus;

/// A single setting definition in the catalog.
struct SettingDefinition
{
    std::string key;
    std::string label;
    std::string description;
    std::string category;
    std::string default_value;
    std::string value_type; ///< "string", "bool", "int", "double", "enum"
    std::vector<std::string> enum_values; ///< Valid values for enum type
    bool requires_restart{false};
};

/// Bootstraps the settings catalog and attaches it to Config.
class SettingsCatalogBootstrap
{
public:
    SettingsCatalogBootstrap(EventBus& bus, Config& cfg);

    /// Register a setting definition.
    void register_setting(const SettingDefinition& def);

    /// Get a setting definition by key.
    [[nodiscard]] auto get_definition(const std::string& key) const
        -> const SettingDefinition*;

    /// Get all setting definitions.
    [[nodiscard]] auto all_definitions() const
        -> const std::unordered_map<std::string, SettingDefinition>&
    {
        return catalog_;
    }

    /// Get settings in a category.
    [[nodiscard]] auto by_category(const std::string& category) const
        -> std::vector<const SettingDefinition*>;

    /// Search settings by query.
    [[nodiscard]] auto search(const std::string& query) const
        -> std::vector<const SettingDefinition*>;

    /// Get catalog size.
    [[nodiscard]] auto size() const -> int
    {
        return static_cast<int>(catalog_.size());
    }

    /// Validate a value against the catalog schema.
    [[nodiscard]] auto validate(const std::string& key, const std::string& value) const -> bool;

private:
    void register_built_in_settings();

    EventBus& event_bus_;
    Config& config_;
    std::unordered_map<std::string, SettingDefinition> catalog_;
};

} // namespace markamp::core
