/// @file ControlActionManifest.h
/// @brief V21 Phase 01 — Canonical control-action manifest.
///
/// Provides the single source of truth for every control surface action in
/// the application. Each control (menu item, toolbar button, status bar
/// action, context menu entry, panel header action, settings control) maps
/// to a canonical ActionEntry with stable ID, metadata, enablement predicate,
/// and handler. Surfaces consume the manifest at render time; execution
/// always flows through it.
#pragma once

#include "ContextKeyService.h"

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ControlSurface — where an action can appear
// ============================================================================

/// Identifies the surface type that hosts a control.
enum class ControlSurface
{
    kMenu,           ///< Main menu bar item
    kToolbar,        ///< Toolbar button
    kStatusBar,      ///< Status bar clickable item
    kContextMenu,    ///< Right-click / context menu entry
    kPanelHeader,    ///< Panel header action button
    kInlineAction,   ///< Inline row-level action (e.g. gear icon on extension card)
    kCommandPalette, ///< Command palette entry
    kSettings,       ///< Settings control (toggle, dropdown, input)
    kStartup,        ///< Startup / welcome / walkthrough CTA button
    kOther           ///< Other surfaces
};

/// Converts ControlSurface to a human-readable label.
[[nodiscard]] inline auto control_surface_label(ControlSurface surface) noexcept
    -> const char*
{
    switch (surface)
    {
    case ControlSurface::kMenu: return "Menu";
    case ControlSurface::kToolbar: return "Toolbar";
    case ControlSurface::kStatusBar: return "StatusBar";
    case ControlSurface::kContextMenu: return "ContextMenu";
    case ControlSurface::kPanelHeader: return "PanelHeader";
    case ControlSurface::kInlineAction: return "InlineAction";
    case ControlSurface::kCommandPalette: return "CommandPalette";
    case ControlSurface::kSettings: return "Settings";
    case ControlSurface::kStartup: return "Startup";
    case ControlSurface::kOther: return "Other";
    }
    return "Unknown";
}

// ============================================================================
// ActionValidationStatus — lifecycle status of a control action
// ============================================================================

/// Validation status for a registered action.
enum class ActionValidationStatus
{
    kLive,       ///< Fully wired — handler executes a real workflow
    kPartial,    ///< Handler exists but covers only part of the workflow
    kStub,       ///< Handler exists but is a no-op or placeholder
    kDead,       ///< Renders but has no handler registered
    kDuplicate,  ///< Multiple handlers compete for the same action ID
    kDeprecated, ///< Marked for removal
    kGated       ///< Hidden behind a feature flag
};

/// Converts ActionValidationStatus to a human-readable label.
[[nodiscard]] inline auto validation_status_label(ActionValidationStatus status) noexcept
    -> const char*
{
    switch (status)
    {
    case ActionValidationStatus::kLive: return "Live";
    case ActionValidationStatus::kPartial: return "Partial";
    case ActionValidationStatus::kStub: return "Stub";
    case ActionValidationStatus::kDead: return "Dead";
    case ActionValidationStatus::kDuplicate: return "Duplicate";
    case ActionValidationStatus::kDeprecated: return "Deprecated";
    case ActionValidationStatus::kGated: return "Gated";
    }
    return "Unknown";
}

// ============================================================================
// EnablementPredicate — callable that checks whether an action is enabled
// ============================================================================

/// Returns true if the action should be enabled in the given context.
using EnablementPredicate = std::function<bool(const ContextKeyService&)>;

/// Returns true if the action should be visible in the given context.
using VisibilityPredicate = std::function<bool(const ContextKeyService&)>;

// ============================================================================
// ActionEntry — canonical metadata for a single control action
// ============================================================================

/// Canonical action metadata. Every visible surface control maps to one entry.
struct ActionEntry
{
    std::string action_id;      ///< Stable identifier, e.g. "file.save"
    std::string label;          ///< Display label, e.g. "Save"
    std::string tooltip;        ///< Rich tooltip text
    std::string icon;           ///< Icon identifier (Lucide/MUI key)
    std::string shortcut_hint;  ///< Keyboard shortcut display string
    std::string category;       ///< Grouping, e.g. "File", "Edit", "View"
    std::string description;    ///< Detailed description for palette/help
    std::string when_clause;    ///< VS Code-style when expression (empty = always)
    std::string feature_flag;   ///< Feature flag that gates this action (empty = ungated)

    /// Surfaces where this action can appear.
    std::vector<ControlSurface> surfaces;

    /// Function to execute the action. Returns true on success.
    std::function<bool()> handler;

    /// Predicate to determine if the action is enabled.
    EnablementPredicate enablement;

    /// Predicate to determine if the action is visible.
    VisibilityPredicate visibility;

    /// Current validation status.
    ActionValidationStatus validation_status{ActionValidationStatus::kLive};

    /// Whether the action has a real handler bound.
    [[nodiscard]] auto has_handler() const noexcept -> bool
    {
        return static_cast<bool>(handler);
    }

    /// Check enablement against the given context.
    [[nodiscard]] auto is_enabled(const ContextKeyService& context) const -> bool
    {
        if (!enablement)
        {
            return true; // No predicate = always enabled
        }
        return enablement(context);
    }

    /// Check visibility against the given context.
    [[nodiscard]] auto is_visible(const ContextKeyService& context) const -> bool
    {
        if (!visibility)
        {
            return true; // No predicate = always visible
        }
        return visibility(context);
    }

    /// Whether this action appears on a specific surface.
    [[nodiscard]] auto appears_on(ControlSurface surface) const -> bool
    {
        for (const auto& s : surfaces)
        {
            if (s == surface)
            {
                return true;
            }
        }
        return false;
    }

    /// Whether this action is fully wired (has handler + live status).
    [[nodiscard]] auto is_fully_wired() const noexcept -> bool
    {
        return has_handler() && validation_status == ActionValidationStatus::kLive;
    }
};

// ============================================================================
// ControlActionManifest — canonical action registry
// ============================================================================

/// Central manifest for all control actions. Every UI surface reads from this
/// manifest to determine what actions to render, how to label them, and whom
/// to call on activation.
class ControlActionManifest
{
public:
    ControlActionManifest() = default;

    // ── Registration ──

    /// Register an action. Overwrites if the action_id already exists.
    void register_action(ActionEntry entry);

    /// Register multiple actions at once.
    void register_actions(std::vector<ActionEntry> entries);

    /// Remove an action by ID. Returns true if found and removed.
    auto unregister_action(const std::string& action_id) -> bool;

    // ── Lookup ──

    /// Get an action by ID. Returns nullptr if not found.
    [[nodiscard]] auto get_action(const std::string& action_id) const -> const ActionEntry*;

    /// Get a mutable action by ID. Returns nullptr if not found.
    [[nodiscard]] auto get_action_mut(const std::string& action_id) -> ActionEntry*;

    /// List all registered actions.
    [[nodiscard]] auto all_actions() const -> std::vector<const ActionEntry*>;

    /// List actions that appear on a given surface.
    [[nodiscard]] auto actions_for_surface(ControlSurface surface) const
        -> std::vector<const ActionEntry*>;

    /// List actions in a given category.
    [[nodiscard]] auto actions_for_category(const std::string& category) const
        -> std::vector<const ActionEntry*>;

    /// List actions that are currently enabled and visible.
    [[nodiscard]] auto active_actions(const ContextKeyService& context) const
        -> std::vector<const ActionEntry*>;

    /// Whether an action ID is registered.
    [[nodiscard]] auto has_action(const std::string& action_id) const -> bool;

    /// Total registered action count.
    [[nodiscard]] auto action_count() const -> std::size_t;

    // ── Execution ──

    /// Execute an action by ID. Returns false if not found or handler fails.
    auto execute_action(const std::string& action_id) -> bool;

    // ── Validation & Audit ──

    /// List all actions with a specific validation status.
    [[nodiscard]] auto actions_with_status(ActionValidationStatus status) const
        -> std::vector<const ActionEntry*>;

    /// List all actions that have no handler (dead affordances).
    [[nodiscard]] auto unresolved_actions() const -> std::vector<const ActionEntry*>;

    /// List all actions that are stubs or partials.
    [[nodiscard]] auto incomplete_actions() const -> std::vector<const ActionEntry*>;

    /// List all unique category names.
    [[nodiscard]] auto get_categories() const -> std::vector<std::string>;

    /// Count of actions per validation status.
    struct ValidationSummary
    {
        int live{0};
        int partial{0};
        int stub{0};
        int dead{0};
        int duplicate{0};
        int deprecated{0};
        int gated{0};
    };

    /// Get aggregate validation counts.
    [[nodiscard]] auto validation_summary() const -> ValidationSummary;

private:
    /// All registered actions, keyed by action_id.
    std::unordered_map<std::string, ActionEntry> actions_;

    /// Insertion order for deterministic iteration.
    std::vector<std::string> insertion_order_;
};

} // namespace markamp::core
