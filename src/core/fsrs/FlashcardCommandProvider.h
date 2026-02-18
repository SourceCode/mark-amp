#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Command Info: describes a single command for the command palette
// ============================================================================

struct FlashcardCommandInfo
{
    std::string id;          // Unique command identifier
    std::string label;       // Display label (e.g. "Flashcard: Start Review")
    std::string description; // Brief description
    std::string category;    // Category for grouping (always "Flashcard")
    std::string keybinding;  // Keyboard shortcut (empty if none)
    bool enabled{true};      // Whether the command is currently available
};

// ============================================================================
// FlashcardCommandProvider: registers flashcard commands in the command palette
// ============================================================================

class FlashcardCommandProvider
{
public:
    FlashcardCommandProvider() = default;

    /// Get all available flashcard commands.
    [[nodiscard]] auto get_commands() const -> std::vector<FlashcardCommandInfo>;

    /// Check if a specific command is currently enabled.
    [[nodiscard]] auto is_enabled(const std::string& command_id) const -> bool;

    /// Get the keybinding for a specific command, if any.
    [[nodiscard]] auto get_keybinding(const std::string& command_id) const
        -> std::optional<std::string>;

    /// Get a specific command by ID.
    [[nodiscard]] auto get_command(const std::string& command_id) const
        -> std::optional<FlashcardCommandInfo>;

    // ── Command IDs ──

    static constexpr const char* kStartReview = "flashcard.startReview";
    static constexpr const char* kBrowseDecks = "flashcard.browseDecks";
    static constexpr const char* kAddCard = "flashcard.addCard";
    static constexpr const char* kImport = "flashcard.import";
    static constexpr const char* kExport = "flashcard.export";
    static constexpr const char* kStatistics = "flashcard.statistics";
    static constexpr const char* kSuspendCard = "flashcard.suspendCard";
    static constexpr const char* kSearchCards = "flashcard.searchCards";

private:
    /// Build the static command list.
    [[nodiscard]] static auto build_commands() -> std::vector<FlashcardCommandInfo>;
};

} // namespace markamp::core::fsrs
