#include "FSRSReviewSession.h"

#include <algorithm>
#include <chrono>

namespace markamp::core::fsrs
{

FSRSReviewSession::FSRSReviewSession(FlashcardStore* store, FSRSEngine* engine, EventBus* event_bus)
    : store_(store)
    , engine_(engine)
    , event_bus_(event_bus)
{
}

auto FSRSReviewSession::start(const std::string& deck_id) -> std::expected<void, std::string>
{
    if (store_ == nullptr || engine_ == nullptr)
    {
        return std::unexpected("Store or engine not initialized");
    }

    deck_id_ = deck_id;
    const auto now = std::chrono::system_clock::now();

    // Load due cards (learning/relearning first, then review, then new)
    queue_ = store_->get_due_cards(deck_id, now);

    if (queue_.empty())
    {
        state_ = ReviewSessionState::Completed;
        return {};
    }

    current_index_ = 0;
    state_ = ReviewSessionState::InProgress;
    again_count_ = 0;
    hard_count_ = 0;
    good_count_ = 0;
    easy_count_ = 0;

    return {};
}

auto FSRSReviewSession::current_card() const -> std::optional<Card>
{
    if (state_ == ReviewSessionState::NotStarted || state_ == ReviewSessionState::Completed ||
        current_index_ >= static_cast<int>(queue_.size()))
    {
        return std::nullopt;
    }
    return queue_[static_cast<size_t>(current_index_)];
}

auto FSRSReviewSession::current_scheduling() const -> std::optional<SchedulingResult>
{
    const auto card_opt = current_card();
    if (!card_opt.has_value() || engine_ == nullptr)
    {
        return std::nullopt;
    }
    return engine_->schedule(card_opt.value(), std::chrono::system_clock::now());
}

void FSRSReviewSession::show_answer()
{
    if (state_ == ReviewSessionState::InProgress)
    {
        state_ = ReviewSessionState::ShowingAnswer;
    }
}

auto FSRSReviewSession::rate(Rating rating) -> std::expected<void, std::string>
{
    const auto card_opt = current_card();
    if (!card_opt.has_value())
    {
        return std::unexpected("No current card to rate");
    }

    const auto now = std::chrono::system_clock::now();
    const auto scheduled = engine_->review(card_opt.value(), rating, now);

    // Update card in store
    if (const auto result = store_->update_card(scheduled.card); !result)
    {
        return std::unexpected(result.error());
    }

    // Add review log
    auto log = scheduled.review_log;
    log.id = card_opt->id + "_log_" + std::to_string(card_opt->reps);
    if (const auto result = store_->add_review_log(log); !result)
    {
        return std::unexpected(result.error());
    }

    // Update statistics
    switch (rating)
    {
        case Rating::Again:
            ++again_count_;
            break;
        case Rating::Hard:
            ++hard_count_;
            break;
        case Rating::Good:
            ++good_count_;
            break;
        case Rating::Easy:
            ++easy_count_;
            break;
    }

    // Advance to next card
    ++current_index_;
    if (current_index_ >= static_cast<int>(queue_.size()))
    {
        state_ = ReviewSessionState::Completed;
    }
    else
    {
        state_ = ReviewSessionState::InProgress;
    }

    return {};
}

void FSRSReviewSession::skip()
{
    if (current_index_ < static_cast<int>(queue_.size()))
    {
        // Move current card to end
        auto card = queue_[static_cast<size_t>(current_index_)];
        queue_.erase(queue_.begin() + current_index_);
        queue_.push_back(std::move(card));

        if (current_index_ >= static_cast<int>(queue_.size()))
        {
            state_ = ReviewSessionState::Completed;
        }
        else
        {
            state_ = ReviewSessionState::InProgress;
        }
    }
}

void FSRSReviewSession::end_session()
{
    state_ = ReviewSessionState::Completed;
    queue_.clear();
    current_index_ = 0;
}

auto FSRSReviewSession::state() const -> ReviewSessionState
{
    return state_;
}

auto FSRSReviewSession::deck_id() const -> const std::string&
{
    return deck_id_;
}

auto FSRSReviewSession::cards_reviewed() const -> int
{
    return again_count_ + hard_count_ + good_count_ + easy_count_;
}

auto FSRSReviewSession::cards_remaining() const -> int
{
    return std::max(0, static_cast<int>(queue_.size()) - current_index_);
}

auto FSRSReviewSession::total_cards() const -> int
{
    return static_cast<int>(queue_.size());
}

auto FSRSReviewSession::again_count() const -> int
{
    return again_count_;
}
auto FSRSReviewSession::hard_count() const -> int
{
    return hard_count_;
}
auto FSRSReviewSession::good_count() const -> int
{
    return good_count_;
}
auto FSRSReviewSession::easy_count() const -> int
{
    return easy_count_;
}

} // namespace markamp::core::fsrs
