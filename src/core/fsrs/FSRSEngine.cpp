#include "FSRSEngine.h"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace markamp::core::fsrs
{

FSRSEngine::FSRSEngine(FSRSParameters params)
    : params_(std::move(params))
{
}

// ── Parameter access ──

auto FSRSEngine::parameters() const -> const FSRSParameters&
{
    return params_;
}

void FSRSEngine::set_parameters(FSRSParameters params)
{
    params_ = std::move(params);
}

// ============================================================================
// Core FSRS Formulas
// ============================================================================

auto FSRSEngine::initial_stability(Rating rating) const -> double
{
    // S_0(G) = w[G-1]
    const auto index = rating_to_int(rating) - 1;
    return std::max(params_.w[index], 0.1);
}

auto FSRSEngine::initial_difficulty(Rating rating) const -> double
{
    // D_0(G) = w[4] - exp(w[5] * (G - 1)) + 1
    const auto grade = static_cast<double>(rating_to_int(rating));
    const auto diff = params_.w[4] - std::exp(params_.w[5] * (grade - 1.0)) + 1.0;
    return clamp_difficulty(diff);
}

auto FSRSEngine::next_difficulty(double current_difficulty, Rating rating) const -> double
{
    // D'(D, G) = w[7] * D_0(4) + (1 - w[7]) * (D - w[6] * (G - 3))
    const auto grade = static_cast<double>(rating_to_int(rating));
    const auto new_diff = current_difficulty - params_.w[6] * (grade - 3.0);
    return clamp_difficulty(mean_reversion(initial_difficulty(Rating::Easy), new_diff));
}

auto FSRSEngine::mean_reversion(double init, double current) const -> double
{
    // mean_reversion(init, current) = w[7] * init + (1 - w[7]) * current
    return params_.w[7] * init + (1.0 - params_.w[7]) * current;
}

auto FSRSEngine::next_recall_stability(double difficulty,
                                       double stability,
                                       double retrievability,
                                       Rating rating) const -> double
{
    // S'_r(D, S, R, G) = S * (1 + exp(w[8]) * (11 - D) *
    //   S^(-w[9]) * (exp((1-R) * w[10]) - 1) *
    //   (G==Hard ? w[15] : G==Easy ? w[16] : 1))
    const auto hard_penalty = (rating == Rating::Hard) ? params_.w[15] : 1.0;
    const auto easy_bonus = (rating == Rating::Easy) ? params_.w[16] : 1.0;

    const auto new_stability =
        stability *
        (1.0 + std::exp(params_.w[8]) * (11.0 - difficulty) * std::pow(stability, -params_.w[9]) *
                   (std::exp((1.0 - retrievability) * params_.w[10]) - 1.0) * hard_penalty *
                   easy_bonus);

    return std::max(new_stability, 0.1);
}

auto FSRSEngine::next_forget_stability(double difficulty,
                                       double stability,
                                       double retrievability) const -> double
{
    // S'_f(D, S, R) = w[11] * D^(-w[12]) *
    //   ((S+1)^w[13] - 1) * exp((1-R) * w[14])
    const auto new_stability = params_.w[11] * std::pow(difficulty, -params_.w[12]) *
                               (std::pow(stability + 1.0, params_.w[13]) - 1.0) *
                               std::exp((1.0 - retrievability) * params_.w[14]);

    return std::max(std::min(new_stability, stability), 0.1);
}

auto FSRSEngine::retrievability(double elapsed_days, double stability) -> double
{
    // R(t, S) = (1 + t / (9 * S))^(-1)
    if (stability <= 0.0)
    {
        return 0.0;
    }
    return std::pow(1.0 + elapsed_days / (9.0 * stability), -1.0);
}

auto FSRSEngine::next_interval(double stability) const -> double
{
    // I(R, S) = 9 * S * (1/R - 1)
    const auto interval = 9.0 * stability * (1.0 / params_.request_retention - 1.0);
    return clamp_interval(std::round(interval));
}

auto FSRSEngine::clamp_difficulty(double diff) -> double
{
    return std::clamp(diff, 1.0, 10.0);
}

auto FSRSEngine::clamp_interval(double interval) const -> double
{
    return std::clamp(interval, 1.0, params_.maximum_interval);
}

// ============================================================================
// Scheduling Dispatch
// ============================================================================

auto FSRSEngine::schedule(const Card& card, TimePoint now) const -> SchedulingResult
{
    switch (card.state)
    {
        case CardState::New:
            return schedule_new(card, now);
        case CardState::Learning:
        case CardState::Relearning:
            return schedule_learning(card, now);
        case CardState::Review:
            return schedule_review(card, now);
    }
    return schedule_new(card, now);
}

auto FSRSEngine::review(const Card& card, Rating rating, TimePoint now) const -> ScheduledState
{
    const auto result = schedule(card, now);
    return result.for_rating(rating);
}

auto FSRSEngine::get_retrievability(const Card& card, TimePoint now) const -> double
{
    if (card.state == CardState::New)
    {
        return 0.0;
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::hours>(now - card.last_review).count() / 24.0;
    return retrievability(elapsed, card.stability);
}

// ============================================================================
// Schedule New Card
// ============================================================================

auto FSRSEngine::schedule_new(const Card& card, TimePoint now) const -> SchedulingResult
{
    SchedulingResult result;

    // Again -> Learning, 1 minute
    result.again = build_scheduled_state(card,
                                         Rating::Again,
                                         CardState::Learning,
                                         initial_difficulty(Rating::Again),
                                         initial_stability(Rating::Again),
                                         1.0 / (24.0 * 60.0), // 1 minute in days
                                         now);

    // Hard -> Learning, 5 minutes
    result.hard = build_scheduled_state(card,
                                        Rating::Hard,
                                        CardState::Learning,
                                        initial_difficulty(Rating::Hard),
                                        initial_stability(Rating::Hard),
                                        5.0 / (24.0 * 60.0), // 5 minutes in days
                                        now);

    // Good -> Learning, 10 minutes
    result.good = build_scheduled_state(card,
                                        Rating::Good,
                                        CardState::Learning,
                                        initial_difficulty(Rating::Good),
                                        initial_stability(Rating::Good),
                                        10.0 / (24.0 * 60.0), // 10 minutes in days
                                        now);

    // Easy -> Review, computed interval
    const auto easy_stability = initial_stability(Rating::Easy);
    const auto easy_interval = next_interval(easy_stability);
    result.easy = build_scheduled_state(card,
                                        Rating::Easy,
                                        CardState::Review,
                                        initial_difficulty(Rating::Easy),
                                        easy_stability,
                                        easy_interval,
                                        now);

    return result;
}

// ============================================================================
// Schedule Learning/Relearning Card
// ============================================================================

auto FSRSEngine::schedule_learning(const Card& card, TimePoint now) const -> SchedulingResult
{
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::hours>(now - card.last_review).count() / 24.0;
    const auto ret =
        (card.stability > 0.0) ? retrievability(std::max(elapsed, 0.0), card.stability) : 0.0;

    const auto curr_diff = (card.difficulty > 0.0) ? card.difficulty : 5.0;
    const auto curr_stab = (card.stability > 0.0) ? card.stability : 1.0;

    SchedulingResult result;

    // Again -> Relearning, 5 minutes
    const auto again_state =
        (card.state == CardState::Learning) ? CardState::Learning : CardState::Relearning;
    result.again = build_scheduled_state(card,
                                         Rating::Again,
                                         again_state,
                                         next_difficulty(curr_diff, Rating::Again),
                                         std::max(curr_stab * 0.5, 0.1),
                                         5.0 / (24.0 * 60.0), // 5 minutes
                                         now);

    // Hard -> Learning, 10 minutes
    result.hard = build_scheduled_state(card,
                                        Rating::Hard,
                                        card.state,
                                        next_difficulty(curr_diff, Rating::Hard),
                                        std::max(curr_stab * 0.8, 0.1),
                                        10.0 / (24.0 * 60.0), // 10 minutes
                                        now);

    // Good -> Graduate to Review
    const auto good_stab = next_recall_stability(curr_diff, curr_stab, ret, Rating::Good);
    const auto good_interval = next_interval(good_stab);
    result.good = build_scheduled_state(card,
                                        Rating::Good,
                                        CardState::Review,
                                        next_difficulty(curr_diff, Rating::Good),
                                        good_stab,
                                        good_interval,
                                        now);
    result.good.card.reps = card.reps + 1;

    // Easy -> Graduate to Review with bonus
    const auto easy_stab = next_recall_stability(curr_diff, curr_stab, ret, Rating::Easy);
    const auto easy_interval = std::max(next_interval(easy_stab), good_interval + 1.0);
    result.easy = build_scheduled_state(card,
                                        Rating::Easy,
                                        CardState::Review,
                                        next_difficulty(curr_diff, Rating::Easy),
                                        easy_stab,
                                        easy_interval,
                                        now);
    result.easy.card.reps = card.reps + 1;

    return result;
}

// ============================================================================
// Schedule Review Card
// ============================================================================

auto FSRSEngine::schedule_review(const Card& card, TimePoint now) const -> SchedulingResult
{
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::hours>(now - card.last_review).count() / 24.0;
    const auto ret = retrievability(std::max(elapsed, 0.0), card.stability);

    SchedulingResult result;

    // Again -> Relearning (lapse)
    const auto again_stab = next_forget_stability(card.difficulty, card.stability, ret);
    result.again = build_scheduled_state(card,
                                         Rating::Again,
                                         CardState::Relearning,
                                         next_difficulty(card.difficulty, Rating::Again),
                                         again_stab,
                                         5.0 / (24.0 * 60.0), // 5 minutes
                                         now);
    result.again.card.lapses = card.lapses + 1;

    // Hard -> Review
    const auto hard_stab =
        next_recall_stability(card.difficulty, card.stability, ret, Rating::Hard);
    const auto hard_interval = next_interval(hard_stab);
    result.hard = build_scheduled_state(card,
                                        Rating::Hard,
                                        CardState::Review,
                                        next_difficulty(card.difficulty, Rating::Hard),
                                        hard_stab,
                                        hard_interval,
                                        now);
    result.hard.card.reps = card.reps + 1;

    // Good -> Review
    const auto good_stab =
        next_recall_stability(card.difficulty, card.stability, ret, Rating::Good);
    const auto good_interval = std::max(next_interval(good_stab), hard_interval + 1.0);
    result.good = build_scheduled_state(card,
                                        Rating::Good,
                                        CardState::Review,
                                        next_difficulty(card.difficulty, Rating::Good),
                                        good_stab,
                                        good_interval,
                                        now);
    result.good.card.reps = card.reps + 1;

    // Easy -> Review with bonus
    const auto easy_stab =
        next_recall_stability(card.difficulty, card.stability, ret, Rating::Easy);
    const auto easy_interval = std::max(next_interval(easy_stab), good_interval + 1.0);
    result.easy = build_scheduled_state(card,
                                        Rating::Easy,
                                        CardState::Review,
                                        next_difficulty(card.difficulty, Rating::Easy),
                                        easy_stab,
                                        easy_interval,
                                        now);
    result.easy.card.reps = card.reps + 1;

    return result;
}

// ============================================================================
// Build a ScheduledState
// ============================================================================

auto FSRSEngine::build_scheduled_state(const Card& card,
                                       Rating rating,
                                       CardState new_state,
                                       double new_difficulty,
                                       double new_stability,
                                       double interval_days,
                                       TimePoint now) const -> ScheduledState
{
    ScheduledState ss;

    // Build review log (captures state BEFORE the review)
    ss.review_log.card_id = card.id;
    ss.review_log.rating = rating;
    ss.review_log.state = card.state;
    ss.review_log.due = card.due;
    ss.review_log.stability = card.stability;
    ss.review_log.difficulty = card.difficulty;
    ss.review_log.elapsed_days = card.elapsed_days;
    ss.review_log.scheduled_days = card.scheduled_days;
    ss.review_log.review = now;

    // Build updated card
    ss.card = card;
    ss.card.state = new_state;
    ss.card.difficulty = new_difficulty;
    ss.card.stability = new_stability;
    ss.card.scheduled_days = interval_days;
    ss.card.last_review = now;

    // Compute elapsed days
    if (card.state == CardState::New)
    {
        ss.card.elapsed_days = 0.0;
    }
    else
    {
        ss.card.elapsed_days =
            std::chrono::duration_cast<std::chrono::hours>(now - card.last_review).count() / 24.0;
    }

    // Compute due date
    const auto interval_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double, std::ratio<86400>>(interval_days));
    ss.card.due = now + interval_duration;

    return ss;
}

} // namespace markamp::core::fsrs
