/// @file SettingsArchitectureAuditor.h
/// @brief V21 Phase 08 — Settings architecture, schema consolidation, ownership auditing.
///
/// Provides:
///   - Settings ownership model (canonical owner vs direct writers)
///   - Catalog consolidation (single authoritative schema)
///   - Scope precedence contract (user → workspace → project)
///   - Deep-link routing validation
///   - Live vs restart settings classification
///   - Architecture diagnostics
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// SettingScope — configuration scope precedence
// ============================================================================

enum class SettingScope : uint8_t
{
    kDefault,    ///< Built-in default
    kUser,       ///< User-global
    kWorkspace,  ///< Workspace-specific
    kProject,    ///< Project-specific (highest precedence)
};

[[nodiscard]] constexpr auto setting_scope_label(SettingScope scope) -> const char*
{
    switch (scope)
    {
    case SettingScope::kDefault: return "Default";
    case SettingScope::kUser: return "User";
    case SettingScope::kWorkspace: return "Workspace";
    case SettingScope::kProject: return "Project";
    }
    return "Unknown";
}

// ============================================================================
// SettingApplicationMode — when a setting change takes effect
// ============================================================================

enum class SettingApplicationMode : uint8_t
{
    kLive,       ///< Apply immediately
    kOnReopen,   ///< Apply when document/panel re-opened
    kOnRestart,  ///< Apply on next application restart
};

[[nodiscard]] constexpr auto application_mode_label(SettingApplicationMode mode) -> const char*
{
    switch (mode)
    {
    case SettingApplicationMode::kLive: return "Live";
    case SettingApplicationMode::kOnReopen: return "OnReopen";
    case SettingApplicationMode::kOnRestart: return "OnRestart";
    }
    return "Unknown";
}

// ============================================================================
// CatalogSettingEntry — authoritative setting definition
// ============================================================================

struct CatalogSettingEntry
{
    std::string setting_id;     ///< e.g., "editor.fontSize"
    std::string label;          ///< Display label
    std::string category;       ///< Category for navigation
    std::string description;
    std::string default_value;
    SettingScope min_scope{SettingScope::kUser}; ///< Minimum scope level
    SettingApplicationMode application_mode{SettingApplicationMode::kLive};
    bool is_deprecated{false};
    bool has_validator{false};
    std::string deep_link;      ///< Deep-link route
};

// ============================================================================
// SettingsOwnershipEntry — who owns a setting mutation
// ============================================================================

struct SettingsOwnershipEntry
{
    std::string setting_id;
    std::string owner;          ///< "canonical" or specific component name
    bool is_direct_write{false}; ///< Bypasses canonical owner
};

// ============================================================================
// SettingsArchitectureDiagnostic
// ============================================================================

struct SettingsArchitectureDiagnostic
{
    std::string setting_id;
    std::string issue;
    bool is_orphaned{false};       ///< Not in catalog
    bool is_duplicate{false};      ///< Registered multiple times
    bool is_direct_write{false};   ///< Bypasses canonical owner
    bool is_missing_deep_link{false};
    bool is_deprecated{false};
};

// ============================================================================
// SettingsArchitectureAuditor — the auditing engine
// ============================================================================

class SettingsArchitectureAuditor
{
public:
    SettingsArchitectureAuditor() = default;

    // ── Catalog Registration ──

    void register_setting(CatalogSettingEntry entry);
    [[nodiscard]] auto get_setting(const std::string& setting_id) const -> const CatalogSettingEntry*;
    [[nodiscard]] auto all_settings() const -> std::vector<const CatalogSettingEntry*>;
    [[nodiscard]] auto settings_for_category(const std::string& category) const
        -> std::vector<const CatalogSettingEntry*>;
    [[nodiscard]] auto setting_count() const -> std::size_t;
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    // ── Scope Precedence ──

    void set_value(const std::string& setting_id, SettingScope scope, const std::string& value);
    [[nodiscard]] auto effective_value(const std::string& setting_id) const -> std::string;
    [[nodiscard]] auto effective_scope(const std::string& setting_id) const -> SettingScope;

    // ── Ownership Tracking ──

    void record_ownership(const std::string& setting_id, const std::string& owner, bool is_direct);
    [[nodiscard]] auto direct_writers() const -> std::vector<SettingsOwnershipEntry>;

    // ── Deep-Link Routing ──

    [[nodiscard]] auto resolve_deep_link(const std::string& setting_id) const -> std::string;
    [[nodiscard]] auto settings_missing_deep_link() const -> std::vector<std::string>;

    // ── Application Mode Classification ──

    [[nodiscard]] auto live_settings() const -> std::vector<const CatalogSettingEntry*>;
    [[nodiscard]] auto restart_required_settings() const -> std::vector<const CatalogSettingEntry*>;

    // ── Diagnostics ──

    [[nodiscard]] auto diagnose() const -> std::vector<SettingsArchitectureDiagnostic>;
    [[nodiscard]] auto catalog_count() const -> std::size_t;
    [[nodiscard]] auto deprecated_count() const -> std::size_t;

private:
    std::unordered_map<std::string, CatalogSettingEntry> catalog_;
    std::vector<std::string> catalog_order_;

    // scope → (setting_id → value)
    std::unordered_map<std::string, std::unordered_map<SettingScope, std::string>> scoped_values_;

    std::vector<SettingsOwnershipEntry> ownership_log_;
};

} // namespace markamp::core
