// ============================================================================
// File: src/canvas/CanvasContextMenu.h
// Phase 11: Canvas Workbench Shell — right-click context menu
// ============================================================================
#pragma once

#include "canvas/CanvasTypes.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;
class SelectionManager;
class UndoRedoStack;

/// An action that can appear in the canvas context menu.
struct ContextMenuAction
{
    std::string id;            ///< Machine identifier (e.g. "canvas.cut")
    std::string label;         ///< Display label (e.g. "Cut")
    std::string shortcut_hint; ///< Keyboard shortcut hint (e.g. "Ctrl+X")
    std::string icon_name;     ///< Optional icon identifier
    bool enabled{true};
    bool is_separator{false};   ///< If true, rendered as a separator line
    bool is_destructive{false}; ///< Red-tinted for destructive actions (delete)

    // ── Round 2 Batch 6 (#51-54) ──────────────────────────────────

    /// (#51) Whether the action is enabled.
    [[nodiscard]] auto is_enabled() const noexcept -> bool
    {
        return enabled;
    }

    /// (#52) Whether the action is disabled.
    [[nodiscard]] auto is_disabled() const noexcept -> bool
    {
        return !enabled;
    }

    /// (#53) Whether a shortcut hint is available.
    [[nodiscard]] auto has_shortcut() const noexcept -> bool
    {
        return !shortcut_hint.empty();
    }

    /// (#54) Whether an icon is assigned.
    [[nodiscard]] auto has_icon() const noexcept -> bool
    {
        return !icon_name.empty();
    }
};

/// Groups of context menu actions.
struct ContextMenuGroup
{
    std::string group_name;
    std::vector<ContextMenuAction> actions;

    // ── Round 2 Batch 6 (#55-56) ──────────────────────────────────

    /// (#55) Whether this group has no actions.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return actions.empty();
    }

    /// (#56) Number of actions in this group.
    [[nodiscard]] auto action_count() const noexcept -> size_t
    {
        return actions.size();
    }
};

/// Builds context-sensitive menus for canvas interactions.
///
/// The menu contents change depending on:
///   - Whether any objects are selected
///   - Whether the selection contains groups or locked objects
///   - The current tool mode
class CanvasContextMenu
{
public:
    CanvasContextMenu(Board& board, SelectionManager& selection, UndoRedoStack& undo_stack);

    // ── Menu Building ──────────────────────────────────────────────

    /// Build the context menu for the current state.
    /// @param click_pos  World-space position of the right-click.
    [[nodiscard]] auto build_menu(const Point2D& click_pos) const -> std::vector<ContextMenuGroup>;

    /// Build a menu specifically for an object (e.g. right-click on an object).
    [[nodiscard]] auto build_object_menu(ObjectId obj_id) const -> std::vector<ContextMenuGroup>;

    /// Build the empty-canvas menu (right-click on background).
    [[nodiscard]] auto build_background_menu(const Point2D& click_pos) const
        -> std::vector<ContextMenuGroup>;

    // ── Action Execution ───────────────────────────────────────────

    /// Execute a context menu action by ID.
    auto execute_action(const std::string& action_id) -> void;

    /// Whether an action is currently available.
    [[nodiscard]] auto is_action_available(const std::string& action_id) const -> bool;

    // ── Standard Action IDs ────────────────────────────────────────

    static constexpr const char* kActionCut = "canvas.cut";
    static constexpr const char* kActionCopy = "canvas.copy";
    static constexpr const char* kActionPaste = "canvas.paste";
    static constexpr const char* kActionDelete = "canvas.delete";
    static constexpr const char* kActionDuplicate = "canvas.duplicate";
    static constexpr const char* kActionSelectAll = "canvas.select_all";
    static constexpr const char* kActionGroup = "canvas.group";
    static constexpr const char* kActionUngroup = "canvas.ungroup";
    static constexpr const char* kActionLock = "canvas.lock";
    static constexpr const char* kActionUnlock = "canvas.unlock";
    static constexpr const char* kActionBringToFront = "canvas.bring_to_front";
    static constexpr const char* kActionSendToBack = "canvas.send_to_back";
    static constexpr const char* kActionBringForward = "canvas.bring_forward";
    static constexpr const char* kActionSendBackward = "canvas.send_backward";
    static constexpr const char* kActionAddConnector = "canvas.add_connector";
    static constexpr const char* kActionAddStickyNote = "canvas.add_sticky_note";
    static constexpr const char* kActionAddTextBox = "canvas.add_text_box";
    static constexpr const char* kActionZoomToFit = "canvas.zoom_to_fit";
    static constexpr const char* kActionZoomToSelection = "canvas.zoom_to_selection";

    /// Total number of registered actions.
    [[nodiscard]] auto action_count() const -> size_t;

    /// Callback for executing an action (wired to external command dispatch).
    using ActionHandler = std::function<void(const std::string& action_id)>;
    auto set_action_handler(ActionHandler handler) -> void;

private:
    Board& board_;
    SelectionManager& selection_;
    [[maybe_unused]] UndoRedoStack& undo_stack_;
    ActionHandler action_handler_;

    [[nodiscard]] auto build_clipboard_group() const -> ContextMenuGroup;
    [[nodiscard]] auto build_arrange_group() const -> ContextMenuGroup;
    [[nodiscard]] auto build_group_lock_group() const -> ContextMenuGroup;
    [[nodiscard]] auto build_create_group(const Point2D& pos) const -> ContextMenuGroup;
    [[nodiscard]] auto build_view_group() const -> ContextMenuGroup;
};

} // namespace markamp::canvas
