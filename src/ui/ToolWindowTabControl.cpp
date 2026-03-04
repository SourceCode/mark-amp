#include "ToolWindowTabControl.h"

namespace markamp::ui
{

auto ToolWindowTabControl::standard_actions(bool is_pinned,
                                            bool is_maximized,
                                            int tab_count,
                                            int tab_index) -> std::vector<ToolWindowTabAction>
{
    std::vector<ToolWindowTabAction> actions;

    // Close group
    actions.push_back({TabActionId::kClose, "Close", "Ctrl+W", "close", true, false});
    actions.push_back(
        {TabActionId::kCloseOthers, "Close Others", "", "close-others", tab_count > 1, false});

    const bool has_tabs_to_right = tab_index < (tab_count - 1);
    actions.push_back({TabActionId::kCloseRight,
                       "Close to the Right",
                       "",
                       "close-right",
                       has_tabs_to_right,
                       false});
    actions.push_back({TabActionId::kCloseAll, "Close All", "", "close-all", tab_count > 0, true});

    // Pin group
    if (is_pinned)
    {
        actions.push_back({TabActionId::kUnpin, "Unpin", "", "unpin", true, true});
    }
    else
    {
        actions.push_back({TabActionId::kPin, "Pin", "", "pin", true, true});
    }

    // Split group
    actions.push_back(
        {TabActionId::kSplitRight, "Split Right", "", "split-horizontal", true, false});
    actions.push_back({TabActionId::kSplitDown, "Split Down", "", "split-vertical", true, true});

    // Maximize/Restore
    if (is_maximized)
    {
        actions.push_back({TabActionId::kRestore, "Restore", "", "restore", true, true});
    }
    else
    {
        actions.push_back({TabActionId::kMaximize, "Maximize Panel", "", "maximize", true, true});
    }

    // Move group
    actions.push_back(
        {TabActionId::kMoveToLeft, "Move to Left Side", "", "arrow-left", true, false});
    actions.push_back(
        {TabActionId::kMoveToRight, "Move to Right Side", "", "arrow-right", true, false});
    actions.push_back({TabActionId::kMoveToBottom, "Move to Bottom", "", "arrow-down", true, true});

    // Utility
    actions.push_back({TabActionId::kCopyTitle, "Copy Title", "", "copy", true, false});
    actions.push_back({TabActionId::kRenameTab, "Rename Tab", "F2", "rename", true, false});

    return actions;
}

auto ToolWindowTabControl::all_action_ids() -> std::vector<TabActionId>
{
    return {TabActionId::kClose,
            TabActionId::kCloseOthers,
            TabActionId::kCloseRight,
            TabActionId::kCloseAll,
            TabActionId::kPin,
            TabActionId::kUnpin,
            TabActionId::kSplitRight,
            TabActionId::kSplitDown,
            TabActionId::kMaximize,
            TabActionId::kRestore,
            TabActionId::kMoveToLeft,
            TabActionId::kMoveToRight,
            TabActionId::kMoveToBottom,
            TabActionId::kCopyTitle,
            TabActionId::kRenameTab};
}

auto ToolWindowTabControl::action_label(TabActionId action_id) -> std::string
{
    switch (action_id)
    {
        case TabActionId::kClose:
            return "Close";
        case TabActionId::kCloseOthers:
            return "Close Others";
        case TabActionId::kCloseRight:
            return "Close to the Right";
        case TabActionId::kCloseAll:
            return "Close All";
        case TabActionId::kPin:
            return "Pin";
        case TabActionId::kUnpin:
            return "Unpin";
        case TabActionId::kSplitRight:
            return "Split Right";
        case TabActionId::kSplitDown:
            return "Split Down";
        case TabActionId::kMaximize:
            return "Maximize Panel";
        case TabActionId::kRestore:
            return "Restore";
        case TabActionId::kMoveToLeft:
            return "Move to Left Side";
        case TabActionId::kMoveToRight:
            return "Move to Right Side";
        case TabActionId::kMoveToBottom:
            return "Move to Bottom";
        case TabActionId::kCopyTitle:
            return "Copy Title";
        case TabActionId::kRenameTab:
            return "Rename Tab";
    }
    return "";
}

auto ToolWindowTabControl::action_shortcut(TabActionId action_id) -> std::string
{
    switch (action_id)
    {
        case TabActionId::kClose:
            return "Ctrl+W";
        case TabActionId::kRenameTab:
            return "F2";
        default:
            return "";
    }
}

auto ToolWindowTabControl::action_icon(TabActionId action_id) -> std::string
{
    switch (action_id)
    {
        case TabActionId::kClose:
            return "close";
        case TabActionId::kCloseOthers:
            return "close-others";
        case TabActionId::kCloseRight:
            return "close-right";
        case TabActionId::kCloseAll:
            return "close-all";
        case TabActionId::kPin:
            return "pin";
        case TabActionId::kUnpin:
            return "unpin";
        case TabActionId::kSplitRight:
            return "split-horizontal";
        case TabActionId::kSplitDown:
            return "split-vertical";
        case TabActionId::kMaximize:
            return "maximize";
        case TabActionId::kRestore:
            return "restore";
        case TabActionId::kMoveToLeft:
            return "arrow-left";
        case TabActionId::kMoveToRight:
            return "arrow-right";
        case TabActionId::kMoveToBottom:
            return "arrow-down";
        case TabActionId::kCopyTitle:
            return "copy";
        case TabActionId::kRenameTab:
            return "rename";
    }
    return "";
}

auto ToolWindowTabControl::action_count() -> int
{
    return 15;
}

auto ToolWindowTabControl::action_id_to_string(TabActionId action_id) -> std::string
{
    switch (action_id)
    {
        case TabActionId::kClose:
            return "close";
        case TabActionId::kCloseOthers:
            return "close_others";
        case TabActionId::kCloseRight:
            return "close_right";
        case TabActionId::kCloseAll:
            return "close_all";
        case TabActionId::kPin:
            return "pin";
        case TabActionId::kUnpin:
            return "unpin";
        case TabActionId::kSplitRight:
            return "split_right";
        case TabActionId::kSplitDown:
            return "split_down";
        case TabActionId::kMaximize:
            return "maximize";
        case TabActionId::kRestore:
            return "restore";
        case TabActionId::kMoveToLeft:
            return "move_to_left";
        case TabActionId::kMoveToRight:
            return "move_to_right";
        case TabActionId::kMoveToBottom:
            return "move_to_bottom";
        case TabActionId::kCopyTitle:
            return "copy_title";
        case TabActionId::kRenameTab:
            return "rename_tab";
    }
    return "";
}

} // namespace markamp::ui
