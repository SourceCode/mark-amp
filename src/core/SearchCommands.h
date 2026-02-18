#pragma once

/// @file SearchCommands.h
/// @brief V9 Phase 10 – Search command palette integration.
///
/// Header-only implementation of search-related commands:
///   - 11 search commands with keyboard shortcuts
///   - Context-aware command enablement
///   - Decoupled execution (returns success/failure, no UI dependency)

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// SearchCommand
// ────────────────────────────────────────────────────────────

/// A search-related command for the command palette.
struct SearchCommand
{
    /// Unique command identifier.
    std::string id;

    /// Display label.
    std::string label;

    /// Description for tooltip/help.
    std::string description;

    /// Keyboard shortcut (platform-specific).
    std::string shortcut;

    /// Category for grouping in the palette.
    std::string category;
};

// ────────────────────────────────────────────────────────────
// SearchCommandContext
// ────────────────────────────────────────────────────────────

/// Context for determining command enablement.
struct SearchCommandContext
{
    /// Whether there's an active search with results.
    bool has_active_search{false};

    /// Whether there are search results to navigate.
    bool has_results{false};

    /// Whether text is currently selected in the editor.
    bool has_selection{false};

    /// The current file path (empty if no file is open).
    std::string current_file_path;

    /// Whether the search panel is currently visible.
    bool search_panel_visible{false};

    /// Whether regex mode is active.
    bool regex_mode{false};

    /// Whether case-sensitive mode is active.
    bool case_sensitive{false};

    /// Whether whole-word mode is active.
    bool whole_word{false};
};

// ────────────────────────────────────────────────────────────
// SearchCommandResult
// ────────────────────────────────────────────────────────────

/// Result of command execution.
enum class SearchCommandResult : std::uint8_t
{
    kSuccess,  ///< Command executed successfully
    kNotFound, ///< Command ID not recognized
    kDisabled, ///< Command is disabled in current context
    kNoEffect  ///< Command ran but had no effect
};

// ────────────────────────────────────────────────────────────
// SearchCommandProvider
// ────────────────────────────────────────────────────────────

/// Provides search-related commands for the command palette.
class SearchCommandProvider
{
public:
    SearchCommandProvider() = default;

    /// Get all registered search commands.
    [[nodiscard]] auto get_commands() const -> std::vector<SearchCommand>
    {
        return {
            {"search.find", "Find", "Open find dialog in current document", "Cmd+F", "Search"},

            {"search.findInFiles",
             "Find in Files",
             "Search across all files in the workspace",
             "Cmd+Shift+F",
             "Search"},

            {"search.replace",
             "Find and Replace",
             "Open find and replace dialog",
             "Cmd+H",
             "Search"},

            {"search.nextMatch",
             "Next Match",
             "Navigate to the next search match",
             "Cmd+G",
             "Search"},

            {"search.previousMatch",
             "Previous Match",
             "Navigate to the previous search match",
             "Cmd+Shift+G",
             "Search"},

            {"search.selectAllMatches",
             "Select All Matches",
             "Select all occurrences of the current match",
             "Cmd+Shift+L",
             "Search"},

            {"search.toggleCaseSensitive",
             "Toggle Case Sensitivity",
             "Toggle case-sensitive search",
             "Cmd+Alt+C",
             "Search"},

            {"search.toggleRegex",
             "Toggle Regex",
             "Toggle regular expression search",
             "Cmd+Alt+R",
             "Search"},

            {"search.toggleWholeWord",
             "Toggle Whole Word",
             "Toggle whole word matching",
             "Cmd+Alt+W",
             "Search"},

            {"search.clearHistory",
             "Clear Search History",
             "Remove all entries from search history",
             "",
             "Search"},

            {"search.exportResults",
             "Export Search Results",
             "Export current search results to file",
             "",
             "Search"},
        };
    }

    /// Check if a command is enabled in the given context.
    [[nodiscard]] auto is_enabled(const std::string& command_id,
                                  const SearchCommandContext& context) const -> bool
    {
        // Commands always available
        if (command_id == "search.find" || command_id == "search.findInFiles" ||
            command_id == "search.replace" || command_id == "search.clearHistory")
        {
            return true;
        }

        // Navigation requires active results
        if (command_id == "search.nextMatch" || command_id == "search.previousMatch" ||
            command_id == "search.selectAllMatches")
        {
            return context.has_results;
        }

        // Toggle commands are always available when search panel is visible
        if (command_id == "search.toggleCaseSensitive" || command_id == "search.toggleRegex" ||
            command_id == "search.toggleWholeWord")
        {
            return context.search_panel_visible || context.has_active_search;
        }

        // Export requires results
        if (command_id == "search.exportResults")
        {
            return context.has_results;
        }

        return false;
    }

    /// Execute a command. Returns the result.
    /// This is a logical handler — actual side effects are performed by the caller.
    [[nodiscard]] auto execute_command(const std::string& command_id,
                                       const SearchCommandContext& context) const
        -> SearchCommandResult
    {
        // Verify the command exists
        auto commands = get_commands();
        bool found = false;
        for (const auto& cmd : commands)
        {
            if (cmd.id == command_id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return SearchCommandResult::kNotFound;
        }

        // Check enablement
        if (!is_enabled(command_id, context))
        {
            return SearchCommandResult::kDisabled;
        }

        // Command-specific logic
        if (command_id == "search.nextMatch" || command_id == "search.previousMatch")
        {
            if (!context.has_results)
            {
                return SearchCommandResult::kNoEffect;
            }
        }

        if (command_id == "search.selectAllMatches")
        {
            if (!context.has_results)
            {
                return SearchCommandResult::kNoEffect;
            }
        }

        return SearchCommandResult::kSuccess;
    }

    /// Get the keybinding for a command.
    [[nodiscard]] auto get_keybinding(const std::string& command_id) const -> std::string
    {
        auto commands = get_commands();
        for (const auto& cmd : commands)
        {
            if (cmd.id == command_id)
            {
                return cmd.shortcut;
            }
        }
        return {};
    }

    /// Get the number of registered commands.
    [[nodiscard]] auto command_count() const -> std::size_t
    {
        return get_commands().size();
    }
};

} // namespace markamp::core
