#pragma once

#include "FSRSEngine.h"
#include "FSRSTypes.h"
#include "FlashcardStore.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
}

namespace markamp::core::fsrs
{

// ============================================================================
// Review Session State
// ============================================================================

enum class ReviewSessionState
{
    NotStarted,
    InProgress,
    ShowingAnswer,
    Completed
};

// ============================================================================
// FSRS Review Session: manages a single study session (Phase 24 version)
// ============================================================================

class FSRSReviewSession
{
public:
    FSRSReviewSession(FlashcardStore* store, FSRSEngine* engine, EventBus* event_bus);

    /// Start a review session for a deck.
    /// Loads due cards + new cards (up to daily limits) and orders them.
    [[nodiscard]] auto start(const std::string& deck_id) -> std::expected<void, std::string>;

    /// Get the current card being reviewed.
    [[nodiscard]] auto current_card() const -> std::optional<Card>;

    /// Get the scheduling result for the current card (shows all 4 intervals).
    [[nodiscard]] auto current_scheduling() const -> std::optional<SchedulingResult>;

    /// Show the answer for the current card.
    void show_answer();

    /// Rate the current card and advance to the next.
    [[nodiscard]] auto rate(Rating rating) -> std::expected<void, std::string>;

    /// Skip the current card (move to end of queue).
    void skip();

    /// End the session early.
    void end_session();

    // ── Session state ──

    [[nodiscard]] auto state() const -> ReviewSessionState;
    [[nodiscard]] auto deck_id() const -> const std::string&;
    [[nodiscard]] auto cards_reviewed() const -> int;
    [[nodiscard]] auto cards_remaining() const -> int;
    [[nodiscard]] auto total_cards() const -> int;

    // ── Statistics for current session ──

    [[nodiscard]] auto again_count() const -> int;
    [[nodiscard]] auto hard_count() const -> int;
    [[nodiscard]] auto good_count() const -> int;
    [[nodiscard]] auto easy_count() const -> int;

private:
    FlashcardStore* store_{nullptr};
    FSRSEngine* engine_{nullptr};
    [[maybe_unused]] EventBus* event_bus_{nullptr};

    std::string deck_id_;
    ReviewSessionState state_{ReviewSessionState::NotStarted};

    std::vector<Card> queue_;
    int current_index_{0};

    // Session statistics
    int again_count_{0};
    int hard_count_{0};
    int good_count_{0};
    int easy_count_{0};
};

} // namespace markamp::core::fsrs
