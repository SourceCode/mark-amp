/// @file ContextMenuActionBinder.h
/// @brief V21 Phase 04 — Context menu → manifest binding with target-aware enablement.
///
/// Bridges ContextMenuRegistry with the canonical ControlActionManifest.
/// Provides:
///   - Target-context payloads for context-sensitive menus
///   - Manifest-driven menu item enablement/visibility
///   - No-op action detection and blocking
///   - Keyboard accessibility auditing
///   - Context menu diagnostics
#pragma once

#include "core/ControlActionManifest.h"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace markamp::core
{

// ============================================================================
// TargetContext — rich context payload for context menus
// ============================================================================

/// The type of target that a context menu is operating on.
enum class TargetType : uint8_t
{
    kFile,
    kFolder,
    kEmptyArea,
    kTab,
    kEditorSelection,
    kPanelItem,
    kTerminalLine,
    kSearchResult,
    kProblemEntry,
};

/// Label for TargetType.
[[nodiscard]] constexpr auto target_type_label(TargetType type) -> const char*
{
    switch (type)
    {
    case TargetType::kFile: return "File";
    case TargetType::kFolder: return "Folder";
    case TargetType::kEmptyArea: return "EmptyArea";
    case TargetType::kTab: return "Tab";
    case TargetType::kEditorSelection: return "EditorSelection";
    case TargetType::kPanelItem: return "PanelItem";
    case TargetType::kTerminalLine: return "TerminalLine";
    case TargetType::kSearchResult: return "SearchResult";
    case TargetType::kProblemEntry: return "ProblemEntry";
    }
    return "Unknown";
}

/// Rich context payload passed into context menu generation.
struct TargetContext
{
    TargetType type{TargetType::kEmptyArea};
    std::string target_id;       ///< Identifier of the target (file path, tab ID, etc.)
    std::string surface;         ///< Surface name ("file_tree", "tab_bar", "editor", etc.)
    bool is_dirty{false};        ///< Whether the target has unsaved changes
    bool is_readonly{false};     ///< Whether the target is read-only
    bool has_selection{false};    ///< Whether text/objects are selected
    int item_count{0};           ///< Number of items in multi-select
};

// ============================================================================
// ContextMenuBinding — a single context menu item bound to a canonical action
// ============================================================================

/// Represents a context menu item bound to a canonical action.
struct ContextMenuBinding
{
    std::string action_id;       ///< Canonical action ID
    std::string label;           ///< Display label
    std::string icon;            ///< Icon identifier
    std::string shortcut_hint;   ///< Keyboard shortcut display
    std::string group;           ///< Grouping within the menu
    int sort_order{0};           ///< Order within group
    bool is_enabled{true};       ///< Context-dependent enablement
    bool is_visible{true};       ///< Context-dependent visibility
    bool is_bound{false};        ///< Whether handler exists
    bool is_destructive{false};  ///< Requires confirmation
    bool is_separator_before{false}; ///< Draw separator before this item
};

// ============================================================================
// ContextMenuDiagnostic — audit entry
// ============================================================================

/// Diagnostic entry for context menu issues.
struct ContextMenuDiagnostic
{
    std::string context_type;    ///< e.g. "file_tree", "tab_bar"
    std::string action_id;
    std::string issue;
    bool is_noop{false};         ///< Action exists but does nothing
    bool is_missing_target{false}; ///< Menu shown without appropriate target
    bool is_no_keyboard{false};  ///< Not reachable via keyboard
};

// ============================================================================
// ContextMenuActionBinder — the binding engine
// ============================================================================

/// Bridges context menus with the canonical ControlActionManifest.
class ContextMenuActionBinder
{
public:
    ContextMenuActionBinder() = default;

    // ── Context Menu Registration ──

    /// Register bindings for a context menu type (e.g., "file_tree", "tab_bar").
    void register_context(const std::string& context_type,
                          std::vector<ContextMenuBinding> bindings);

    /// Get all registered context types.
    [[nodiscard]] auto registered_contexts() const -> std::vector<std::string>;

    /// Check if a context type is registered.
    [[nodiscard]] auto has_context(const std::string& context_type) const -> bool;

    /// Get bindings for a specific context type.
    [[nodiscard]] auto bindings_for_context(const std::string& context_type) const
        -> std::vector<const ContextMenuBinding*>;

    /// Total context type count.
    [[nodiscard]] auto context_count() const -> std::size_t;

    // ── Target-Aware Resolution ──

    /// Resolve a context menu for a given target. Returns bindings with
    /// enablement/visibility adjusted based on the target context.
    [[nodiscard]] auto resolve(const std::string& context_type,
                               const TargetContext& target,
                               const ControlActionManifest& manifest) const
        -> std::vector<ContextMenuBinding>;

    // ── Dispatch ──

    /// Dispatch a context menu action. Returns true if executed.
    auto dispatch(const std::string& action_id,
                  const TargetContext& target,
                  ControlActionManifest& manifest) -> bool;

    // ── No-Op Blocking ──

    /// Tag an action as a no-op (partially implemented). Blocked actions
    /// will be disabled in menus and flagged in diagnostics.
    void block_noop(const std::string& action_id, const std::string& reason);

    /// Unblock a previously blocked action.
    void unblock(const std::string& action_id);

    /// Check if an action is blocked as a no-op.
    [[nodiscard]] auto is_blocked(const std::string& action_id) const -> bool;

    /// Get the reason an action is blocked.
    [[nodiscard]] auto block_reason(const std::string& action_id) const -> std::string;

    /// Get all blocked action IDs.
    [[nodiscard]] auto blocked_actions() const -> std::vector<std::string>;

    // ── Keyboard Accessibility ──

    /// Register that an action is keyboard-accessible.
    void set_keyboard_accessible(const std::string& action_id, bool accessible);

    /// Check keyboard accessibility for an action.
    [[nodiscard]] auto is_keyboard_accessible(const std::string& action_id) const -> bool;

    /// Get actions that are NOT keyboard-accessible (accessibility gap).
    [[nodiscard]] auto keyboard_gaps() const -> std::vector<std::string>;

    // ── Diagnostics ──

    /// Diagnose issues across all registered context menus.
    [[nodiscard]] auto diagnose(const ControlActionManifest& manifest) const
        -> std::vector<ContextMenuDiagnostic>;

    /// Count of live (bound + enabled) bindings across all contexts.
    [[nodiscard]] auto total_live_bindings() const -> std::size_t;

    /// Count of dead (unbound) bindings across all contexts.
    [[nodiscard]] auto total_dead_bindings() const -> std::size_t;

private:
    /// Context type → list of bindings.
    std::unordered_map<std::string, std::vector<ContextMenuBinding>> contexts_;
    std::vector<std::string> context_order_;

    /// Blocked no-op actions: action_id → reason.
    std::unordered_map<std::string, std::string> blocked_;

    /// Keyboard accessibility: action_id → accessible.
    std::unordered_map<std::string, bool> keyboard_access_;
};

} // namespace markamp::core
