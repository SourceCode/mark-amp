#pragma once

#include "../FlashcardTypes.h"

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Leech Status: evaluation result for a single card
// ============================================================================

struct LeechStatus
{
    std::string card_id;
    int32_t lapse_count{0};
    bool is_leech{false};
    bool is_suspended{false};
};

// ============================================================================
// LeechDetector: identifies problematic cards and manages suspension
// A leech is a card that has been forgotten (lapsed) too many times,
// indicating it may need to be rewritten, split, or suspended.
// ============================================================================

class LeechDetector
{
public:
    /// Default leech threshold: 8 lapses.
    static constexpr int32_t kDefaultLeechThreshold = 8;

    /// Tag used to mark suspended cards.
    static constexpr const char* kSuspendedTag = "suspended";

    explicit LeechDetector(int32_t threshold = kDefaultLeechThreshold);

    /// Check a single card for leech status.
    [[nodiscard]] auto check_card(const FlashcardCard& card) const -> LeechStatus;

    /// Find all leeches in a deck.
    [[nodiscard]] auto get_leeches(const FlashcardDeck& deck) const -> std::vector<LeechStatus>;

    /// Suspend a card by adding the "suspended" tag to its block_id metadata.
    /// Returns the updated card or an error.
    [[nodiscard]] auto suspend_card(FlashcardDeck& deck, const std::string& card_id) const
        -> std::expected<void, std::string>;

    /// Unsuspend a card by removing the "suspended" tag.
    [[nodiscard]] auto unsuspend_card(FlashcardDeck& deck, const std::string& card_id) const
        -> std::expected<void, std::string>;

    /// Check if a card is currently suspended.
    [[nodiscard]] auto is_suspended(const FlashcardCard& card) const -> bool;

    /// Get all suspended cards in a deck.
    [[nodiscard]] auto get_suspended(const FlashcardDeck& deck) const -> std::vector<std::string>;

    // ── Configuration ──

    void set_leech_threshold(int32_t count);
    [[nodiscard]] auto leech_threshold() const -> int32_t;

private:
    int32_t threshold_{kDefaultLeechThreshold};
};

} // namespace markamp::core::fsrs
