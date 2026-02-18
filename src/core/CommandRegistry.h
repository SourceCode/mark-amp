/// @file CommandRegistry.h
/// @brief V9 Phase 36 Tasks 1, 2, 3, 4, 6 — Centralized command registry.
///
/// Provides a single registration point for all commands in the application.
/// Each command has metadata (ID, title, category, description, icon, shortcut,
/// when-clause) and an execute callback. The registry supports:
///   - Duplicate-ID detection and warning
///   - Category-based grouping
///   - When-clause context filtering via WhenClauseEvaluator
///   - Recently-used tracking with persistence
///   - Fuzzy search over command titles and categories
#pragma once

#include "ContextKeyService.h"
#include "WhenClause.h"

#include <chrono>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// CommandEntry — metadata for a single registered command
// ============================================================================

/// Sources from which a command can be invoked.
enum class CommandSource
{
    kPalette,   // Command palette selection
    kShortcut,  // Keyboard shortcut
    kMenu,      // Application menu
    kExtension, // Extension API
    kInternal   // Programmatic invocation
};

/// Describes a single command registered in the system.
struct CommandEntry
{
    std::string id;          // Unique command ID, e.g. "editor.action.formatDocument"
    std::string title;       // Display name, e.g. "Format Document"
    std::string category;    // Grouping, e.g. "Editor", "File", "View"
    std::string description; // Tooltip / detailed description
    std::string shortcut;    // Shortcut hint, e.g. "Cmd+Shift+F"
    std::string when_clause; // VS Code-style when expression (empty = always)
    std::string icon;        // Optional icon identifier

    /// Callback to execute the command. Returns true on success.
    std::function<bool()> execute_fn;

    /// Check whether the command is available in the given context.
    [[nodiscard]] auto is_available(const ContextKeyService& context) const -> bool
    {
        if (when_clause.empty())
        {
            return true;
        }
        return WhenClauseEvaluator::matches(when_clause, context);
    }
};

// ============================================================================
// CommandUsageRecord — tracks individual usage of a command
// ============================================================================

struct CommandUsageRecord
{
    std::string command_id;
    std::chrono::steady_clock::time_point last_used;
    int use_count{0};
};

// ============================================================================
// CommandRegistry — central registration and lookup
// ============================================================================

/// Centralized command registry. All commands in the application register here.
///
/// Usage:
/// ```cpp
/// CommandRegistry registry;
/// CommandEntry entry;
/// entry.id = "file.save";
/// entry.title = "Save";
/// entry.category = "File";
/// entry.execute_fn = []() -> bool { /* save logic */ return true; };
/// registry.register_command(std::move(entry));
///
/// auto* cmd = registry.get_command("file.save");
/// if (cmd) cmd->execute_fn();
/// ```
class CommandRegistry
{
public:
    CommandRegistry() = default;

    // ── Registration ──

    /// Register a command. Logs a warning and replaces if ID already exists.
    void register_command(CommandEntry entry);

    /// Remove a command by ID. Returns true if found and removed.
    auto unregister_command(const std::string& command_id) -> bool;

    /// Register multiple commands at once.
    void register_commands(std::vector<CommandEntry> entries);

    // ── Lookup ──

    /// Get a command by ID. Returns nullptr if not found.
    [[nodiscard]] auto get_command(const std::string& command_id) const -> const CommandEntry*;

    /// Get all registered commands.
    [[nodiscard]] auto all_commands() const -> std::vector<const CommandEntry*>;

    /// Get commands filtered to a specific category.
    [[nodiscard]] auto commands_for_category(const std::string& category) const
        -> std::vector<const CommandEntry*>;

    /// Get commands that are currently active (when-clause evaluates to true).
    [[nodiscard]] auto active_commands(const ContextKeyService& context) const
        -> std::vector<const CommandEntry*>;

    /// Get all unique category names.
    [[nodiscard]] auto get_categories() const -> std::vector<std::string>;

    /// Total registered command count.
    [[nodiscard]] auto command_count() const -> std::size_t;

    // ── Execution ──

    /// Execute a command by ID. Returns false if not found or execution fails.
    auto execute_command(const std::string& command_id,
                         CommandSource source = CommandSource::kInternal) -> bool;

    // ── Fuzzy Search ──

    /// Search result with relevance score.
    struct SearchResult
    {
        const CommandEntry* entry{nullptr};
        int score{0}; // Higher = better match
    };

    /// Fuzzy search over command titles and categories.
    /// Returns results sorted by score descending, limited to max_results.
    [[nodiscard]] auto search(const std::string& query, int max_results = 50) const
        -> std::vector<SearchResult>;

    // ── Usage Tracking ──

    /// Record that a command was used (called automatically by execute_command).
    void record_usage(const std::string& command_id);

    /// Get the most recently used commands (up to count).
    [[nodiscard]] auto get_recently_used(int count = 20) const -> std::vector<CommandUsageRecord>;

    /// Get most frequently used commands.
    [[nodiscard]] auto get_most_frequent(int count = 10) const -> std::vector<CommandUsageRecord>;

    /// Clear usage history.
    void clear_history();

    /// Export usage history as JSON string.
    [[nodiscard]] auto export_history_json() const -> std::string;

    /// Import usage history from JSON string.
    void import_history_json(const std::string& json_data);

    /// Check if a command ID is already registered.
    [[nodiscard]] auto has_command(const std::string& command_id) const -> bool;

private:
    /// All registered commands, keyed by ID.
    std::unordered_map<std::string, CommandEntry> commands_;

    /// Insertion order for deterministic iteration.
    std::vector<std::string> insertion_order_;

    /// Usage tracking data.
    std::unordered_map<std::string, CommandUsageRecord> usage_history_;

    /// Fuzzy score a candidate against a query (higher = better, 0 = no match).
    static auto fuzzy_score(const std::string& query, const std::string& candidate) -> int;
};

} // namespace markamp::core
