#include "FlashcardCommandProvider.h"

#include <algorithm>

namespace markamp::core::fsrs
{

auto FlashcardCommandProvider::get_commands() const -> std::vector<FlashcardCommandInfo>
{
    return build_commands();
}

auto FlashcardCommandProvider::is_enabled(const std::string& command_id) const -> bool
{
    const auto commands = build_commands();
    const auto it = std::find_if(commands.begin(),
                                 commands.end(),
                                 [&command_id](const FlashcardCommandInfo& cmd)
                                 { return cmd.id == command_id; });
    if (it == commands.end())
    {
        return false;
    }
    return it->enabled;
}

auto FlashcardCommandProvider::get_keybinding(const std::string& command_id) const
    -> std::optional<std::string>
{
    const auto commands = build_commands();
    const auto it = std::find_if(commands.begin(),
                                 commands.end(),
                                 [&command_id](const FlashcardCommandInfo& cmd)
                                 { return cmd.id == command_id; });
    if (it == commands.end() || it->keybinding.empty())
    {
        return std::nullopt;
    }
    return it->keybinding;
}

auto FlashcardCommandProvider::get_command(const std::string& command_id) const
    -> std::optional<FlashcardCommandInfo>
{
    const auto commands = build_commands();
    const auto it = std::find_if(commands.begin(),
                                 commands.end(),
                                 [&command_id](const FlashcardCommandInfo& cmd)
                                 { return cmd.id == command_id; });
    if (it == commands.end())
    {
        return std::nullopt;
    }
    return *it;
}

auto FlashcardCommandProvider::build_commands() -> std::vector<FlashcardCommandInfo>
{
    return {
        {kStartReview,
         "Flashcard: Start Review",
         "Begin a review session with all due cards",
         "Flashcard",
         "Cmd+Shift+R",
         true},
        {kBrowseDecks,
         "Flashcard: Browse Decks",
         "Open the deck browser panel",
         "Flashcard",
         "",
         true},
        {kAddCard, "Flashcard: Add Card", "Create a new flashcard manually", "Flashcard", "", true},
        {kImport, "Flashcard: Import", "Import flashcards from CSV or JSON", "Flashcard", "", true},
        {kExport, "Flashcard: Export", "Export flashcards to CSV or JSON", "Flashcard", "", true},
        {kStatistics,
         "Flashcard: Statistics",
         "View deck statistics and study streak",
         "Flashcard",
         "",
         true},
        {kSuspendCard,
         "Flashcard: Suspend Card",
         "Suspend the current card from review",
         "Flashcard",
         "",
         true},
        {kSearchCards,
         "Flashcard: Search Cards",
         "Search flashcards by content or tags",
         "Flashcard",
         "",
         true},
    };
}

} // namespace markamp::core::fsrs
