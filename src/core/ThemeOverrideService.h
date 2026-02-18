/// @file ThemeOverrideService.h
/// @brief V9 Phase 38 — Per-vault/workspace theme token overrides.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ThemeOverride — a single token override
// ============================================================================

enum class OverrideScope : uint8_t
{
    kVault,
    kWorkspace,
    kGlobal
};

struct ThemeOverride
{
    std::string token_name;     ///< e.g. "editor.background"
    std::string original_value; ///< Original theme value
    std::string override_value; ///< User-specified override
    OverrideScope scope{OverrideScope::kVault};
};

// ============================================================================
// ThemeOverrideService
// ============================================================================

/// Manages per-vault and per-workspace theme token overrides.
///
/// Usage:
/// ```cpp
/// ThemeOverrideService service;
/// service.set_override("editor.background", "#1a1a2e", "#ffffff", OverrideScope::kVault);
/// auto merged = service.apply_overrides(base_token_map);
/// ```
class ThemeOverrideService
{
public:
    ThemeOverrideService() = default;

    // ── CRUD ──

    /// Set a theme token override.
    void set_override(const std::string& token_name,
                      const std::string& original_value,
                      const std::string& override_value,
                      OverrideScope scope = OverrideScope::kVault);

    /// Remove an override by token name and scope.
    auto remove_override(const std::string& token_name, OverrideScope scope) -> bool;

    /// Get an override by token name and scope.
    [[nodiscard]] auto get_override(const std::string& token_name, OverrideScope scope) const
        -> const ThemeOverride*;

    /// List all overrides.
    [[nodiscard]] auto list_overrides() const -> const std::vector<ThemeOverride>&;

    /// Get overrides filtered by scope.
    [[nodiscard]] auto overrides_for_scope(OverrideScope scope) const
        -> std::vector<const ThemeOverride*>;

    /// Total override count.
    [[nodiscard]] auto override_count() const -> std::size_t;

    // ── Application ──

    /// Apply all overrides to a base token map. Returns merged map.
    /// The map is token_name → value.
    [[nodiscard]] auto
    apply_overrides(const std::vector<std::pair<std::string, std::string>>& base_tokens) const
        -> std::vector<std::pair<std::string, std::string>>;

    /// Reset all overrides for a given scope.
    void reset_overrides(OverrideScope scope);

    /// Reset all overrides across all scopes.
    void reset_all();

    // ── Export / Import ──

    /// Export overrides as JSON.
    [[nodiscard]] auto export_overrides() const -> std::string;

    /// Import overrides from JSON. Returns number imported.
    auto import_overrides(const std::string& json_data) -> int;

    /// Scope enum to string.
    [[nodiscard]] static auto scope_name(OverrideScope scope) -> std::string;

private:
    std::vector<ThemeOverride> overrides_;
};

} // namespace markamp::core
