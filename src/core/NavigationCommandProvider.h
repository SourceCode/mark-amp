/// @file NavigationCommandProvider.h
/// @brief V9 Phase 17 Tasks 14–15 — Command palette integration for navigation.
///
/// Header-only. Provides 12 navigation commands for the command palette
/// with execution, enabled-state, and keybinding support.

#pragma once

#include "SurfaceLink.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Navigation command IDs
// ============================================================================

/// Well-known navigation command identifiers.
namespace nav_commands
{
inline constexpr const char* kGoBack = "navigation.goBack";
inline constexpr const char* kGoForward = "navigation.goForward";
inline constexpr const char* kGoToDefinition = "navigation.goToDefinition";
inline constexpr const char* kPeekDefinition = "navigation.peekDefinition";
inline constexpr const char* kShowBrokenLinks = "navigation.showBrokenLinks";
inline constexpr const char* kFollowLink = "navigation.followLink";
inline constexpr const char* kOpenInSide = "navigation.openInSide";
inline constexpr const char* kNavigateToCanvas = "navigation.navigateToCanvas";
inline constexpr const char* kNavigateToGraph = "navigation.navigateToGraph";
inline constexpr const char* kNavigateToNotebook = "navigation.navigateToNotebook";
inline constexpr const char* kCopyDeepLink = "navigation.copyDeepLink";
inline constexpr const char* kGoToHeading = "navigation.goToHeading";
} // namespace nav_commands

// ============================================================================
// NavigationCommandContext — execution context for commands
// ============================================================================

/// Context passed to a navigation command during execution.
struct NavigationCommandContext
{
    SurfaceKind active_surface{SurfaceKind::kEditor};
    std::string active_document;
    int cursor_line{-1};
    int cursor_character{-1};
    std::string selected_text;
    bool has_history_back{false};
    bool has_history_forward{false};
    bool has_link_under_cursor{false};
};

// ============================================================================
// NavigationCommand — a single command definition
// ============================================================================

/// Describes a registered navigation command.
struct NavigationCommand
{
    std::string id;
    std::string label;
    std::string description;
    std::string keybinding;
    std::string category{"Navigation"};

    /// Callback to check if the command is enabled in the given context.
    std::function<bool(const NavigationCommandContext&)> is_enabled_fn;

    /// Callback to execute the command.
    std::function<bool(const NavigationCommandContext&)> execute_fn;
};

// ============================================================================
// NavigationCommandProvider
// ============================================================================

/// Provides navigation commands for the command palette.
///
/// Registers 12 built-in navigation commands with keybindings,
/// enabled-state checks, and execution logic.
///
/// Usage:
/// ```cpp
/// NavigationCommandProvider provider;
/// auto cmds = provider.all_commands();
/// bool ok = provider.execute_command("navigation.goBack", context);
/// ```
class NavigationCommandProvider
{
public:
    NavigationCommandProvider()
    {
        register_built_in_commands();
    }

    /// Get all registered commands.
    [[nodiscard]] auto all_commands() const -> std::vector<NavigationCommand>
    {
        std::vector<NavigationCommand> result;
        result.reserve(commands_.size());
        for (const auto& [cmd_id, cmd] : commands_)
        {
            result.push_back(cmd);
        }
        return result;
    }

    /// Get a specific command by ID.
    [[nodiscard]] auto get_command(const std::string& command_id) const -> const NavigationCommand*
    {
        auto iter = commands_.find(command_id);
        if (iter != commands_.end())
        {
            return &iter->second;
        }
        return nullptr;
    }

    /// Execute a command by ID.
    [[nodiscard]] auto execute_command(const std::string& command_id,
                                       const NavigationCommandContext& context) const -> bool
    {
        auto iter = commands_.find(command_id);
        if (iter == commands_.end())
        {
            return false;
        }

        if (!iter->second.is_enabled_fn || !iter->second.is_enabled_fn(context))
        {
            return false;
        }

        if (iter->second.execute_fn)
        {
            return iter->second.execute_fn(context);
        }

        return false;
    }

    /// Check if a command is currently enabled.
    [[nodiscard]] auto is_enabled(const std::string& command_id,
                                  const NavigationCommandContext& context) const -> bool
    {
        auto iter = commands_.find(command_id);
        if (iter == commands_.end())
        {
            return false;
        }

        if (!iter->second.is_enabled_fn)
        {
            return true; // Default: enabled
        }

        return iter->second.is_enabled_fn(context);
    }

    /// Get the keybinding for a command.
    [[nodiscard]] auto get_keybinding(const std::string& command_id) const -> std::string
    {
        auto iter = commands_.find(command_id);
        if (iter != commands_.end())
        {
            return iter->second.keybinding;
        }
        return {};
    }

    /// Count of registered commands.
    [[nodiscard]] auto command_count() const -> std::size_t
    {
        return commands_.size();
    }

    /// Register an additional custom command.
    void register_command(NavigationCommand command)
    {
        auto cmd_id = command.id;
        commands_[cmd_id] = std::move(command);
    }

private:
    std::unordered_map<std::string, NavigationCommand> commands_;

    void register_built_in_commands()
    {
        // 1. Go Back
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kGoBack;
            cmd.label = "Go Back";
            cmd.description = "Navigate to the previous location in history";
            cmd.keybinding = "Alt+Left";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return ctx.has_history_back; };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/)
            {
                return true; // Placeholder — actual navigation handled by NavigationService
            };
            commands_[cmd.id] = std::move(cmd);
        }

        // 2. Go Forward
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kGoForward;
            cmd.label = "Go Forward";
            cmd.description = "Navigate to the next location in history";
            cmd.keybinding = "Alt+Right";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return ctx.has_history_forward; };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 3. Go to Definition
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kGoToDefinition;
            cmd.label = "Go to Definition";
            cmd.description = "Navigate to the definition of the symbol under cursor";
            cmd.keybinding = "F12";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return ctx.has_link_under_cursor; };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 4. Peek Definition
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kPeekDefinition;
            cmd.label = "Peek Definition";
            cmd.description = "Show inline preview of the definition under cursor";
            cmd.keybinding = "Alt+F12";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return ctx.has_link_under_cursor; };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 5. Show Broken Links
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kShowBrokenLinks;
            cmd.label = "Show Broken Links";
            cmd.description = "Scan the current document for broken links";
            cmd.keybinding = "Ctrl+Shift+B";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return !ctx.active_document.empty(); };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 6. Follow Link
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kFollowLink;
            cmd.label = "Follow Link";
            cmd.description = "Open the link under cursor";
            cmd.keybinding = "Ctrl+Click";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return ctx.has_link_under_cursor; };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 7. Open in Side Panel
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kOpenInSide;
            cmd.label = "Open in Side Panel";
            cmd.description = "Open the link target in the side panel";
            cmd.keybinding = "Ctrl+Enter";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return ctx.has_link_under_cursor; };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 8. Navigate to Canvas
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kNavigateToCanvas;
            cmd.label = "Open in Canvas";
            cmd.description = "Open the current document in the canvas surface";
            cmd.keybinding = "Ctrl+Shift+C";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return !ctx.active_document.empty(); };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 9. Navigate to Graph
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kNavigateToGraph;
            cmd.label = "Open in Graph";
            cmd.description = "Show the current document in the graph view";
            cmd.keybinding = "Ctrl+Shift+G";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return !ctx.active_document.empty(); };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 10. Navigate to Notebook
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kNavigateToNotebook;
            cmd.label = "Open in Notebook";
            cmd.description = "Open the current document in notebook view";
            cmd.keybinding = "Ctrl+Shift+N";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return !ctx.active_document.empty(); };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 11. Copy Deep Link
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kCopyDeepLink;
            cmd.label = "Copy Deep Link";
            cmd.description = "Copy a markamp:// deep link to the current location";
            cmd.keybinding = "Ctrl+Shift+L";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return !ctx.active_document.empty(); };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }

        // 12. Go to Heading
        {
            NavigationCommand cmd;
            cmd.id = nav_commands::kGoToHeading;
            cmd.label = "Go to Heading...";
            cmd.description = "Jump to a heading in the current document";
            cmd.keybinding = "Ctrl+Shift+H";
            cmd.is_enabled_fn = [](const NavigationCommandContext& ctx)
            { return !ctx.active_document.empty(); };
            cmd.execute_fn = [](const NavigationCommandContext& /*ctx*/) { return true; };
            commands_[cmd.id] = std::move(cmd);
        }
    }
};

} // namespace markamp::core
