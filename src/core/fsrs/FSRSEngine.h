#pragma once

#include "FSRSTypes.h"

#include <chrono>

namespace markamp::core::fsrs
{

/// Core FSRS v3 scheduling engine.
/// All mathematical formulas are implemented as pure functions.
/// Thread-safe: the engine is stateless (all state is in the Card struct).
class FSRSEngine
{
public:
    explicit FSRSEngine(FSRSParameters params = FSRSParameters{});

    // ── Primary API ──

    /// Schedule a card after a review with the given rating.
    /// Returns all 4 possible outcomes (Again/Hard/Good/Easy).
    [[nodiscard]] auto schedule(const Card& card, TimePoint now) const -> SchedulingResult;

    /// Apply a specific rating to a card. Returns the updated card and review log.
    [[nodiscard]] auto review(const Card& card, Rating rating, TimePoint now) const
        -> ScheduledState;

    /// Calculate the retrievability (probability of recall) for a card at a given time.
    /// Returns a value between 0.0 and 1.0.
    [[nodiscard]] auto get_retrievability(const Card& card, TimePoint now) const -> double;

    // ── Parameter access ──

    [[nodiscard]] auto parameters() const -> const FSRSParameters&;
    void set_parameters(FSRSParameters params);

    // ── Core FSRS Formulas (public for testing) ──

    /// Calculate initial stability for a new card based on first rating.
    /// Formula: S_0(G) = w[G-1]
    [[nodiscard]] auto initial_stability(Rating rating) const -> double;

    /// Calculate initial difficulty for a new card based on first rating.
    /// Formula: D_0(G) = w[4] - exp(w[5] * (G - 1)) + 1
    /// Clamped to [1, 10].
    [[nodiscard]] auto initial_difficulty(Rating rating) const -> double;

    /// Calculate next difficulty after a review.
    /// Formula: D'(D, G) = w[7] * D_0(4) + (1 - w[7]) * (D - w[6] * (G - 3))
    /// (mean reversion toward initial difficulty of Easy rating)
    /// Clamped to [1, 10].
    [[nodiscard]] auto next_difficulty(double current_difficulty, Rating rating) const -> double;

    /// Calculate next stability after a SUCCESSFUL recall (Review state).
    /// Formula: S'_r(D, S, R, G) = S * (1 + exp(w[8]) * (11 - D) *
    ///          S^(-w[9]) * (exp((1-R) * w[10]) - 1) *
    ///          (G==Hard ? w[15] : G==Easy ? w[16] : 1))
    [[nodiscard]] auto next_recall_stability(double difficulty,
                                             double stability,
                                             double retrievability,
                                             Rating rating) const -> double;

    /// Calculate next stability after a FAILED recall (lapse).
    /// Formula: S'_f(D, S, R) = w[11] * D^(-w[12]) *
    ///          ((S+1)^w[13] - 1) * exp((1-R) * w[14])
    [[nodiscard]] auto next_forget_stability(double difficulty,
                                             double stability,
                                             double retrievability) const -> double;

    /// Calculate retrievability (recall probability) given elapsed days and stability.
    /// Formula: R(t, S) = (1 + t / (9 * S))^(-1)
    /// This is the power forgetting curve from the FSRS model.
    [[nodiscard]] static auto retrievability(double elapsed_days, double stability) -> double;

    /// Calculate the interval (in days) to achieve a target retrievability.
    /// Formula: I(R, S) = 9 * S * (1/R - 1)
    /// Used to convert stability to a concrete review interval.
    [[nodiscard]] auto next_interval(double stability) const -> double;

    /// Mean reversion function: blends a value toward a mean.
    /// Formula: mean_reversion(init, current) = w[7] * init + (1 - w[7]) * current
    [[nodiscard]] auto mean_reversion(double init, double current) const -> double;

private:
    FSRSParameters params_;

    /// Clamp difficulty to [1, 10].
    [[nodiscard]] static auto clamp_difficulty(double diff) -> double;

    /// Clamp interval to [1, maximum_interval].
    [[nodiscard]] auto clamp_interval(double interval) const -> double;

    /// Schedule a New card.
    [[nodiscard]] auto schedule_new(const Card& card, TimePoint now) const -> SchedulingResult;

    /// Schedule a Learning/Relearning card.
    [[nodiscard]] auto schedule_learning(const Card& card, TimePoint now) const -> SchedulingResult;

    /// Schedule a Review card.
    [[nodiscard]] auto schedule_review(const Card& card, TimePoint now) const -> SchedulingResult;

    /// Build a ScheduledState for a specific rating transition.
    [[nodiscard]] auto build_scheduled_state(const Card& card,
                                             Rating rating,
                                             CardState new_state,
                                             double new_difficulty,
                                             double new_stability,
                                             double interval_days,
                                             TimePoint now) const -> ScheduledState;
};

} // namespace markamp::core::fsrs
