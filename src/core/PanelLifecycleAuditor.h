/// @file PanelLifecycleAuditor.h
/// @brief V21 Phase 05 — Panel lifecycle auditing and readiness tracking.
///
/// Extends the sidebar panel registry concept with:
///   - Panel readiness and lifecycle state tracking
///   - Placeholder/stub detection for panels not fully implemented
///   - Explorer section completeness auditing
///   - Canonical header action binding for panel headers
///   - Panel diagnostics (dead panels, stale panels, missing factories)
#pragma once

#include "core/ControlActionManifest.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// PanelLifecycleState — lifecycle stages
// ============================================================================

/// Lifecycle state of a panel.
enum class PanelLifecycleState : uint8_t
{
    kRegistered,    ///< Factory registered but panel not yet created
    kCreated,       ///< Panel instantiated
    kActive,        ///< Panel is visible and active
    kHidden,        ///< Panel exists but is hidden
    kStale,         ///< Panel needs refresh/recreation
    kDestroyed,     ///< Panel was destroyed
};

/// Label for PanelLifecycleState.
[[nodiscard]] constexpr auto panel_lifecycle_label(PanelLifecycleState state) -> const char*
{
    switch (state)
    {
    case PanelLifecycleState::kRegistered: return "Registered";
    case PanelLifecycleState::kCreated: return "Created";
    case PanelLifecycleState::kActive: return "Active";
    case PanelLifecycleState::kHidden: return "Hidden";
    case PanelLifecycleState::kStale: return "Stale";
    case PanelLifecycleState::kDestroyed: return "Destroyed";
    }
    return "Unknown";
}

// ============================================================================
// PanelReadiness — readiness classification
// ============================================================================

/// Whether a panel is production-ready or still a placeholder.
enum class PanelReadiness : uint8_t
{
    kReady,        ///< Fully implemented, production-safe
    kPlaceholder,  ///< Placeholder/stub — should be gated from production
    kExperimental, ///< Partially implemented — behind feature flag
    kDeprecated,   ///< Scheduled for removal
};

/// Label for PanelReadiness.
[[nodiscard]] constexpr auto panel_readiness_label(PanelReadiness readiness) -> const char*
{
    switch (readiness)
    {
    case PanelReadiness::kReady: return "Ready";
    case PanelReadiness::kPlaceholder: return "Placeholder";
    case PanelReadiness::kExperimental: return "Experimental";
    case PanelReadiness::kDeprecated: return "Deprecated";
    }
    return "Unknown";
}

// ============================================================================
// PanelRegistryEntry — extended panel entry with lifecycle metadata
// ============================================================================

/// Extended panel registry entry with lifecycle and readiness metadata.
struct PanelRegistryEntry
{
    std::string panel_id;         ///< Unique panel identifier
    std::string label;            ///< Display label
    std::string icon;             ///< Icon identifier
    std::string area;             ///< Host area: "primary", "secondary", "bottom"
    PanelLifecycleState lifecycle{PanelLifecycleState::kRegistered};
    PanelReadiness readiness{PanelReadiness::kReady};
    std::string feature_flag;     ///< Empty = always available; otherwise gated
    bool has_factory{false};      ///< Whether a creation factory is registered
    bool has_real_content{false}; ///< Whether the panel has real content (not empty shell)
    int sort_order{0};            ///< Display ordering

    /// Whether this panel should be visible in production.
    [[nodiscard]] auto is_production_visible() const noexcept -> bool
    {
        return readiness == PanelReadiness::kReady && has_factory && has_real_content;
    }
};

// ============================================================================
// PanelHeaderAction — canonical action bound to a panel header
// ============================================================================

/// An action button in a panel's header bar.
struct PanelHeaderAction
{
    std::string action_id;      ///< Canonical action ID
    std::string panel_id;       ///< Panel this action belongs to
    std::string label;          ///< Tooltip/label
    std::string icon;           ///< Icon identifier
    bool is_enabled{true};
    bool is_bound{false};       ///< Whether handler exists in manifest
};

// ============================================================================
// ExplorerSection — section inside the explorer panel
// ============================================================================

/// Describes a section within the Explorer panel.
struct ExplorerSection
{
    std::string section_id;     ///< e.g., "open_editors", "file_tree", "outline", "timeline"
    std::string label;          ///< Display label
    bool is_implemented{false}; ///< Whether real content exists
    bool is_collapsible{true};
    bool is_visible{true};
};

// ============================================================================
// PanelDiagnostic — audit entry
// ============================================================================

/// Diagnostic entry for panel issues.
struct PanelDiagnostic
{
    std::string panel_id;
    std::string issue;
    bool is_placeholder{false};     ///< Panel is a placeholder
    bool is_dead{false};            ///< No factory or content
    bool is_stale{false};           ///< Needs refresh
    bool is_incomplete_section{false}; ///< Explorer section not implemented
};

// ============================================================================
// PanelLifecycleAuditor — the auditing engine
// ============================================================================

/// Audits panel lifecycle, readiness, and completeness.
class PanelLifecycleAuditor
{
public:
    PanelLifecycleAuditor() = default;

    // ── Panel Registration ──

    /// Register a panel entry.
    void register_panel(PanelRegistryEntry entry);

    /// Get a panel entry by ID.
    [[nodiscard]] auto get_panel(const std::string& panel_id) const
        -> const PanelRegistryEntry*;

    /// Get all panel entries.
    [[nodiscard]] auto all_panels() const -> std::vector<const PanelRegistryEntry*>;

    /// Get panels for a specific area.
    [[nodiscard]] auto panels_for_area(const std::string& area) const
        -> std::vector<const PanelRegistryEntry*>;

    /// Get production-ready panels only.
    [[nodiscard]] auto production_panels() const -> std::vector<const PanelRegistryEntry*>;

    /// Get placeholder panels that should be gated.
    [[nodiscard]] auto placeholder_panels() const -> std::vector<const PanelRegistryEntry*>;

    /// Total panel count.
    [[nodiscard]] auto panel_count() const -> std::size_t;

    // ── Lifecycle State Transitions ──

    /// Transition a panel to a new lifecycle state.
    auto set_lifecycle(const std::string& panel_id, PanelLifecycleState state) -> bool;

    /// Mark a panel as stale (needs refresh).
    auto mark_stale(const std::string& panel_id) -> bool;

    /// Get panels in a specific lifecycle state.
    [[nodiscard]] auto panels_in_state(PanelLifecycleState state) const
        -> std::vector<const PanelRegistryEntry*>;

    // ── Header Actions ──

    /// Register a header action for a panel.
    void register_header_action(PanelHeaderAction action);

    /// Get header actions for a panel.
    [[nodiscard]] auto header_actions(const std::string& panel_id) const
        -> std::vector<const PanelHeaderAction*>;

    /// Validate header actions against manifest.
    void refresh_header_actions(const ControlActionManifest& manifest);

    // ── Explorer Sections ──

    /// Register an explorer section.
    void register_explorer_section(ExplorerSection section);

    /// Get all explorer sections.
    [[nodiscard]] auto explorer_sections() const -> std::vector<const ExplorerSection*>;

    /// Get unimplemented explorer sections.
    [[nodiscard]] auto incomplete_explorer_sections() const
        -> std::vector<const ExplorerSection*>;

    // ── Diagnostics ──

    /// Diagnose issues across all registered panels.
    [[nodiscard]] auto diagnose() const -> std::vector<PanelDiagnostic>;

    /// Count production-ready panels.
    [[nodiscard]] auto production_count() const -> std::size_t;

    /// Count placeholder/experimental panels.
    [[nodiscard]] auto non_production_count() const -> std::size_t;

private:
    std::unordered_map<std::string, PanelRegistryEntry> panels_;
    std::vector<std::string> panel_order_;
    std::vector<PanelHeaderAction> header_actions_;
    std::vector<ExplorerSection> explorer_sections_;
};

} // namespace markamp::core
