/// @file ActionReadinessGate.h
/// @brief V23 Phase 02 — Action surface and panel readiness enforcement gate.
///
/// Reconciles visible action surfaces (menus, toolbar, palette, context menus,
/// panel actions, status actions, notebook actions, canvas actions) with the
/// canonical manifest. Gates releases on zero dead/stub/unbound actions and
/// zero placeholder panels.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ActionSurfaceKind — where an action is exposed
// ============================================================================

enum class ActionSurfaceKind : uint8_t
{
    kMenu,
    kToolbar,
    kCommandPalette,
    kContextMenu,
    kPanelAction,
    kStatusAction,
    kNotebookAction,
    kCanvasAction,
};

/// Label for ActionSurfaceKind.
[[nodiscard]] constexpr auto action_surface_label(ActionSurfaceKind kind) -> const char*
{
    switch (kind)
    {
    case ActionSurfaceKind::kMenu:           return "Menu";
    case ActionSurfaceKind::kToolbar:        return "Toolbar";
    case ActionSurfaceKind::kCommandPalette: return "CommandPalette";
    case ActionSurfaceKind::kContextMenu:    return "ContextMenu";
    case ActionSurfaceKind::kPanelAction:    return "PanelAction";
    case ActionSurfaceKind::kStatusAction:   return "StatusAction";
    case ActionSurfaceKind::kNotebookAction: return "NotebookAction";
    case ActionSurfaceKind::kCanvasAction:   return "CanvasAction";
    }
    return "Unknown";
}

// ============================================================================
// ActionBindingStatus — the state of an action binding
// ============================================================================

enum class ActionBindingStatus : uint8_t
{
    kLive,         ///< Bound to a real handler and manifested
    kStub,         ///< Handler exists but is a no-op
    kDead,         ///< No handler at all
    kGated,        ///< Intentionally hidden/disabled pending completion
    kOrphaned,     ///< Visible but no manifest entry
};

/// Label for ActionBindingStatus.
[[nodiscard]] constexpr auto binding_status_label(ActionBindingStatus status) -> const char*
{
    switch (status)
    {
    case ActionBindingStatus::kLive:     return "Live";
    case ActionBindingStatus::kStub:     return "Stub";
    case ActionBindingStatus::kDead:     return "Dead";
    case ActionBindingStatus::kGated:    return "Gated";
    case ActionBindingStatus::kOrphaned: return "Orphaned";
    }
    return "Unknown";
}

// ============================================================================
// ActionReadinessItem — a single action-surface binding record
// ============================================================================

struct ActionReadinessItem
{
    std::string action_id;
    ActionSurfaceKind surface{ActionSurfaceKind::kMenu};
    ActionBindingStatus status{ActionBindingStatus::kDead};
    std::string label;
    std::string evidence_file;
    int evidence_line{0};
    bool has_handler{false};
    bool has_manifest_entry{false};

    /// Whether this item blocks the gate.
    [[nodiscard]] auto is_gate_blocker() const noexcept -> bool
    {
        return status == ActionBindingStatus::kDead ||
               status == ActionBindingStatus::kStub ||
               status == ActionBindingStatus::kOrphaned;
    }
};

// ============================================================================
// PanelReadinessItem — a panel readiness record
// ============================================================================

struct PanelReadinessItem
{
    std::string panel_id;
    std::string label;
    bool has_factory{false};       ///< Real panel factory exists
    bool has_toggle{false};        ///< Toggle/show action wired
    bool is_placeholder{false};    ///< Panel shows placeholder content
    bool is_gated{false};          ///< Intentionally hidden pending completion
    std::string gate_reason;

    /// Whether this panel blocks the gate.
    [[nodiscard]] auto is_gate_blocker() const noexcept -> bool
    {
        return (!has_factory || is_placeholder) && !is_gated;
    }
};

// ============================================================================
// ActionGateResult — result of running the readiness gate
// ============================================================================

struct ActionGateResult
{
    bool passes{false};
    std::size_t total_actions{0};
    std::size_t live_actions{0};
    std::size_t stub_actions{0};
    std::size_t dead_actions{0};
    std::size_t gated_actions{0};
    std::size_t orphaned_actions{0};
    std::size_t total_panels{0};
    std::size_t ready_panels{0};
    std::size_t placeholder_panels{0};
    std::size_t gated_panels{0};
    std::vector<std::string> blocking_reasons;

    [[nodiscard]] auto action_blocker_count() const noexcept -> std::size_t
    {
        return stub_actions + dead_actions + orphaned_actions;
    }

    [[nodiscard]] auto panel_blocker_count() const noexcept -> std::size_t
    {
        return placeholder_panels;
    }

    [[nodiscard]] auto is_clear() const noexcept -> bool
    {
        return blocking_reasons.empty();
    }
};

// ============================================================================
// ActionReadinessGate — the readiness enforcement engine
// ============================================================================

class ActionReadinessGate
{
public:
    ActionReadinessGate() = default;

    // ── Action Registration ──

    void add_action(ActionReadinessItem item);
    void add_actions(std::vector<ActionReadinessItem> items);

    // ── Panel Registration ──

    void add_panel(PanelReadinessItem item);
    void add_panels(std::vector<PanelReadinessItem> items);

    // ── Action Queries ──

    [[nodiscard]] auto action_count() const noexcept -> std::size_t;
    [[nodiscard]] auto actions_by_surface(ActionSurfaceKind surface) const
        -> std::vector<const ActionReadinessItem*>;
    [[nodiscard]] auto actions_by_status(ActionBindingStatus status) const
        -> std::vector<const ActionReadinessItem*>;
    [[nodiscard]] auto blocking_actions() const
        -> std::vector<const ActionReadinessItem*>;
    [[nodiscard]] auto all_surfaces() const -> std::vector<ActionSurfaceKind>;

    // ── Panel Queries ──

    [[nodiscard]] auto panel_count() const noexcept -> std::size_t;
    [[nodiscard]] auto ready_panels() const
        -> std::vector<const PanelReadinessItem*>;
    [[nodiscard]] auto placeholder_panels() const
        -> std::vector<const PanelReadinessItem*>;
    [[nodiscard]] auto blocking_panels() const
        -> std::vector<const PanelReadinessItem*>;

    // ── Gate Check ──

    [[nodiscard]] auto check_gate() const -> ActionGateResult;

    // ── Clear ──

    void clear();

    // ── Export ──

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<ActionReadinessItem> actions_;
    std::vector<PanelReadinessItem> panels_;
};

} // namespace markamp::core
