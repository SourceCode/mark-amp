#pragma once

#include "Config.h"
#include "SettingsCatalog.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Layered configuration that cascades App → Workspace → Project.
/// Reads resolve from most-specific scope; writes target a specific scope.
///
/// Resolution order (first non-empty wins):
///   1. Project config  (per-project overrides)
///   2. Workspace config (per-workspace preferences)
///   3. App config       (global user settings)
///   4. SettingsCatalog default_value
class ScopedConfig
{
public:
    /// Construct with required app-level config and optional catalog for defaults.
    explicit ScopedConfig(Config& app_config, SettingsCatalog* catalog = nullptr);

    // ── Scoped getters (cascade Project → Workspace → App → catalog default) ──

    [[nodiscard]] auto get_string(std::string_view key, std::string_view default_val = "") const
        -> std::string;
    [[nodiscard]] auto get_int(std::string_view key, int default_val = 0) const -> int;
    [[nodiscard]] auto get_bool(std::string_view key, bool default_val = false) const -> bool;
    [[nodiscard]] auto get_double(std::string_view key, double default_val = 0.0) const -> double;

    // ── Scoped setters ──

    void set(std::string_view key,
             std::string_view value,
             ConfigScope scope = ConfigScope::kApplication);
    void set(std::string_view key, int value, ConfigScope scope = ConfigScope::kApplication);
    void set(std::string_view key, bool value, ConfigScope scope = ConfigScope::kApplication);
    void set(std::string_view key, double value, ConfigScope scope = ConfigScope::kApplication);

    /// Return which scope is currently providing the effective value for a key.
    [[nodiscard]] auto effective_scope(std::string_view key) const -> ConfigScope;

    /// Return whether a key has been explicitly set in a given scope.
    [[nodiscard]] auto has_override(std::string_view key, ConfigScope scope) const -> bool;

    /// Reset a key in a specific scope (removes the override).
    void reset_in_scope(std::string_view key, ConfigScope scope);

    /// Reset a key in all scopes (reverts to catalog default).
    void reset_to_default(std::string_view key);

    // ── Scope lifecycle ──

    /// Attach a workspace-level config layer.
    void set_workspace_config(Config* workspace_config);

    /// Attach a project-level config layer.
    void set_project_config(Config* project_config);

    /// Direct access to the underlying app config.
    [[nodiscard]] auto app_config() -> Config&
    {
        return app_config_;
    }
    [[nodiscard]] auto app_config() const -> const Config&
    {
        return app_config_;
    }

    /// Access the catalog (may be nullptr).
    [[nodiscard]] auto catalog() const -> const SettingsCatalog*
    {
        return catalog_;
    }

    // ── Batch 19-22 (#119-121) ──

    /// (#119) Return the number of attached config scopes (1-3).
    [[nodiscard]] auto scope_count() const -> int;

    /// (#120) Return the number of scopes that have at least one override.
    [[nodiscard]] auto active_scope_count() const -> int;

    /// (#121) Return all keys overridden in workspace or project scope.
    [[nodiscard]] auto all_overridden_keys() const -> std::vector<std::string>;

private:
    Config& app_config_;
    Config* workspace_config_{nullptr};
    Config* project_config_{nullptr};
    SettingsCatalog* catalog_{nullptr};

    /// Internal: resolve from most-specific scope that has the key.
    [[nodiscard]] auto resolve_config(std::string_view key) const -> const Config*;

    /// Get the catalog default value as a string, or empty if not found.
    [[nodiscard]] auto catalog_default(std::string_view key) const -> std::string;
};

} // namespace markamp::core
