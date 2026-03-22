/// @file InlineActionRouter.h
/// @brief V21 Phase 04 — Inline action routing for panel headers, explorer toolbars, & cards.
///
/// Routes small inline controls (icon buttons, gear menus, action strips)
/// through the canonical ControlActionManifest so they are as trustworthy
/// as top-level menus.
#pragma once

#include "core/ControlActionManifest.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// InlineActionBinding — a small inline control bound to a canonical action
// ============================================================================

/// Represents an inline action button/icon bound to a canonical action.
struct InlineActionBinding
{
    std::string action_id;       ///< Canonical action ID
    std::string label;           ///< Tooltip or label
    std::string icon;            ///< Icon identifier
    std::string surface;         ///< Host surface ("panel_header", "explorer_toolbar", "extension_card")
    std::string host_id;         ///< Specific host component (e.g., panel ID)
    bool is_enabled{true};
    bool is_visible{true};
    bool is_bound{false};        ///< Whether handler exists
};

// ============================================================================
// InlineActionDiagnostic — audit entry
// ============================================================================

/// Diagnostic for inline action issues.
struct InlineActionDiagnostic
{
    std::string action_id;
    std::string surface;
    std::string host_id;
    std::string issue;
    bool is_dead{false};
    bool is_orphaned{false};     ///< No host component found
};

// ============================================================================
// InlineActionRouter — the routing engine
// ============================================================================

/// Routes inline action controls through the canonical manifest.
class InlineActionRouter
{
public:
    InlineActionRouter() = default;

    // ── Registration ──

    /// Register an inline action binding.
    void register_action(InlineActionBinding binding);

    /// Register multiple bindings.
    void register_actions(std::vector<InlineActionBinding> bindings);

    /// Remove an action by ID.
    auto remove_action(const std::string& action_id) -> bool;

    // ── Lookup ──

    /// Get a binding by action ID.
    [[nodiscard]] auto get_action(const std::string& action_id) const
        -> const InlineActionBinding*;

    /// Get all bindings.
    [[nodiscard]] auto all_actions() const -> std::vector<const InlineActionBinding*>;

    /// Get bindings for a specific surface.
    [[nodiscard]] auto actions_for_surface(const std::string& surface) const
        -> std::vector<const InlineActionBinding*>;

    /// Get bindings for a specific host component.
    [[nodiscard]] auto actions_for_host(const std::string& host_id) const
        -> std::vector<const InlineActionBinding*>;

    /// Total binding count.
    [[nodiscard]] auto action_count() const -> std::size_t;

    // ── Dispatch ──

    /// Dispatch an inline action click. Returns true if executed.
    auto dispatch(const std::string& action_id,
                  ControlActionManifest& manifest) -> bool;

    // ── Enablement ──

    /// Refresh enablement from manifest + context.
    void refresh_enablement(const ControlActionManifest& manifest,
                            const ContextKeyService& context);

    // ── Diagnostics ──

    /// Detect dead and orphaned inline actions.
    [[nodiscard]] auto diagnose(const ControlActionManifest& manifest) const
        -> std::vector<InlineActionDiagnostic>;

    /// Count of live (bound + enabled) inline actions.
    [[nodiscard]] auto live_count() const -> std::size_t;

    /// Count of dead (unbound) inline actions.
    [[nodiscard]] auto dead_count() const -> std::size_t;

private:
    std::unordered_map<std::string, InlineActionBinding> actions_;
    std::vector<std::string> insertion_order_;
};

} // namespace markamp::core
