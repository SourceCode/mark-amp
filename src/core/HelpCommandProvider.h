/// @file HelpCommandProvider.h
/// @brief V9 Phase 35 Tasks 4, 18 — Help commands for the command palette.
///
/// Header-only command provider that registers 8 help-related commands
/// following the NavigationCommandProvider pattern: Getting Started,
/// Keyboard Shortcuts, Markdown Reference, What's New, Help Search,
/// Report Issue, Open Playground, Command Reference.

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// HelpCommand — a single help command definition
// ============================================================================

/// Describes a registered help command.
struct HelpCommand
{
    std::string id;          ///< Unique command ID, e.g. "help.gettingStarted"
    std::string label;       ///< Display label
    std::string description; ///< Brief description
    std::string keybinding;  ///< Default keybinding
    std::string category{"Help"};

    /// Callback to execute the command.
    std::function<bool()> execute_fn;
};

// ============================================================================
// HelpCommandProvider
// ============================================================================

/// Provides help commands for the command palette.
///
/// Registers 8 built-in help commands with keybindings and execution logic.
///
/// Usage:
/// ```cpp
/// HelpCommandProvider provider;
/// auto cmds = provider.all_commands();
/// bool ok = provider.execute_command("help.gettingStarted");
/// ```
class HelpCommandProvider
{
public:
    HelpCommandProvider()
    {
        register_built_in_commands();
    }

    /// Get all registered commands.
    [[nodiscard]] auto all_commands() const -> const std::vector<HelpCommand>&
    {
        return commands_;
    }

    /// Get a specific command by ID.
    [[nodiscard]] auto get_command(const std::string& command_id) const -> const HelpCommand*
    {
        for (const auto& cmd : commands_)
        {
            if (cmd.id == command_id)
            {
                return &cmd;
            }
        }
        return nullptr;
    }

    /// Execute a command by ID.
    auto execute_command(const std::string& command_id) -> bool
    {
        const auto* cmd = get_command(command_id);
        if (cmd != nullptr && cmd->execute_fn)
        {
            return cmd->execute_fn();
        }
        return false;
    }

    /// Get the keybinding for a command.
    [[nodiscard]] auto get_keybinding(const std::string& command_id) const -> std::string
    {
        const auto* cmd = get_command(command_id);
        if (cmd != nullptr)
        {
            return cmd->keybinding;
        }
        return {};
    }

    /// Count of registered commands.
    [[nodiscard]] auto command_count() const -> std::size_t
    {
        return commands_.size();
    }

    /// Register an additional custom command.
    void register_command(HelpCommand cmd)
    {
        commands_.push_back(std::move(cmd));
    }

private:
    std::vector<HelpCommand> commands_;

    void register_built_in_commands()
    {
        // 1. Getting Started
        {
            HelpCommand cmd;
            cmd.id = "help.gettingStarted";
            cmd.label = "Help: Getting Started";
            cmd.description = "Open the Getting Started walkthrough";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool
            {
                // Dispatched by the main app to show the walkthrough panel
                return true;
            };
            commands_.push_back(std::move(cmd));
        }

        // 2. Keyboard Shortcuts
        {
            HelpCommand cmd;
            cmd.id = "help.keyboardShortcuts";
            cmd.label = "Help: Keyboard Shortcuts";
            cmd.description = "Show all keyboard shortcuts";
            cmd.keybinding = "F1";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }

        // 3. Markdown Reference
        {
            HelpCommand cmd;
            cmd.id = "help.markdownReference";
            cmd.label = "Help: Markdown Reference";
            cmd.description = "Open the Markdown syntax reference";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }

        // 4. What's New
        {
            HelpCommand cmd;
            cmd.id = "help.whatsNew";
            cmd.label = "Help: What's New";
            cmd.description = "Show release notes for the current version";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }

        // 5. Search Help
        {
            HelpCommand cmd;
            cmd.id = "help.search";
            cmd.label = "Help: Search Help";
            cmd.description = "Search across all help articles";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }

        // 6. Report Issue
        {
            HelpCommand cmd;
            cmd.id = "help.reportIssue";
            cmd.label = "Help: Report Issue";
            cmd.description = "Report a bug or request a feature";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }

        // 7. Open Playground
        {
            HelpCommand cmd;
            cmd.id = "help.openPlayground";
            cmd.label = "Help: Open Playground";
            cmd.description = "Open a sandbox with sample content for experimenting";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }

        // 8. Command Reference
        {
            HelpCommand cmd;
            cmd.id = "help.commandReference";
            cmd.label = "Help: Command Reference";
            cmd.description = "List all commands alphabetically with descriptions and shortcuts";
            cmd.keybinding = "";
            cmd.execute_fn = []() -> bool { return true; };
            commands_.push_back(std::move(cmd));
        }
    }
};

} // namespace markamp::core
