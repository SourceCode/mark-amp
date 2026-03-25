/// @file ActionManifest.h
/// @brief V24 P04-T01: Single, authoritative action manifest for the shell.
///
/// All visible controls (menus, toolbars, context actions, palette items)
/// derive from this manifest.  Actions declare their ID, label, icon,
/// keybinding, enablement context, and handler.  This eliminates divergent
/// behavior between UI surfaces.
#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Action category for grouping.
enum class ActionCategory
{
    kFile,
    kEdit,
    kView,
    kNavigate,
    kSearch,
    kTerminal,
    kDebug,
    kHelp,
    kSourceControl,
    kExtension,
    kCustom
};

/// Returns a label for an ActionCategory.
[[nodiscard]] constexpr auto action_category_label(ActionCategory c) noexcept -> const char*
{
    switch (c) {
        case ActionCategory::kFile:          return "File";
        case ActionCategory::kEdit:          return "Edit";
        case ActionCategory::kView:          return "View";
        case ActionCategory::kNavigate:      return "Navigate";
        case ActionCategory::kSearch:        return "Search";
        case ActionCategory::kTerminal:      return "Terminal";
        case ActionCategory::kDebug:         return "Debug";
        case ActionCategory::kHelp:          return "Help";
        case ActionCategory::kSourceControl: return "SourceControl";
        case ActionCategory::kExtension:     return "Extension";
        case ActionCategory::kCustom:        return "Custom";
    }
    return "Unknown";
}

/// Handler invoked when action executes.
using ActionHandler = std::function<void()>;

/// Enablement predicate evaluated before action execution.
using EnablementPredicate = std::function<bool()>;

/// A single action in the manifest.
struct ManifestAction
{
    std::string action_id;                       ///< Unique id (e.g. "file.new")
    std::string label;                           ///< Display label
    std::string icon;                            ///< Icon identifier
    std::optional<std::string> keybinding;       ///< Keyboard shortcut
    ActionCategory category{ActionCategory::kCustom};
    std::string when_clause;                     ///< Context key expression for enablement
    ActionHandler handler;                       ///< The actual handler
    EnablementPredicate enablement;              ///< Optional runtime predicate
    bool is_visible{true};                       ///< Whether to show in UI
    int sort_order{100};                         ///< Sort within category

    [[nodiscard]] auto has_handler() const noexcept -> bool
    {
        return handler != nullptr;
    }

    [[nodiscard]] auto has_keybinding() const noexcept -> bool
    {
        return keybinding.has_value() && !keybinding->empty();
    }

    [[nodiscard]] auto is_enabled() const -> bool
    {
        return !enablement || enablement();
    }
};

/// Execution result from calling an action.
struct ActionExecutionResult
{
    bool success{false};
    std::string error_message;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// The authoritative action manifest for the shell.
class ActionManifest
{
public:
    ActionManifest() = default;

    /// Register an action. Returns false if ID already exists.
    [[nodiscard]] auto register_action(ManifestAction action) -> bool;

    /// Execute an action by ID.
    [[nodiscard]] auto execute(const std::string& action_id) -> ActionExecutionResult;

    /// Check if an action is enabled.
    [[nodiscard]] auto is_enabled(const std::string& action_id) const -> bool;

    /// Get action by ID. Returns nullptr if not found.
    [[nodiscard]] auto get_action(const std::string& action_id) const -> const ManifestAction*;

    /// All actions.
    [[nodiscard]] auto all_actions() const -> std::vector<const ManifestAction*>;

    /// Actions by category.
    [[nodiscard]] auto actions_by_category(ActionCategory cat) const
        -> std::vector<const ManifestAction*>;

    /// Total action count.
    [[nodiscard]] auto action_count() const noexcept -> int
    {
        return static_cast<int>(actions_.size());
    }

    /// Clear all actions.
    void clear();

private:
    std::unordered_map<std::string, ManifestAction> actions_;
    std::vector<std::string> insertion_order_;
};

} // namespace markamp::core
