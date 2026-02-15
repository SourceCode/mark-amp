#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Card State Machine
// New -> Learning -> Review (successful) or Relearning (lapse)
// ============================================================================

enum class CardState
{
    New,       // Never reviewed
    Learning,  // In initial learning phase (short intervals)
    Review,    // In long-term review phase (graduated)
    Relearning // Lapsed: was in Review, answered Again -> re-enter learning
};

[[nodiscard]] auto card_state_to_string(CardState state) -> std::string;
[[nodiscard]] auto string_to_card_state(const std::string& str) -> CardState;

// ============================================================================
// Rating (user's self-assessment of recall quality)
// ============================================================================

enum class Rating : int
{
    Again = 1, // Complete failure to recall
    Hard = 2,  // Recalled with significant difficulty
    Good = 3,  // Recalled with some effort (correct)
    Easy = 4   // Recalled instantly with no effort
};

[[nodiscard]] auto rating_to_string(Rating rating) -> std::string;
[[nodiscard]] auto string_to_rating(const std::string& str) -> Rating;
[[nodiscard]] auto rating_to_int(Rating rating) -> int;

// ============================================================================
// FSRS Parameters (17 trainable weights)
// Default values from the FSRS-4.5 paper
// ============================================================================

struct FSRSParameters
{
    // w[0..3]: initial stability for each rating (Again, Hard, Good, Easy)
    // w[4..5]: initial difficulty parameters
    // w[6]: difficulty mean reversion rate
    // w[7]: stability after lapse base
    // w[8..10]: recall stability growth parameters
    // w[11..14]: forget stability parameters
    // w[15]: hard penalty factor
    // w[16]: easy bonus factor
    double w[17] = {
        0.4,  // w[0]  - initial stability for Again
        0.6,  // w[1]  - initial stability for Hard
        2.4,  // w[2]  - initial stability for Good
        5.8,  // w[3]  - initial stability for Easy
        4.93, // w[4]  - initial difficulty base
        0.94, // w[5]  - initial difficulty scaling
        0.86, // w[6]  - difficulty mean reversion rate
        0.01, // w[7]  - (reserved/stability after lapse)
        1.49, // w[8]  - stability growth exp base
        0.14, // w[9]  - stability growth difficulty factor
        0.94, // w[10] - stability growth retrievability factor
        2.18, // w[11] - forget stability base
        0.05, // w[12] - forget stability difficulty factor
        0.34, // w[13] - forget stability previous stability factor
        1.26, // w[14] - forget stability retrievability factor
        0.29, // w[15] - hard penalty factor
        2.61  // w[16] - easy bonus factor
    };

    /// Maximum interval in days.
    double maximum_interval{36500.0}; // 100 years

    /// Desired retention rate (0.0 to 1.0). Default 0.9 = 90% target recall.
    double request_retention{0.9};

    [[nodiscard]] auto operator==(const FSRSParameters& other) const -> bool = default;
};

// ============================================================================
// Card: the schedulable unit
// ============================================================================

using TimePoint = std::chrono::system_clock::time_point;

struct Card
{
    std::string id;      // Unique card ID (UUID)
    std::string deck_id; // Deck this card belongs to

    // ── Scheduling state ──
    TimePoint due;                   // When this card is next due for review
    double stability{0.0};           // Memory stability (days until R drops to 90%)
    double difficulty{0.0};          // Item difficulty (1.0 = easiest, 10.0 = hardest)
    double elapsed_days{0.0};        // Days since last review
    double scheduled_days{0.0};      // Days until next review (interval)
    int reps{0};                     // Total number of successful reviews
    int lapses{0};                   // Total number of times the card was forgotten
    CardState state{CardState::New}; // Current state in the state machine
    TimePoint last_review;           // Timestamp of last review

    // ── Content reference ──
    std::string block_id;      // Reference to the Markdown block (front/back)
    std::string front_content; // Cached front text
    std::string back_content;  // Cached back text

    [[nodiscard]] auto operator==(const Card& other) const -> bool = default;
};

// ============================================================================
// Review Log: immutable record of a single review
// ============================================================================

struct ReviewLog
{
    std::string id;                  // Unique log ID
    std::string card_id;             // Which card was reviewed
    Rating rating{Rating::Good};     // User's rating
    CardState state{CardState::New}; // Card state BEFORE the review
    TimePoint due;                   // When the card was due
    double stability{0.0};           // Stability BEFORE the review
    double difficulty{0.0};          // Difficulty BEFORE the review
    double elapsed_days{0.0};        // Days since previous review
    double scheduled_days{0.0};      // Interval that was scheduled
    TimePoint review;                // When the review actually happened

    [[nodiscard]] auto operator==(const ReviewLog& other) const -> bool = default;
};

// ============================================================================
// Scheduled State: the card state after applying a specific rating
// ============================================================================

struct ScheduledState
{
    Card card;            // Updated card with new scheduling parameters
    ReviewLog review_log; // Log entry for this review
};

// ============================================================================
// Scheduling Result: shows what happens for EACH possible rating
// The UI shows all 4 options (Again/Hard/Good/Easy) with their intervals
// ============================================================================

struct SchedulingResult
{
    ScheduledState again; // What happens if user rates Again
    ScheduledState hard;  // What happens if user rates Hard
    ScheduledState good;  // What happens if user rates Good
    ScheduledState easy;  // What happens if user rates Easy

    /// Get the scheduled state for a specific rating.
    [[nodiscard]] auto for_rating(Rating rating) const -> const ScheduledState&;

    /// Get display intervals for the review buttons (e.g. "1m", "10m", "1d", "4d").
    [[nodiscard]] auto interval_display(Rating rating) const -> std::string;
};

// ============================================================================
// Deck: a collection of cards
// ============================================================================

struct Deck
{
    std::string id;   // Unique deck ID
    std::string name; // Display name
    std::string description;
    int new_cards_per_day{20};    // Maximum new cards introduced per day
    int max_reviews_per_day{200}; // Maximum reviews per day
    FSRSParameters params;        // Per-deck FSRS parameters (override global)
    bool use_global_params{true}; // If true, use global params instead of per-deck

    [[nodiscard]] auto operator==(const Deck& other) const -> bool = default;
};

// ============================================================================
// Due Card Counts
// ============================================================================

struct DueCardCounts
{
    int new_count{0};      // Cards in New state
    int learning_count{0}; // Cards in Learning/Relearning state
    int review_count{0};   // Cards in Review state that are due
    int total_due{0};      // Sum of all due cards

    [[nodiscard]] auto is_empty() const -> bool
    {
        return total_due == 0;
    }
};

} // namespace markamp::core::fsrs
