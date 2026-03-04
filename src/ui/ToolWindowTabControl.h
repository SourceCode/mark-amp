#pragma once

/**
 * @file ToolWindowTabControl.h
 * @brief Phase 29 Task 1: Tool window tab control model.
 *
 * Standardized tab actions (close, pin, reorder, split, context menu)
 * that mirror main tab conventions for consistency.
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Tool window tab action identifier.
enum class TabActionId : uint8_t
{
    kClose,
    kCloseOthers,
    kCloseRight,
    kCloseAll,
    kPin,
    kUnpin,
    kSplitRight,
    kSplitDown,
    kMaximize,
    kRestore,
    kMoveToLeft,
    kMoveToRight,
    kMoveToBottom,
    kCopyTitle,
    kRenameTab,
};

/// A single tool window tab action definition.
struct ToolWindowTabAction
{
    TabActionId action_id{TabActionId::kClose};
    std::string label;
    std::string shortcut;
    std::string icon_name;
    bool is_enabled{true};
    bool separator_after{false};
};

/// Callback invoked when a tab action is executed.
using TabActionCallback = std::function<void(const std::string& panel_id, TabActionId action)>;

/**
 * @brief Model for tool window tab control behaviors.
 *
 * Provides the standard set of tab actions (close, pin, reorder, split)
 * and supports dynamic enablement based on current panel state.
 */
class ToolWindowTabControl
{
public:
    /// Get the standard tab actions for a panel (with enablement applied).
    [[nodiscard]] static auto
    standard_actions(bool is_pinned, bool is_maximized, int tab_count, int tab_index)
        -> std::vector<ToolWindowTabAction>;

    /// Get all possible tab action IDs.
    [[nodiscard]] static auto all_action_ids() -> std::vector<TabActionId>;

    /// Get the human-readable label for an action.
    [[nodiscard]] static auto action_label(TabActionId action_id) -> std::string;

    /// Get the shortcut string for an action (may be empty).
    [[nodiscard]] static auto action_shortcut(TabActionId action_id) -> std::string;

    /// Get the icon name for an action.
    [[nodiscard]] static auto action_icon(TabActionId action_id) -> std::string;

    /// Get the total number of standard actions.
    [[nodiscard]] static auto action_count() -> int;

    /// Convert a TabActionId to its string representation.
    [[nodiscard]] static auto action_id_to_string(TabActionId action_id) -> std::string;
};

} // namespace markamp::ui
