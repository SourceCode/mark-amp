#pragma once

#include "FlashcardTypes.h"

#include <chrono>
#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class DeckManager;

// ============================================================================
// Review Card: enriched card with rendered content and schedule previews
// ============================================================================

struct ReviewCard
{
    FlashcardCard card;     // The underlying card
    std::string front_html; // Rendered HTML for the front
    std::string back_html;  // Rendered HTML for the back

    /// Preview intervals for each rating button
    std::string again_interval; // e.g. "1m"
    std::string hard_interval;  // e.g. "6m"
    std::string good_interval;  // e.g. "10m"
    std::string easy_interval;  // e.g. "4d"
};

// ============================================================================
// Session Statistics
// ============================================================================

struct SessionStats
{
    int total_cards{0};
    int cards_reviewed{0};
    int cards_remaining{0};
    int again_count{0};
    int hard_count{0};
    int good_count{0};
    int easy_count{0};
    int skipped_count{0};
    std::chrono::seconds total_time{0}; // Total session duration

    [[nodiscard]] auto average_time_per_card() const -> std::chrono::seconds
    {
        if (cards_reviewed == 0)
        {
            return std::chrono::seconds{0};
        }
        return total_time / cards_reviewed;
    }

    [[nodiscard]] auto retention_rate() const -> double
    {
        if (cards_reviewed == 0)
        {
            return 0.0;
        }
        return static_cast<double>(cards_reviewed - again_count) /
               static_cast<double>(cards_reviewed);
    }
};

// ============================================================================
// Next Card Result
// ============================================================================

struct NextCardResult
{
    std::optional<ReviewCard> card; // nullopt if session complete
    SessionStats stats;
    bool session_complete{false};
};

// ============================================================================
// Review Record (for undo support)
// ============================================================================

struct ReviewRecord
{
    FlashcardCard card_before;           // Card state before review
    FlashcardCard card_after;            // Card state after review
    Rating rating_applied{Rating::Good}; // Rating that was applied
    std::chrono::system_clock::time_point reviewed_at;
};

// ============================================================================
// Review Session State
// ============================================================================

enum class SessionState
{
    NotStarted,
    InProgress,
    ShowingAnswer,
    Completed,
    Paused
};

// ============================================================================
// ReviewSession: Phase 26 high-level review session controller
// Manages the full review experience with undo, statistics, and smart ordering.
// ============================================================================

class ReviewSession
{
public:
    ReviewSession(DeckManager& deck_manager, EventBus& event_bus);
    ~ReviewSession() = default;

    ReviewSession(const ReviewSession&) = delete;
    auto operator=(const ReviewSession&) -> ReviewSession& = delete;
    ReviewSession(ReviewSession&&) = delete;
    auto operator=(ReviewSession&&) -> ReviewSession& = delete;

    /// Start a review session for a specific deck.
    /// content_renderer is called to convert block_id -> HTML.
    using ContentRenderer = std::function<std::string(const std::string& block_id)>;

    [[nodiscard]] auto start_review(const std::string& deck_id, ContentRenderer renderer = nullptr)
        -> std::expected<NextCardResult, std::string>;

    /// Show the answer for the current card.
    [[nodiscard]] auto reveal_answer() -> std::expected<ReviewCard, std::string>;

    /// Rate the current card and advance to the next.
    [[nodiscard]] auto rate_card(Rating rating) -> std::expected<NextCardResult, std::string>;

    /// Skip the current card (move to end of queue).
    [[nodiscard]] auto skip_card() -> std::expected<NextCardResult, std::string>;

    /// Undo the last rating (restore previous card state).
    [[nodiscard]] auto undo_last_rating() -> std::expected<NextCardResult, std::string>;

    /// End the session early and save all progress.
    [[nodiscard]] auto end_session() -> std::expected<SessionStats, std::string>;

    // ── Accessors ──

    [[nodiscard]] auto current_card() const -> std::optional<ReviewCard>;
    [[nodiscard]] auto state() const -> SessionState;
    [[nodiscard]] auto stats() const -> SessionStats;
    [[nodiscard]] auto deck_id() const -> const std::string&;
    [[nodiscard]] auto can_undo() const -> bool;

private:
    DeckManager& deck_manager_;
    EventBus& event_bus_;

    std::string deck_id_;
    SessionState state_{SessionState::NotStarted};
    ContentRenderer renderer_;

    std::vector<ReviewCard> queue_;
    int current_index_{0};

    // Undo stack (most recent first)
    std::vector<ReviewRecord> undo_stack_;

    // Statistics
    SessionStats stats_;
    std::chrono::system_clock::time_point session_start_;

    /// Build a ReviewCard from a FlashcardCard.
    [[nodiscard]] auto build_review_card(const FlashcardCard& card) -> ReviewCard;

    /// Build a NextCardResult for the current state.
    [[nodiscard]] auto build_next_result() -> NextCardResult;

    /// Update elapsed time in stats.
    void update_elapsed();
};

} // namespace markamp::core
