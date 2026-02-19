#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Rich command metadata (Phase 10 Task 1).
struct CommandMetadata
{
    std::string command_id;           ///< Unique command ID
    std::string label;                ///< Display name
    std::string category;             ///< Category grouping (e.g., "File", "View", "Editor")
    std::string shortcut;             ///< Keyboard shortcut display string
    std::string description;          ///< Brief description for preview pane
    std::string source;               ///< Origin: "builtin", "extension", "user"
    std::vector<std::string> aliases; ///< Alternative search terms
    bool is_destructive{false};       ///< Requires confirmation before execution
    bool is_pinned{false};            ///< User-pinned to top
};

/// Ranked search result with score.
struct CommandMatch
{
    int index{0}; ///< Index into the original command list
    int score{0}; ///< Ranking score (higher = better match)
};

/// Testable model for the Command Palette (Phase 10).
///
/// Encapsulates:
/// - Rich metadata with category, scope, shortcut, source
/// - Fuzzy search with alias support and verb-first ranking
/// - Preview pane data for destructive commands
/// - MRU (Most Recently Used) history
/// - Pinned commands
class CommandPaletteModel
{
public:
    /// Register a command.
    void add_command(CommandMetadata command);

    /// Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<CommandMetadata>&;

    /// Total command count.
    [[nodiscard]] auto command_count() const -> int;

    // ── Search/ranking ──────────────────────────────────────────────

    /// Fuzzy search commands by query. Returns ranked results.
    /// Ranking: pinned first → MRU boost → alias matches → label/category match.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<CommandMatch>;

    // ── MRU ─────────────────────────────────────────────────────────

    /// Record that a command was executed (adds to MRU front).
    void record_usage(const std::string& command_id);

    /// Get MRU list (most recent first).
    [[nodiscard]] auto mru_history() const -> const std::vector<std::string>&;

    /// Max MRU entries.
    static constexpr int kMaxMruEntries = 20;

    // ── Pinned commands ─────────────────────────────────────────────

    /// Pin/unpin a command.
    void pin_command(const std::string& command_id);
    void unpin_command(const std::string& command_id);

    // ── Preview/safety ──────────────────────────────────────────────

    /// Check if a command requires confirmation.
    [[nodiscard]] auto needs_confirmation(const std::string& command_id) const -> bool;

    /// Get preview text for a command.
    [[nodiscard]] auto preview_text(const std::string& command_id) const -> std::string;

private:
    std::vector<CommandMetadata> commands_;
    std::vector<std::string> mru_history_;

    /// Compute fuzzy match score between query and candidate.
    [[nodiscard]] static auto fuzzy_score(const std::string& query, const std::string& candidate)
        -> int;
};

} // namespace markamp::ui
