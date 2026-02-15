#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// State of a flashcard relative to the scheduling algorithm.
enum class CardState : uint8_t
{
    New = 0,       // Never reviewed
    Learning = 1,  // In initial learning steps
    Review = 2,    // Graduated to review queue
    Relearning = 3 // Failed review, relearning
};

/// Scheduling rating for a flashcard review.
enum class Rating : uint8_t
{
    Again = 1,
    Hard = 2,
    Good = 3,
    Easy = 4
};

/// A single flashcard linking a document block to FSRS scheduling data.
struct FlashcardCard
{
    std::string id;       // Unique card ID (UUID)
    std::string block_id; // ID of the Markdown block this card references
    std::string deck_id;  // ID of the deck containing this card
    CardState state{CardState::New};

    // FSRS scheduling fields
    double stability{0.0};
    double difficulty{0.0};
    int elapsed_days{0};
    int scheduled_days{0};
    int reps{0};
    int lapses{0};

    std::chrono::system_clock::time_point due;
    std::chrono::system_clock::time_point last_review;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point updated;

    [[nodiscard]] auto is_due() const -> bool
    {
        return due <= std::chrono::system_clock::now();
    }

    [[nodiscard]] auto is_new() const -> bool
    {
        return state == CardState::New;
    }

    [[nodiscard]] auto days_until_due() const -> int
    {
        auto now = std::chrono::system_clock::now();
        if (due <= now)
        {
            return 0;
        }
        return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(due - now).count() /
                                24);
    }
};

/// Aggregate statistics for a deck.
struct DeckStats
{
    int total_cards{0};
    int new_cards{0};
    int learning_cards{0};
    int review_cards{0};
    int due_cards{0};

    [[nodiscard]] auto is_empty() const -> bool
    {
        return total_cards == 0;
    }
};

/// A flashcard deck — a named collection of cards.
struct FlashcardDeck
{
    std::string id;   // Unique deck ID (UUID or "builtin" for the default deck)
    std::string name; // Human-readable deck name
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point updated;
    std::vector<FlashcardCard> cards;

    [[nodiscard]] auto card_count() const -> int
    {
        return static_cast<int>(cards.size());
    }

    [[nodiscard]] auto compute_stats() const -> DeckStats
    {
        DeckStats stats;
        stats.total_cards = card_count();
        for (const auto& card : cards)
        {
            switch (card.state)
            {
                case CardState::New:
                    ++stats.new_cards;
                    break;
                case CardState::Learning:
                case CardState::Relearning:
                    ++stats.learning_cards;
                    break;
                case CardState::Review:
                    ++stats.review_cards;
                    break;
            }
            if (card.is_due())
            {
                ++stats.due_cards;
            }
        }
        return stats;
    }

    [[nodiscard]] auto find_card(const std::string& card_id) const -> const FlashcardCard*
    {
        for (const auto& card : cards)
        {
            if (card.id == card_id)
            {
                return &card;
            }
        }
        return nullptr;
    }

    [[nodiscard]] auto find_card_mut(const std::string& card_id) -> FlashcardCard*
    {
        for (auto& card : cards)
        {
            if (card.id == card_id)
            {
                return &card;
            }
        }
        return nullptr;
    }

    [[nodiscard]] auto has_block(const std::string& block_id) const -> bool
    {
        for (const auto& card : cards)
        {
            if (card.block_id == block_id)
            {
                return true;
            }
        }
        return false;
    }
};

/// Configuration parameters for the FSRS scheduler.
struct FlashcardConfig
{
    int new_card_limit{20};
    int review_card_limit{200};
    double request_retention{0.9};
    int maximum_interval{36500}; // days (~100 years)
};

/// Identifier for the built-in default deck that always exists.
inline constexpr std::string_view kBuiltinDeckID = "builtin";

} // namespace markamp::core
