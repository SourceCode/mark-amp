/// @file ToolbarCommandBinder.h
/// @brief V21 Phase 03 — Toolbar-to-manifest binding and global button wiring.
///
/// Bridges the ToolbarModel with the canonical ControlActionManifest.
/// Provides:
///   - Model-driven toolbar population from manifest
///   - Canonical dispatch for button clicks
///   - Context-aware enablement/visibility refresh
///   - Toggle state synchronization
///   - CTA (Call-to-Action) button wiring for startup/welcome surfaces
#pragma once

#include "core/ControlActionManifest.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ToolbarButtonBinding — a single toolbar button bound to a canonical action
// ============================================================================

/// Represents a toolbar button bound to a canonical action.
struct ToolbarButtonBinding
{
    std::string action_id;        ///< Canonical action ID
    std::string label;            ///< Display label
    std::string tooltip;          ///< Full tooltip text
    std::string icon;             ///< Icon identifier
    std::string shortcut_hint;    ///< Keyboard shortcut display
    std::string context_mode;     ///< Workbench mode (empty = global)
    bool is_toggle{false};        ///< Whether this is a toggle button
    bool is_toggled{false};       ///< Current toggle state
    bool is_enabled{true};        ///< Current enablement
    bool is_visible{true};        ///< Current visibility
    bool is_bound{false};         ///< Whether handler exists in manifest
    int sort_order{0};            ///< Ordering within the toolbar

    /// Whether this binding represents a live, rendered button.
    [[nodiscard]] auto is_live() const noexcept -> bool
    {
        return is_bound && is_visible && is_enabled;
    }
};

// ============================================================================
// CTABinding — a call-to-action button on startup/welcome surfaces
// ============================================================================

/// CTA button on startup, welcome, or walkthrough surfaces.
struct CTABinding
{
    std::string action_id;       ///< Canonical action ID
    std::string label;           ///< Button label
    std::string description;     ///< Description text
    std::string surface;         ///< "startup", "welcome", "walkthrough", "empty_state"
    std::string icon;            ///< Icon identifier
    bool is_primary{false};      ///< Whether this is the primary CTA
    bool is_bound{false};        ///< Whether handler exists
    bool was_executed{false};     ///< Track if CTA was successfully triggered
};

// ============================================================================
// ToggleStateEntry — describes a toggle control's state binding
// ============================================================================

/// State binding for a toggle-style toolbar control.
struct ToggleStateEntry
{
    std::string action_id;       ///< Canonical action ID
    std::string state_key;       ///< Settings or shell-state key that drives the toggle
    bool current_state{false};   ///< Current toggle state
};

// ============================================================================
// ToolbarDiagnostic — audit entry for toolbar health
// ============================================================================

/// Diagnostic entry for a toolbar binding issue.
struct ToolbarDiagnostic
{
    std::string action_id;
    std::string issue;
    bool is_dead{false};        ///< No handler bound
    bool is_wrong_target{false}; ///< Button visible but dispatches to wrong/no action
    bool is_stale_toggle{false}; ///< Toggle state doesn't reflect real state
};

// ============================================================================
// ToolbarCommandBinder — the binding engine
// ============================================================================

/// Bridges toolbar buttons with the canonical ControlActionManifest.
class ToolbarCommandBinder
{
public:
    ToolbarCommandBinder() = default;

    // ── Toolbar Binding Registration ──

    /// Bind a toolbar button to a canonical action.
    void bind(ToolbarButtonBinding binding);

    /// Bind multiple buttons at once.
    void bind_all(std::vector<ToolbarButtonBinding> bindings);

    /// Remove a binding by action ID.
    auto unbind(const std::string& action_id) -> bool;

    // ── Lookup ──

    /// Get a binding by action ID.
    [[nodiscard]] auto get_binding(const std::string& action_id) const
        -> const ToolbarButtonBinding*;

    /// Get all bindings.
    [[nodiscard]] auto all_bindings() const -> std::vector<const ToolbarButtonBinding*>;

    /// Get bindings for a specific context mode (empty = global).
    [[nodiscard]] auto bindings_for_mode(const std::string& mode) const
        -> std::vector<const ToolbarButtonBinding*>;

    /// Get global-only bindings (no context mode).
    [[nodiscard]] auto global_bindings() const -> std::vector<const ToolbarButtonBinding*>;

    /// Total binding count.
    [[nodiscard]] auto binding_count() const -> std::size_t;

    // ── Execution ──

    /// Dispatch a button click to the manifest. Returns true if executed.
    auto dispatch_click(const std::string& action_id,
                        ControlActionManifest& manifest) -> bool;

    // ── Enablement Sync ──

    /// Refresh enablement/visibility for all bindings using current context.
    void refresh_enablement(const ControlActionManifest& manifest,
                            const ContextKeyService& context);

    /// Get bindings that are currently disabled.
    [[nodiscard]] auto disabled_bindings() const -> std::vector<const ToolbarButtonBinding*>;

    // ── Toggle Sync ──

    /// Register a toggle state binding.
    void register_toggle(ToggleStateEntry entry);

    /// Update all toggle states from their bound state keys.
    void sync_toggle_states();

    /// Set a toggle state explicitly.
    void set_toggle_state(const std::string& action_id, bool state);

    /// Get a toggle state.
    [[nodiscard]] auto get_toggle_state(const std::string& action_id) const -> bool;

    /// Get all toggle bindings.
    [[nodiscard]] auto toggle_entries() const -> std::vector<const ToggleStateEntry*>;

    // ── Manifest Sync ──

    /// Populate toolbar bindings from manifest actions targeting kToolbar surface.
    auto sync_from_manifest(const ControlActionManifest& manifest) -> int;

    // ── CTA Wiring ──

    /// Register a CTA button binding.
    void register_cta(CTABinding cta);

    /// Get all CTA bindings.
    [[nodiscard]] auto all_ctas() const -> std::vector<const CTABinding*>;

    /// Get CTAs for a specific surface.
    [[nodiscard]] auto ctas_for_surface(const std::string& surface) const
        -> std::vector<const CTABinding*>;

    /// Dispatch a CTA button click. Returns true if executed.
    auto dispatch_cta(const std::string& action_id,
                      ControlActionManifest& manifest) -> bool;

    /// Count of live (bound) CTAs.
    [[nodiscard]] auto live_cta_count() const -> std::size_t;

    /// Count of dead (unbound) CTAs.
    [[nodiscard]] auto dead_cta_count() const -> std::size_t;

    // ── Diagnostics ──

    /// Detect issues in toolbar bindings using the manifest.
    [[nodiscard]] auto diagnose(const ControlActionManifest& manifest) const
        -> std::vector<ToolbarDiagnostic>;

    /// Count of live toolbar bindings.
    [[nodiscard]] auto live_binding_count() const -> std::size_t;

    /// Count of dead toolbar bindings.
    [[nodiscard]] auto dead_binding_count() const -> std::size_t;

private:
    std::unordered_map<std::string, ToolbarButtonBinding> bindings_;
    std::vector<std::string> insertion_order_;
    std::unordered_map<std::string, ToggleStateEntry> toggle_states_;
    std::unordered_map<std::string, CTABinding> ctas_;
    std::vector<std::string> cta_insertion_order_;
};

} // namespace markamp::core
