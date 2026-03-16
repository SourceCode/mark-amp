#include "ReviewSession.h"

#include "DeckManager.h"

#include <algorithm>
#include <chrono>

namespace markamp::core
{

ReviewSession::ReviewSession(DeckManager& deck_manager, EventBus& event_bus)
    : deck_manager_(deck_manager)
    , event_bus_(event_bus)
{
}

auto ReviewSession::start_review(const std::string& deck_id, ContentRenderer renderer)
    -> std::expected<NextCardResult, std::string>
{
    deck_id_ = deck_id;
    renderer_ = std::move(renderer);

    const auto config = deck_manager_.flashcard_config();

    // Load due cards + limited new cards
    auto due = deck_manager_.get_due_cards(deck_id, config.review_card_limit);
    auto new_cards = deck_manager_.get_new_cards(deck_id, config.new_card_limit);

    queue_.clear();
    undo_stack_.clear();

    // Build review cards: due first, then new
    for (auto& card : due)
    {
        queue_.push_back(build_review_card(card));
    }
    for (auto& card : new_cards)
    {
        queue_.push_back(build_review_card(card));
    }

    stats_ = SessionStats{};
    stats_.total_cards = static_cast<int>(queue_.size());
    stats_.cards_remaining = stats_.total_cards;
    current_index_ = 0;

    session_start_ = std::chrono::system_clock::now();

    if (queue_.empty())
    {
        state_ = SessionState::Completed;
    }
    else
    {
        state_ = SessionState::InProgress;
    }

    return build_next_result();
}

auto ReviewSession::reveal_answer() -> std::expected<ReviewCard, std::string>
{
    if (state_ != SessionState::InProgress)
    {
        return std::unexpected("Cannot reveal answer in current state");
    }
    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        return std::unexpected("No current card");
    }

    state_ = SessionState::ShowingAnswer;
    return queue_[static_cast<size_t>(current_index_)];
}

auto ReviewSession::rate_card(Rating rating) -> std::expected<NextCardResult, std::string>
{
    if (state_ != SessionState::ShowingAnswer && state_ != SessionState::InProgress)
    {
        return std::unexpected("Cannot rate card in current state");
    }
    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        return std::unexpected("No current card to rate");
    }

    auto& review_card = queue_[static_cast<size_t>(current_index_)];
    const auto card_before = review_card.card;

    // Create updated card with new scheduling data
    auto updated_card = review_card.card;
    updated_card.last_review = std::chrono::system_clock::now();
    updated_card.reps += 1;
    updated_card.updated = std::chrono::system_clock::now();

    // Transition state based on rating
    switch (rating)
    {
        case Rating::Again:
            updated_card.state = (updated_card.state == CardState::Review) ? CardState::Relearning
                                                                           : CardState::Learning;
            if (updated_card.state == CardState::Relearning)
            {
                updated_card.lapses += 1;
            }
            break;
        case Rating::Hard:
            // Stay in current state (learning) or stay in review
            break;
        case Rating::Good:
        case Rating::Easy:
            if (updated_card.state == CardState::New || updated_card.state == CardState::Learning ||
                updated_card.state == CardState::Relearning)
            {
                updated_card.state = CardState::Review; // Graduate
            }
            break;
    }

    // Save undo record
    ReviewRecord record;
    record.card_before = card_before;
    record.card_after = updated_card;
    record.rating_applied = rating;
    record.reviewed_at = std::chrono::system_clock::now();
    undo_stack_.push_back(std::move(record));

    // Persist update
    static_cast<void>(deck_manager_.update_card_after_review(updated_card.id, updated_card));

    // Update statistics
    ++stats_.cards_reviewed;
    switch (rating)
    {
        case Rating::Again:
            ++stats_.again_count;
            break;
        case Rating::Hard:
            ++stats_.hard_count;
            break;
        case Rating::Good:
            ++stats_.good_count;
            break;
        case Rating::Easy:
            ++stats_.easy_count;
            break;
    }

    // Advance
    ++current_index_;
    stats_.cards_remaining = std::max(0, static_cast<int>(queue_.size()) - current_index_);

    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        state_ = SessionState::Completed;
    }
    else
    {
        state_ = SessionState::InProgress;
    }

    update_elapsed();
    return build_next_result();
}

auto ReviewSession::skip_card() -> std::expected<NextCardResult, std::string>
{
    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        return std::unexpected("No card to skip");
    }

    // Move current card to end
    auto card = std::move(queue_[static_cast<size_t>(current_index_)]);
    queue_.erase(queue_.begin() + current_index_);
    queue_.push_back(std::move(card));

    ++stats_.skipped_count;

    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        state_ = SessionState::Completed;
    }
    else
    {
        state_ = SessionState::InProgress;
    }

    return build_next_result();
}

auto ReviewSession::undo_last_rating() -> std::expected<NextCardResult, std::string>
{
    if (undo_stack_.empty())
    {
        return std::unexpected("Nothing to undo");
    }

    const auto& record = undo_stack_.back();

    // Restore card state
    static_cast<void>(
        deck_manager_.update_card_after_review(record.card_before.id, record.card_before));

    // Revert stats
    --stats_.cards_reviewed;
    switch (record.rating_applied)
    {
        case Rating::Again:
            --stats_.again_count;
            break;
        case Rating::Hard:
            --stats_.hard_count;
            break;
        case Rating::Good:
            --stats_.good_count;
            break;
        case Rating::Easy:
            --stats_.easy_count;
            break;
    }

    // Move index back
    --current_index_;
    stats_.cards_remaining = std::max(0, static_cast<int>(queue_.size()) - current_index_);

    // Rebuild the review card at the current position
    queue_[static_cast<size_t>(current_index_)] = build_review_card(record.card_before);

    undo_stack_.pop_back();
    state_ = SessionState::InProgress;

    return build_next_result();
}

auto ReviewSession::end_session() -> std::expected<SessionStats, std::string>
{
    update_elapsed();
    state_ = SessionState::Completed;
    return stats_;
}

// ── Accessors ──

auto ReviewSession::current_card() const -> std::optional<ReviewCard>
{
    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        return std::nullopt;
    }
    return queue_[static_cast<size_t>(current_index_)];
}

auto ReviewSession::state() const -> SessionState
{
    return state_;
}

auto ReviewSession::stats() const -> SessionStats
{
    return stats_;
}

auto ReviewSession::deck_id() const -> const std::string&
{
    return deck_id_;
}

auto ReviewSession::can_undo() const -> bool
{
    return !undo_stack_.empty();
}

// ── Private ──

auto ReviewSession::build_review_card(const FlashcardCard& card) -> ReviewCard
{
    ReviewCard review_card;
    review_card.card = card;

    // Render content if renderer is available
    if (renderer_)
    {
        review_card.front_html = renderer_(card.block_id + ":front");
        review_card.back_html = renderer_(card.block_id + ":back");
    }
    else
    {
        review_card.front_html = "<p>Card " + card.id + " front</p>";
        review_card.back_html = "<p>Card " + card.id + " back</p>";
    }

    // Compute review intervals from card stability and state.
    // FSRS-derived: intervals scale with stability.
    auto format_interval = [](double days) -> std::string
    {
        if (days < 1.0 / 24.0)
        {
            const int minutes = std::max(1, static_cast<int>(days * 24.0 * 60.0));
            return std::to_string(minutes) + "m";
        }
        if (days < 1.0)
        {
            const int hours = std::max(1, static_cast<int>(days * 24.0));
            return std::to_string(hours) + "h";
        }
        const int whole_days = std::max(1, static_cast<int>(days));
        return std::to_string(whole_days) + "d";
    };

    // Base intervals from card state and stability.
    const double base_stability = std::max(0.4, card.stability);
    review_card.again_interval = format_interval(base_stability * 0.01); // ~1% of stability
    review_card.hard_interval = format_interval(base_stability * 0.5);   // 50% of stability
    review_card.good_interval = format_interval(base_stability);         // 100% of stability
    review_card.easy_interval = format_interval(base_stability * 2.5);   // 250% of stability

    return review_card;
}

auto ReviewSession::build_next_result() -> NextCardResult
{
    NextCardResult result;
    result.stats = stats_;
    result.session_complete = (state_ == SessionState::Completed);

    if (current_index_ < static_cast<int>(queue_.size()))
    {
        result.card = queue_[static_cast<size_t>(current_index_)];
    }

    return result;
}

void ReviewSession::update_elapsed()
{
    const auto now = std::chrono::system_clock::now();
    stats_.total_time = std::chrono::duration_cast<std::chrono::seconds>(now - session_start_);
}

} // namespace markamp::core
