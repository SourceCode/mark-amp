// ============================================================================
// File: src/canvas/CanvasContextMenu.cpp
// Phase 11: Canvas Workbench Shell — context menu implementation
// ============================================================================
#include "CanvasContextMenu.h"

#include "canvas/Board.h"
#include "canvas/SelectionManager.h"
#include "canvas/UndoRedoStack.h"

#include <algorithm>
#include <utility>

namespace markamp::canvas
{

CanvasContextMenu::CanvasContextMenu(Board& board,
                                     SelectionManager& selection,
                                     UndoRedoStack& undo_stack)
    : board_(board)
    , selection_(selection)
    , undo_stack_(undo_stack)
{
}

// ── Menu Building ──────────────────────────────────────────────────

auto CanvasContextMenu::build_menu(const Point2D& click_pos) const -> std::vector<ContextMenuGroup>
{
    if (selection_.selection_count() > 0)
    {
        return build_object_menu(selection_.selected_ids().front());
    }
    return build_background_menu(click_pos);
}

auto CanvasContextMenu::build_object_menu(ObjectId /*obj_id*/) const
    -> std::vector<ContextMenuGroup>
{
    std::vector<ContextMenuGroup> groups;
    groups.push_back(build_clipboard_group());
    groups.push_back(build_arrange_group());
    groups.push_back(build_group_lock_group());
    groups.push_back(build_view_group());
    return groups;
}

auto CanvasContextMenu::build_background_menu(const Point2D& pos) const
    -> std::vector<ContextMenuGroup>
{
    std::vector<ContextMenuGroup> groups;

    // Paste action
    ContextMenuGroup clipboard_grp;
    clipboard_grp.group_name = "Clipboard";
    clipboard_grp.actions.push_back(
        {kActionPaste, "Paste", "Ctrl+V", "clipboard", true, false, false});
    groups.push_back(std::move(clipboard_grp));

    // Create group
    groups.push_back(build_create_group(pos));

    // View group
    groups.push_back(build_view_group());

    return groups;
}

// ── Action Execution ───────────────────────────────────────────────

auto CanvasContextMenu::execute_action(const std::string& action_id) -> void
{
    if (action_handler_)
    {
        action_handler_(action_id);
    }
}

auto CanvasContextMenu::is_action_available(const std::string& action_id) const -> bool
{
    const auto count = selection_.selection_count();

    if (action_id == kActionCut || action_id == kActionCopy || action_id == kActionDelete ||
        action_id == kActionDuplicate)
    {
        return count > 0;
    }
    if (action_id == kActionGroup)
    {
        return count >= 2;
    }
    if (action_id == kActionSelectAll)
    {
        return board_.object_count() > 0;
    }
    if (action_id == kActionZoomToSelection)
    {
        return count > 0;
    }
    // Available by default
    return true;
}

auto CanvasContextMenu::action_count() const -> size_t
{
    return 19; // Total number of kAction* constants
}

auto CanvasContextMenu::set_action_handler(ActionHandler handler) -> void
{
    action_handler_ = std::move(handler);
}

// ── Private Builders ───────────────────────────────────────────────

auto CanvasContextMenu::build_clipboard_group() const -> ContextMenuGroup
{
    ContextMenuGroup grp;
    grp.group_name = "Clipboard";

    const bool has_sel = selection_.selection_count() > 0;
    grp.actions.push_back({kActionCut, "Cut", "Ctrl+X", "scissors", has_sel, false, false});
    grp.actions.push_back({kActionCopy, "Copy", "Ctrl+C", "copy", has_sel, false, false});
    grp.actions.push_back({kActionPaste, "Paste", "Ctrl+V", "clipboard", true, false, false});
    grp.actions.push_back(
        {kActionDuplicate, "Duplicate", "Ctrl+D", "copy-plus", has_sel, false, false});
    grp.actions.push_back({"", "", "", "", false, true, false}); // separator
    grp.actions.push_back({kActionDelete, "Delete", "Del", "trash", has_sel, false, true});

    return grp;
}

auto CanvasContextMenu::build_arrange_group() const -> ContextMenuGroup
{
    ContextMenuGroup grp;
    grp.group_name = "Arrange";

    const bool has_sel = selection_.selection_count() > 0;
    grp.actions.push_back({kActionBringToFront,
                           "Bring to Front",
                           "Ctrl+]",
                           "arrow-up-to-line",
                           has_sel,
                           false,
                           false});
    grp.actions.push_back(
        {kActionSendToBack, "Send to Back", "Ctrl+[", "arrow-down-to-line", has_sel, false, false});
    grp.actions.push_back(
        {kActionBringForward, "Bring Forward", "]", "arrow-up", has_sel, false, false});
    grp.actions.push_back(
        {kActionSendBackward, "Send Backward", "[", "arrow-down", has_sel, false, false});

    return grp;
}

auto CanvasContextMenu::build_group_lock_group() const -> ContextMenuGroup
{
    ContextMenuGroup grp;
    grp.group_name = "Grouping";

    const auto count = selection_.selection_count();
    grp.actions.push_back({kActionGroup, "Group", "Ctrl+G", "group", count >= 2, false, false});
    grp.actions.push_back(
        {kActionUngroup, "Ungroup", "Ctrl+Shift+G", "ungroup", count > 0, false, false});
    grp.actions.push_back({"", "", "", "", false, true, false}); // separator
    grp.actions.push_back({kActionLock, "Lock", "Ctrl+L", "lock", count > 0, false, false});
    grp.actions.push_back(
        {kActionUnlock, "Unlock", "Ctrl+Shift+L", "unlock", count > 0, false, false});

    return grp;
}

auto CanvasContextMenu::build_create_group(const Point2D& /*pos*/) const -> ContextMenuGroup
{
    ContextMenuGroup grp;
    grp.group_name = "Insert";

    grp.actions.push_back(
        {kActionAddStickyNote, "Sticky Note", "", "sticky-note", true, false, false});
    grp.actions.push_back({kActionAddTextBox, "Text Box", "T", "text-box", true, false, false});
    grp.actions.push_back({kActionAddConnector, "Connector", "", "git-merge", true, false, false});

    return grp;
}

auto CanvasContextMenu::build_view_group() const -> ContextMenuGroup
{
    ContextMenuGroup grp;
    grp.group_name = "View";

    grp.actions.push_back({kActionSelectAll,
                           "Select All",
                           "Ctrl+A",
                           "check-square",
                           board_.object_count() > 0,
                           false,
                           false});
    grp.actions.push_back(
        {kActionZoomToFit, "Zoom to Fit", "Ctrl+0", "maximize", true, false, false});
    grp.actions.push_back({kActionZoomToSelection,
                           "Zoom to Selection",
                           "Ctrl+Shift+0",
                           "focus",
                           selection_.selection_count() > 0,
                           false,
                           false});

    return grp;
}

} // namespace markamp::canvas
