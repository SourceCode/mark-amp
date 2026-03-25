#pragma once

/**
 * @file NavigationCommandSet.h
 * @brief Phase 30 Task 1: Unified navigation command set.
 *
 * Standardized back, forward, recent locations, quick switch,
 * go-to-file, and go-to-symbol commands that work across all surfaces.
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Navigation command scope.
enum class NavigationScope : uint8_t
{
    kGlobal, ///< Works regardless of active surface
    kPane,   ///< Applies to the currently focused pane
    kEditor, ///< Only available in editor surfaces
};

/// A single navigation command definition.
struct NavigationCommand
{
    std::string command_id;
    std::string label;
    std::string shortcut;
    std::string icon_name;
    NavigationScope scope{NavigationScope::kGlobal};
    bool is_enabled{true};

    /// Human-readable scope label.
    [[nodiscard]] auto scope_label() const -> std::string;
};

/// Callback invoked when a navigation command is executed.
using NavigationCommandCallback = std::function<void(const std::string& command_id)>;

/**
 * @brief Unified navigation command registry.
 *
 * Provides the standard set of navigation commands that work across
 * all surfaces (editor, canvas, graph, panels).
 */
class NavigationCommandSet
{
public:
    /// Get all standard navigation commands.
    [[nodiscard]] static auto standard_commands() -> std::vector<NavigationCommand>;

    /// Get commands filtered by scope.
    [[nodiscard]] static auto commands_for_scope(NavigationScope scope)
        -> std::vector<NavigationCommand>;

    /// Get a specific command by ID.
    [[nodiscard]] static auto find_command(const std::string& command_id)
        -> const NavigationCommand*;

    /// Get the total number of standard navigation commands.
    [[nodiscard]] static auto command_count() -> int;

    /// Get all unique scope values used by commands.
    [[nodiscard]] static auto available_scopes() -> std::vector<NavigationScope>;
};

} // namespace markamp::ui
